// /* File: clock.c
//  * -------------
//  * Nicole Mulvey
//  * This file is for the clock diplay, refresh loop, and countdown
//  */
// #include "gpio.h"
// #include "timer.h"


// unsigned char binary_pattern[10] = {
//     0b00111111,  //0
//     0b00000110,  //1
//     0b01011011,  //2
//     0b01001111,  //3 
//     0b01100110,  //4
//     0b01101101,  //5
//     0b01111101,  //6
//     0b00000111,  //7 
//     0b01111111,  //8 
//     0b01101111   //9

// };

// void seting_segment(int binary_pattern) {

//     gpio_write(GPIO_PD17, (binary_pattern >> 0) & 0x1);  //A
//     gpio_write(GPIO_PB6,  (binary_pattern >> 1) & 0x1);  //B
//     gpio_write(GPIO_PB12, (binary_pattern >> 2) & 0x1);  //C
//     gpio_write(GPIO_PB11, (binary_pattern >> 3) & 0x1);  //D
//     gpio_write(GPIO_PB10, (binary_pattern >> 4) & 0x1);  //E
//     gpio_write(GPIO_PE17, (binary_pattern >> 5) & 0x1);  //F
//     gpio_write(GPIO_PD11, (binary_pattern >> 6) & 0x1);  //G
// }


// void display_digit(int digit, int digit_pin) {
//     if (digit < 0 || digit > 9) {
//         return;  // Invalid digit
//     }

//     unsigned char pattern = binary_pattern[digit];
//     seting_segment(pattern);  // Set the segments based on the pattern

//     gpio_write(digit_pin, 1);   // Turn on the digit
//     timer_delay_us(2500);       // Wait for 2500 microseconds
//     gpio_write(digit_pin, 0);   // Turn off the digit
// }



// void display_time(int minutes, int seconds) {
//     int min_tens_place = minutes / 10;
//     int min_ones_place = minutes % 10;
//     int sec_tens_place = seconds / 10;
//     int sec_ones_place = seconds % 10;

//     display_digit(min_tens_place, GPIO_PB4);  //Tens in Minutes
//     display_digit(min_ones_place, GPIO_PB3);  //Ones in Minutes
//     display_digit(sec_tens_place, GPIO_PB2);  //Tens in Seconds
//     display_digit(sec_ones_place, GPIO_PC0);  //Ones in Seconds
// }

// void countdown(int minutes, int seconds) {
//     //get current ticks
//     int start_tick = timer_get_ticks();
//     int num_ticks = 0;

//     while (minutes > 0 || seconds > 0) {
//         //current time
//         display_time(minutes, seconds);

//         num_ticks++;
//         // Decrement the time
//         if (timer_get_ticks() >= start_tick + 1000000 * TICKS_PER_USEC){
//           start_tick = timer_get_ticks();
//             if (seconds == 0) {
//                 if (minutes > 0) {
//                     minutes--;
//                     seconds = 59;
//                 } else {
                    
//                     minutes = 0;
//                     seconds = 0;
//                     break; 
//                 }
//             } else {
//                 seconds--;
//             }
//     }
// }
// }

// void end_pattern() {
//     for (int i = 0; i < 10; i++) {

//          // blink "d" 
//         seting_segment( 0b01011110);
//         gpio_write(GPIO_PB4, 1);   
//         timer_delay_ms(200);  

        
//         seting_segment(0b00000000);  // Turn off all segments
//         gpio_write(GPIO_PB4, 0);
//         timer_delay_ms(200);  

//         // bink "o" 
//         seting_segment( 0b01011100);
//         gpio_write(GPIO_PB3, 1);   
//         timer_delay_ms(200);  

       
//         seting_segment(0b00000000);
//         gpio_write(GPIO_PB3, 0);
//         timer_delay_ms(200);

//         // blink "n"
//         seting_segment( 0b01010100);
//         gpio_write(GPIO_PB2, 1);   
//         timer_delay_ms(200); 

        
//         seting_segment(0b00000000);
//         gpio_write(GPIO_PB2, 0);
//         timer_delay_ms(200);

//         // blink "e" 
//         seting_segment( 0b01111001);
//         gpio_write(GPIO_PC0, 1);  
//         timer_delay_ms(200);  

    
//         seting_segment(0b00000000);
//         gpio_write(GPIO_PC0, 0);
//         timer_delay_ms(200);
//     }
// }

