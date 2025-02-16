/* File: ps2_assign7.c
 * -------------------
 * ***** TODO: add your file header comment here *****
 * 
 * name: Nicole Mulvey
 * This ps2 code handles the interrupt 
 */

// Copy/paste your code from assign5/ps2.c as starting point


#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "uart.h" 
#include "ringbuffer.h"
#include "gpio_interrupt.h"
#include "timer.h"
#include "printf.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the gpio id for the
// clock pin, and the data field stores the gpio id for the data pin.
// Read ps2_new for example code that sets and uses these fields.
//
// You may extend the ps2_device structure with additional fields as needed.
// A pointer to the current ps2_device is passed into all ps2_ calls.
// Storing state in this structure is preferable to using global variables:
// it allows your driver to support multiple PS2 devices accessed concurrently
// (e.g., a keyboard and a mouse).
//
// This definition fills out the structure declared in ps2.h.
struct ps2_device {
    gpio_id_t clock;
    gpio_id_t data;
    volatile uint8_t scancode; //scancode
    volatile int bit_count; //bits processed
    volatile bool ready; //scancode ready
    volatile uint8_t parity_bit; //parity bit
    rb_t *buffer;   //ringbuf
    volatile bool is_extended;
    volatile bool is_break_code;
    volatile uint8_t prev_scancode;
};


static void ps2_interrupt_handler(void *aux_data){

    ps2_device_t *dev = (ps2_device_t *) aux_data;

    //read current bit
    uint8_t bit = gpio_read(dev->data);

    //switch case for bit count
    switch (dev->bit_count) {
        case 0:
            //check start bit
            if (bit != 0) {
                //if invalid reset 
                dev->bit_count = 0;
                dev->scancode = 0;
            } else {
                dev->scancode = 0;     // Reset scancode
                dev->parity_bit = 0;
                dev->bit_count++;
            }
            break;
        case 1 ... 8:
            //these are for the data bits to put them into the scancode
            dev->scancode |= (bit << (dev->bit_count - 1));
            dev->bit_count++;
            break;
        case 9:
            //getting the parity bit for later
            dev->parity_bit = bit;
            uint8_t data_bits = dev->scancode & 0xFF;
            uint8_t parity = dev->parity_bit;
            

            for (int i = 0; i < 8; i++) {

                parity ^= (data_bits >> i) & 0x1;
            }

            if (parity != 1) {
                    //parity error and reset 
                dev->bit_count = 0;
                dev->scancode = 0;
                dev->parity_bit = 0;
                dev->is_extended = false;
                dev->is_break_code = false;

                
            } else {
                dev->bit_count++;
            }


            break;
        case 10:
            //stop bit should be 1
            if (bit != 1) {
                //if invalid 
                dev->bit_count = 0;
                dev->scancode = 0;
                dev->is_extended = false;
                dev->is_break_code = false;
               
            } else {
                
                uint8_t scancode = dev->scancode;

                if (scancode == 0xE0) {
                    // Extended scancode prefix
                    dev->is_extended = true;
                } else if (scancode == 0xF0) {
                    // Break code prefix
                    dev->is_break_code = true;
                    dev->prev_scancode = scancode;
                } else {
                    if (!dev->is_break_code) {
                     if (dev->is_extended) {
                        // Add specific handling for extended keys
                        // For example, numpad keys
                        switch (scancode) {
                            case 0x75: scancode = 0xE0 | 0x75; break; // up arrow
                            case 0x72: scancode = 0xE0 | 0x72; break; // down arrow
                            case 0x6B: scancode = 0xE0 | 0x6B; break; // left arrow
                            case 0x74: scancode = 0xE0 | 0x74; break; // right arrow
                            // Add more extended key mappings as needed
                        }
                     }
                    

                    if (!dev->is_break_code && !rb_full(dev->buffer)) {
                        // Special handling for backspace (scancode 0x66)
                        if (scancode == 0x66 || scancode == 0x08) {
                            rb_enqueue(dev->buffer, 0x66);
                        } else {
                            rb_enqueue(dev->buffer, scancode);
                        }
                    }
                    }
                 // Reset flags
                    dev->is_extended = false;
                    dev->is_break_code = false;
                }

                // Reset for next scancode
                dev->bit_count = 0;
                dev->scancode = 0;
            }

            break;   
                

        default:
            //something went wrong and we should reset
            dev->bit_count = 0;
            dev->scancode = 0;
            dev->parity_bit = 0;
            dev->is_extended = false;
            dev->is_break_code = false;
            break;
    }


    gpio_interrupt_clear(dev->clock);
}

static uint8_t read_bit(ps2_device_t *dev) {
    //wait to go low
    while (gpio_read(dev->clock)) {
        
    }

    
    bool bit = gpio_read(dev->data);

    //wait to go high
    while (!gpio_read(dev->clock)) {
        
    }

    return bit ? 1 : 0;
}




ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio){

    static bool gpio_interrupt_initialized = false;

    //check and see if GPIO system is initizalized 
    if (!gpio_interrupt_initialized) {

        gpio_interrupt_init();

        gpio_interrupt_initialized = true;
    }
    
    //allocate memory 
    ps2_device_t *dev = malloc(sizeof(*dev));

    //set up the clock GPIO
    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    //set up data GPIO
    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    
    //inizalize states
    dev->scancode = 0;
    dev->bit_count = 0;
    dev->ready = false;
    dev->parity_bit = 0;

    //make new ring buf
    dev->buffer = rb_new();

    if (dev->buffer == NULL) {
        //ring buf failed to make 
        free(dev);
        return NULL;
    }

    gpio_interrupt_config(dev->clock, GPIO_INTERRUPT_NEGATIVE_EDGE, false);

    gpio_interrupt_register_handler(dev->clock, ps2_interrupt_handler, dev);

    gpio_interrupt_enable(dev->clock);

    return dev;
}


uint8_t ps2_read(ps2_device_t *dev) {
    /**** TODO: your code goes here *****/

   
    //wait until there is data
    while (rb_empty(dev->buffer)) {
        
    }

    interrupts_global_disable();
    //get out from the queue
    int scancode;
    rb_dequeue(dev->buffer, &scancode);

    interrupts_global_enable();

    if ((scancode & 0xE0) == 0xE0) {
        // This is an extended key - handle accordingly
        return scancode & 0x7F;  // Strip the extended bit
    }

    //return data
    return (uint8_t)scancode;
}

