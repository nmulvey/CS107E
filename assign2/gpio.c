/* File: gpio.c
 * ------------
 * Nicole Mulvey
 * This contains set, get, write, and read for gipo library
 */
#include "gpio.h"
#include <stddef.h>

enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };

typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

// The gpio_id_t enumeration assigns a symbolic constant for each
// in such a way to use a single hex constant. The more significant
// hex digit identifies the group and lower 2 hex digits are pin index:
//       constant 0xNnn  N = which group,  nn = pin index within group
//
// This helper function extracts the group and pin index from a gpio_id_t
// e.g. GPIO_PB4 belongs to GROUP_B and has pin_index 4
static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

// The gpio groups are differently sized, e.g. B has 13 pins, C only 8.
// This helper function confirms that a gpio_id_t is valid (group
// and pin index are valid)
bool gpio_id_is_valid(gpio_id_t pin) {
    gpio_pin_t gp = get_group_and_index(pin);
    switch (gp.group) {
        case GROUP_B: return (gp.pin_index <= GPIO_PB_LAST_INDEX);
        case GROUP_C: return (gp.pin_index <= GPIO_PC_LAST_INDEX);
        case GROUP_D: return (gp.pin_index <= GPIO_PD_LAST_INDEX);
        case GROUP_E: return (gp.pin_index <= GPIO_PE_LAST_INDEX);
        case GROUP_F: return (gp.pin_index <= GPIO_PF_LAST_INDEX);
        case GROUP_G: return (gp.pin_index <= GPIO_PG_LAST_INDEX);
        default:      return false;
    }
}

// This helper function is suggested to return the address of
// the config0 register for a gpio group, i.e. get_cfg0_reg(GROUP_B)
// Refer to the D1 user manual to learn the address the config0 register
// for each group. Be sure to note how the address of the config1 and
// config2 register can be computed as relative offset from config0.
static volatile unsigned int *get_cfg0_reg(unsigned int group) {

        switch (group) {
            case GROUP_B:
                return (volatile unsigned int *) 0x02000030;  // PB_CFG0 offset
            case GROUP_C:
                return (volatile unsigned int *) 0x02000060;  // PC_CFG0 offset
            case GROUP_D:
                return (volatile unsigned int *) 0x02000090;  // PD_CFG0 offset
            case GROUP_E:
                return (volatile unsigned int *) 0x020000C0;  // PE_CFG0 offset
            case GROUP_F:
                return (volatile unsigned int *) 0x020000F0;  // PF_CFG0 offset
            case GROUP_G:
                return (volatile unsigned int *) 0x02000120;  // PG_CFG0 offset
            default:
                return NULL;  // Invalid group, return NULL
            }

}

// This helper function is suggested to return the address of
// the data register for a gpio group. Refer to the D1 user manual
// to learn the address of the data register for each group.
static volatile unsigned int *get_data_reg(unsigned int group) {
    /***** TODO: Your code goes here *****/
    switch (group) {
            case GROUP_B:
                return (volatile unsigned int *) 0x02000040;  // PB_CFG0 data_reg 
            case GROUP_C:
                return (volatile unsigned int *) 0x02000070;  // PC_CFG0 data_reg 
            case GROUP_D:
                return (volatile unsigned int *) 0x020000A0;  // PD_CFG0 data_reg
            case GROUP_E:
                return (volatile unsigned int *) 0x020000D0;  // PE_CFG0 data_reg 
            case GROUP_F:
                return (volatile unsigned int *) 0x02000100;  // PF_CFG0 data_reg 
            case GROUP_G:
                return (volatile unsigned int *) 0x02000130;  // PG_CFG0 data_reg
            default:
                return NULL;  // Invalid group, return NULL
            }

    return NULL;
}

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_input(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_INPUT);
}

void gpio_set_output(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_OUTPUT);
}

void gpio_set_function(gpio_id_t pin, unsigned int function) {
   /***** TODO: Your code goes here *****/

   //used check if a valid input
   if (!gpio_id_is_valid(pin) || function > 0xF) {
        return;  
    }


    gpio_pin_t gp = get_group_and_index(pin);
    gpio_id_t gp_pin = gp.pin_index;
    gpio_id_t group = gp.group; 

    volatile unsigned int *cfg_reg_add = get_cfg0_reg(group); //getting register
    
    
    volatile unsigned int *reg_addr = cfg_reg_add + (gp_pin / 8);  // used to select register

    //Find the bit position 
    unsigned int bit_position = (gp_pin % 8) * 4;  // Used to get pin's position 

    
    unsigned int mask = ~(0xF << bit_position);  // Create a mask 
    unsigned int sft_function = (function << bit_position);  

    // perform to update the register
    *reg_addr = ((*reg_addr & mask) | sft_function);

}


unsigned int gpio_get_function(gpio_id_t pin) {
    bool pin_val = gpio_id_is_valid(pin); //new check
    if (pin_val == false) {
        return -1;  // Return -1 for an invalid pin
    }

    gpio_pin_t gp = get_group_and_index(pin);  // extract group and pin index from pin
    unsigned int group = gp.group;
    unsigned int pin_index = gp.pin_index;

    volatile unsigned int *cfg_base = get_cfg0_reg(group);
   
    if (cfg_base == NULL) {
        return GPIO_INVALID_REQUEST;  // Return failure if the group is invalid
    }

    //calculate the correct register for the pin CFG0, etc.
    volatile unsigned int *cfg_reg_add = cfg_base + (pin_index / 8);   

    //Find the bit position 
    unsigned int bit_position = (pin_index % 8) * 4;  

    
    unsigned int function = (*cfg_reg_add >> bit_position) & 0xF;  

    return function;
}

void gpio_write(gpio_id_t pin, int value) {
    /***** TODO: Your code goes here *****/

    
    gpio_pin_t gp = get_group_and_index(pin);

    gpio_id_t gp_pin = gp.pin_index;
    gpio_id_t group = gp.group;

    volatile unsigned int *dat_reg = get_data_reg(group);


    if (value == 1) {
        // Set the pin to 1 
        *dat_reg |= (1 << gp_pin);
    } else {
        // Set the pin to 0
        *dat_reg &= ~(1 << gp_pin);
    }

}

int gpio_read(gpio_id_t pin) {
    /***** TODO: Your code goes here *****/
    bool pin_val = gpio_id_is_valid(pin); //new check
    if (pin_val == false) {
        return -1;  // Return -1 for an invalid pin
    }

    gpio_pin_t gp = get_group_and_index(pin);
    gpio_id_t gp_pin = gp.pin_index;
    gpio_id_t group = gp.group;

    
    volatile unsigned int *dat_reg = get_data_reg(group);
    if (dat_reg == NULL) {
        return 0;  // invalid data_reg
    }

    int pin_state = (*dat_reg >> gp_pin) & 0x1;  

    return pin_state;  // Return 1 if high, 0 if low
    
}