// void main(void) {
//     /***** TODO: Your code goes here *****/

//     gpio_set_input(GPIO_PB1); //Button GPIO
//     gpio_set_output(GPIO_PB4);
//     gpio_set_output(GPIO_PB3);
//     gpio_set_output(GPIO_PB2);
//     gpio_set_output(GPIO_PC0);


//     gpio_set_output(GPIO_PD17);  //A
//     gpio_set_output(GPIO_PB6);   //B
//     gpio_set_output(GPIO_PB12);  //C
//     gpio_set_output(GPIO_PB11);  //D
//     gpio_set_output(GPIO_PB10);  //E
//     gpio_set_output(GPIO_PE17);  //F
//     gpio_set_output(GPIO_PD11);  //G
//     gpio_init();

//     int minutes = 1;
//     int seconds = 7;

//     while (gpio_read(GPIO_PB1) == 1) { //wait for button
         
//     }
//     //countdown 
//     countdown(minutes, seconds);
//     end_pattern();


// }

/* File: clock.c
 * -------------
 * Nicole Mulvey
 * This file is for the clock Extension
 */
#include "gpio.h"
#include "timer.h"


unsigned char binary_pattern[10] = {
    0b00111111,  //0
    0b00000110,  //1
    0b01011011,  //2
    0b01001111,  //3 
    0b01100110,  //4
    0b01101101,  //5
    0b01111101,  //6
    0b00000111,  //7 
    0b01111111,  //8 
    0b01101111   //9

};

void seting_segment(int binary_pattern) {

    gpio_write(GPIO_PD17, (binary_pattern >> 0) & 0x1);  //A
    gpio_write(GPIO_PB6,  (binary_pattern >> 1) & 0x1);  //B
    gpio_write(GPIO_PB12, (binary_pattern >> 2) & 0x1);  //C
    gpio_write(GPIO_PB11, (binary_pattern >> 3) & 0x1);  //D
    gpio_write(GPIO_PB10, (binary_pattern >> 4) & 0x1);  //E
    gpio_write(GPIO_PE17, (binary_pattern >> 5) & 0x1);  //F
    gpio_write(GPIO_PD11, (binary_pattern >> 6) & 0x1);  //G
}


void display_digit(int digit, int digit_pin, int highlight) {
    if (digit < 0 || digit > 9) {
        return;  // Invalid digit
    }

    unsigned char pattern = binary_pattern[digit];
    //tried to get to blink :/
    if (highlight) {
        pattern |= 0b00000000;  
    }

    seting_segment(pattern);  // Set the segments based on the pattern

    gpio_write(digit_pin, 1);   // Turn on the digit
    timer_delay_us(2500);       // Wait for 2500 microseconds
    gpio_write(digit_pin, 0);   // Turn off the digit
}



//Old display time
void display_time(int minutes, int seconds, int current_digit) {
    int min_tens_place = minutes / 10;
    int min_ones_place = minutes % 10;
    int sec_tens_place = seconds / 10;
    int sec_ones_place = seconds % 10;


    display_digit(min_tens_place, GPIO_PB4, current_digit == 0);  //Tens in Minutes
    display_digit(min_ones_place, GPIO_PB3, current_digit == 1);  //Ones in Minutes
    display_digit(sec_tens_place, GPIO_PB2, current_digit == 2);  //Tens in Seconds
    display_digit(sec_ones_place, GPIO_PC0, current_digit == 3);  //Ones in Seconds
}

void countdown(int minutes, int seconds) {
    //get current ticks
    int start_tick = timer_get_ticks();
    int num_ticks = 0;

    while (minutes > 0 || seconds > 0) {
        // Display the current time
        display_time(minutes, seconds, -1); 
       
        num_ticks++;
        // Decrement the time
        if (timer_get_ticks() >= start_tick + 1000000 * TICKS_PER_USEC){
          start_tick = timer_get_ticks();
            if (seconds == 0) {
                if (minutes > 0) {
                    minutes--;
                    seconds = 59;
                } else {
                    // Time's done !!!!
                    minutes = 0;
                    seconds = 0;
                    break; 
                }
            } else {
                seconds--;
            }
    }
}
}

void set_LED(int BLUE, int GREEN, int RED) {

    gpio_write(GPIO_PD21, RED);
    gpio_write(GPIO_PD22, GREEN);
    gpio_write(GPIO_PB0, BLUE);

}

