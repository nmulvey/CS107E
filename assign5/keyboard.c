/* File: keyboard.c
 * -----------------
 * ***** TODO: add your file header comment here *****
 * name: Nicole Mulvey
 * This is the Keyboard driver
 */

#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;


void keyboard_init(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    dev = ps2_new(clock_gpio, data_gpio);
}

uint8_t keyboard_read_scancode(void) {
    return ps2_read(dev);
}

key_action_t keyboard_read_sequence(void) {
    /***** TODO: Your code goes here *****/
    key_action_t action = { 0 };

    uint8_t first_byte = keyboard_read_scancode();
    uint8_t second_byte = 0;
    uint8_t third_byte = 0;

    if (first_byte == 0xE0){ //see if it an extended scancode

        second_byte = keyboard_read_scancode(); 

        if(second_byte == 0xF0){ //check if release
            third_byte = keyboard_read_scancode();
            action.what = KEY_RELEASE;
            action.keycode = third_byte;
            
        }else{ //check if regular scancode
            action.what = KEY_PRESS;
            action.keycode = second_byte;
        }

    }else if (first_byte == 0xF0){ //check if release

        second_byte = keyboard_read_scancode();
        action.what = KEY_RELEASE;
        action.keycode = second_byte;

    } else{ //then it is key press

        action.what = KEY_PRESS;
        action.keycode = first_byte;
    }

    return action;
}

key_event_t keyboard_read_event(void) {
    /***** TODO: Your code goes here *****/
    key_event_t event = { 0 };

    static keyboard_modifiers_t modifers = 0;

    static int caps_lock_key_pressed = 0;
    

   while (1){

    key_action_t action = keyboard_read_sequence();

    uint8_t scancode = action.keycode; //get scancode

    ps2_key_t key = ps2_keys[scancode]; //look up info

    if(key.ch == PS2_KEY_SHIFT){

        if(action.what == KEY_PRESS){ //if Shift is pressed

            modifers |= KEYBOARD_MOD_SHIFT; //set Shift modifer

        }else if (action.what == KEY_RELEASE){

            modifers &= ~KEYBOARD_MOD_SHIFT; //clear shift modifer
        }
        continue;

    } else if (key.ch == PS2_KEY_CTRL){ //see if CTRL is pressed

        if(action.what == KEY_PRESS){

            modifers |= KEYBOARD_MOD_CTRL; //set CTRL

        }else if (action.what == KEY_RELEASE){

            modifers &= ~KEYBOARD_MOD_CTRL; //clear CTRL
        }
        continue;

    }else if (key.ch == PS2_KEY_ALT){ //see if ALT is preressed

        if(action.what == KEY_PRESS){

            modifers |= KEYBOARD_MOD_ALT; //Set ALT 

        }else if (action.what == KEY_RELEASE){

            modifers &= ~KEYBOARD_MOD_ALT; //Clear ALT
        }
        continue;

    } else if ( key.ch == PS2_KEY_CAPS_LOCK){ //See if CAPS is pressed

        if(action.what == KEY_PRESS){
            if (!caps_lock_key_pressed) {
                modifers ^= KEYBOARD_MOD_CAPS_LOCK; //Set CAPS as sticky
                caps_lock_key_pressed = 1;
            }
        }else if (action.what == KEY_RELEASE) {
            caps_lock_key_pressed = 0;
        }

        continue;

    }else if (key.ch >= PS2_KEY_SHIFT) {
            // Handle special keys (including arrow keys)
            event.action = action;
            event.key = key;
            event.modifiers = modifers;
            return event;
    
    } else{

        event.action = action;

        event.key = key;

        event.modifiers = modifers;

    
        return event;
   }
   }

}


char keyboard_read_next(void) {
    /***** TODO: Your code goes here *****/

    while (1) {

        key_event_t event = keyboard_read_event();


        if (event.action.what != KEY_PRESS){ //if it is not a key press continue

            continue;

        }

        ps2_key_t key = event.key;

        if((unsigned char)key.ch >= PS2_KEY_SHIFT ){  //if function key return special key 

            return key.ch;
        }
        char ch = key.ch;

        keyboard_modifiers_t modifiers = event.modifiers;

        int shift_on = modifiers & KEYBOARD_MOD_SHIFT; //check if shift is pressed

        int cap_lock_on = modifiers & KEYBOARD_MOD_CAPS_LOCK; //check if CAPS pressed

        if(shift_on){

            ch = key.other_ch; //use capital letter if shift on

        }else if(cap_lock_on){ 

            if (ch >= 'a' && ch <= 'z'){ //if caps is on it is lower case

                ch = key.other_ch; //convert

            }
        }

        return ch;

    }
    
}