void end_pattern() {

    for (int i = 0; i < 5; i++) {
        // Turn off all LEDs
        set_LED(0, 0, 0);

        // Flash Red
        set_LED(0, 0, 1);
        timer_delay_ms(500);
        set_LED(0, 0, 1);
        timer_delay_ms(100);

        // Flash Green
        set_LED(0, 1, 0);
        timer_delay_ms(500);
        set_LED(0, 1, 0);
        timer_delay_ms(100);

        // Flash Blue
        set_LED(1, 0, 0);
        timer_delay_ms(500);
        set_LED(1, 0, 0);
        timer_delay_ms(100);
    }

    // Turn off all LEDs at the end
    gpio_write(GPIO_PD12, 0);
    gpio_write(GPIO_PD13, 0);
    gpio_write(GPIO_PD11 , 0);
}

int read_rotary(int *last_A) {
    int result = 0;
    int A = gpio_read(GPIO_PD15);
    int B = gpio_read(GPIO_PE16);

    if (A != *last_A) {  // if A Pin aka GPIO PD15 has changed
        if (A == 1) {  // Rising edge ??? I hate digital 
            if (B == 0) {
                // Clockwise 
                result = 1;
            } else {
                // Counter-clockwise 
                result = -1;
            }
        }
    }
    *last_A = A;
    return result;
}

void main(void) {
    /***** TODO: Your code goes here *****/

    //gpio_set_input(GPIO_PB1); //Button GPIO
    gpio_set_input(GPIO_PD14); //Rotary Button 
    gpio_set_output(GPIO_PB4);
    gpio_set_output(GPIO_PB3);
    gpio_set_output(GPIO_PB2);
    gpio_set_output(GPIO_PC0);


    gpio_set_output(GPIO_PD17);  //A
    gpio_set_output(GPIO_PB6);   //B
    gpio_set_output(GPIO_PB12);  //C
    gpio_set_output(GPIO_PB11);  //D
    gpio_set_output(GPIO_PB10);  //E
    gpio_set_output(GPIO_PE17);  //F
    gpio_set_output(GPIO_PD11);  //G

    gpio_set_output(GPIO_PD21);  //first pin of LED for Extension
    gpio_set_output(GPIO_PD22);  //second pin of LED for Extension
    gpio_set_output(GPIO_PB0);  //third pin of LED for Extension

    gpio_set_input(GPIO_PD15);
    gpio_set_input(GPIO_PE16);
    gpio_init();


    int last_A = gpio_read(GPIO_PD15); 
    
    int min_tens = 0;
    int min_ones = 0;
    int sec_tens = 0;
    int sec_ones = 7;

    int current_digit = 0;  // Start with the minutes tens place

    int button_last_state = gpio_read(GPIO_PD14);
    int button_pressed = 0;

    while (1) {
        
        int movement = read_rotary(&last_A); //read rotary 
        if (movement != 0) {
            // Adjust the current digit based on rotation
            switch (current_digit) {
                case 0: 
                    min_tens += movement;
                    if (min_tens > 9) min_tens = 0;  
                    if (min_tens < 0) min_tens = 9;
                    break;
                case 1:  
                    min_ones += movement;
                    if (min_ones > 9) min_ones = 0;
                    if (min_ones < 0) min_ones = 9;
                    break;
                case 2:  
                    sec_tens += movement;
                    if (sec_tens > 5) sec_tens = 0;  // Max 59 seconds
                    if (sec_tens < 0) sec_tens = 5;
                    break;
                case 3:  
                    sec_ones += movement;
                    if (sec_ones > 9) sec_ones = 0;
                    if (sec_ones < 0) sec_ones = 9;
                    break;
                default:
                    break;
            }
        }

        int minutes = min_tens * 10 + min_ones;
        int seconds = sec_tens * 10 + sec_ones;

        display_time(minutes, seconds, -1);

        // Read the button state
        int button_state = gpio_read(GPIO_PD14);
        if (button_state == 0 && button_last_state == 1) {
            // Button has been pressed
            button_pressed = 1;
        }
        button_last_state = button_state;

        if (button_pressed) {
            button_pressed = 0;
            current_digit++;
            if (current_digit > 3) {
            
                break;
            }
        }

        // debounce 
        timer_delay_ms(5);
    }

    int minutes = min_tens * 10 + min_ones;
    int seconds = sec_tens * 10 + sec_ones;
    
    countdown(minutes, seconds);
    end_pattern();


}