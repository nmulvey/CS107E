/* File: printf.c
 * --------------
 * ***** TODO: add your file header comment here *****
 * 
 * Nicole Mulvey
 * 
 * This file contains the printf functions 
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/* Prototypes for internal helpers.
 * Typically we would qualify these functions as static (private to module)
 * but in order to call them from the test program, must declare externally
 */
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);

// max number of digits in long + space for negative sign and null-terminator
#define MAX_DIGITS 25


/* Convenience functions `hex_string` and `decimal_string` are provided
 * to you.  You should use the functions as-is, do not change the code!
 *
 * A key implementation detail to note is these functions declare
 * a buffer to hold the output string and return the address of buffer
 * to the caller. If that buffer memory were located on stack, it would be
 * incorrect to use pointer after function exit because local variables
 * are deallocated. To ensure the buffer memory is accessible after
 * the function exists, the declaration is qualified `static`. Memory
 * for a static variable is not stored on stack, but instead in the global data
 * section, which exists outside of any function call. Additionally static
 * makes it so there is a single copy of the variable, which is shared by all
 * calls to the function. Each time you call the function, it overwrites/reuses
 * the same variable/memory.
 *
 * Adding static qualifier to a variable declared inside a function is a
 * highly atypical practice and appropriate only in very specific situations.
 * You will likely never need to do this yourself.
 * Come talk to us if you want to know more!
 */

const char *hex_string(unsigned long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    num_to_string(val, 16, buf); // num_to_string does the hard work
    return buf;
}

const char *decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        num_to_string(-val, 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        num_to_string(val, 10, buf);
    }
    return buf;
}

void num_to_string(unsigned long num, int base, char *outstr) {
    /***** TODO: Your code goes here *****/

    char temp_buf[MAX_DIGITS];
    int count = 0;

    if (num == 0) { //handle if the num is 0
        outstr[0] = '0';
        outstr[1] = '\0'; 
        return;
    }
    
    while( num > 0){

        int digit_value = num % base; //this will get us the remainder and works for both bases :)
        char digit_char; //get the char of the current num
    
        //digit value convert
        if (digit_value < 10) {
            digit_char = '0' + digit_value; //for nums 0-9
        } else {
            digit_char = 'a' + (digit_value - 10); //for nums greater than 10 map to hex
        }

        temp_buf[count] = digit_char;
        count++;
        num = num / base; //go to the next digit

    }
    
    //reverse loop
    for(int i = 0; i < count; i++) {
        outstr[i] = temp_buf[count - i - 1];
    }

    outstr[count] = '\0';
  
}

//helper method to add padding 
void append_formatted(char *buf, size_t bufsize, size_t *i, size_t *len,
                      const char *str, int width, char pad_char) {
    size_t str_len = strlen(str);
    size_t padding = 0; //padding needed for string

    //if width is > str_len find amount of padding
    if (width > str_len) {
        padding = width - str_len;
    }

    //add padding before the string
    for (size_t j = 0; j < padding; j++) {
        if (*i  < bufsize - 1) { //to not exceed buffer size
            buf[(*i)++] = pad_char; 
        }
        (*len)++; //incrament length 
    }

    //add the string 
    for (size_t j = 0; j < str_len; j++) {
        if (*i  < bufsize - 1) {
            buf[(*i)++] = str[j]; 
        }
        (*len)++; //incrament length
    }
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args) {
    /***** TODO: Your code goes here *****/
    size_t i = 0;   //index into buf
    size_t len = 0; //total length


    const char *p = format; //p == pointer 

    //loop through each character 
    while (*p != '\0') {
        if (*p == '%') {
            p++; 

            //check for null 
            if (*p == '\0') {
                //to handle empty %
                if (i < bufsize - 1) {
                    buf[i++] = '%';
                }
                len += 1;
                break;
            }

            // Parse field width
            int width = 0;
            if (*p >= '0' && *p <= '9') {
                const char *endptr;
                width = (int)strtonum(p, &endptr); //convert num to int
                p = endptr;
            }

            //set padding as default ' '
            char pad_char = ' ';

            //check for format specifier
            if (*p == '%') { //handle %%
                
                if (i < bufsize - 1) {
                    buf[i++] = '%';
                }

                len += 1;
                p++;

            } else if (*p == 'c') { //handle %c
               
                int c = va_arg(args, int); //get char arg
                char str[2] = {(char)c, '\0'}; //convert to string
                //pad with spaces
                append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                p++;


            } else if (*p == 's') { //handle %s
        
                const char *str = va_arg(args, const char *);
                
                //handle null
                if (str == NULL) {
                    str = "(null)";
                }
                
                //padddddddd
                append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                p++;


            } else if (*p == 'l') { //handle %l...
                p++;


                if (*p == 'd') { //handle %ld
                    
                    long val = va_arg(args, long);
                    const char *str = decimal_string(val);
                    
                    append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                    p++;
                } else if (*p == 'x') { //handle %lx
                    unsigned long val = va_arg(args, unsigned long);
                    const char *str = hex_string(val);

                    //padding with 0's
                    pad_char = '0';
                    append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                    p++;
                } else { //treat anything else as not specifier

                    if (i  < bufsize - 1) {
                        buf[i++] = '%';
                       if (i < bufsize - 1){ 
                            buf[i++] = 'l';
                       }
                    }
                    len += 2;
                    p++;
                }
            } else if (*p == 'd') { //handle %d
                int val = va_arg(args, int);
                const char *str = decimal_string(val); //convert to dec
                
                //we padding again 
                append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                p++;


            } else if (*p == 'x') { //handle %x
                unsigned int val = va_arg(args, unsigned int);
                const char *str = hex_string(val);
                
                //padding with 0's
                pad_char = '0';
                append_formatted(buf, bufsize, &i, &len, str, width, pad_char);
                p++;
            } else if (*p == 'p') { //Handle %p <--pointer

                void *ptr = va_arg(args, void *);
                unsigned long val = (unsigned long)ptr; //cast to long to format
                const char *str = hex_string(val);

                int hex_len = strlen(str);
                int min_hex_digit;
                if (width > 0){

                    min_hex_digit = width;
                } else {
                    min_hex_digit = 8;
                }

                if (min_hex_digit < hex_len){
                    min_hex_digit = hex_len;
                }

                int padding_zeros = min_hex_digit - hex_len;

                char pointer_str[2 + min_hex_digit + 1];
                int idx = 0;

                pointer_str[idx++] = '0';
                pointer_str[idx++] = 'x';

                for( int j = 0; j < padding_zeros; j++){
                    pointer_str[idx++] = '0';
                }

                for (int j = 0; j < hex_len; j++) {
                    pointer_str[idx++] = str[j];
                }

                pointer_str[idx] = '\0';
                
                append_formatted(buf, bufsize, &i, &len, pointer_str, 0, pad_char);
                p++;
            } else { //handle if we do not know it 

                if (i  < bufsize - 1) {
                    buf[i++] = '%';
                    if (i < bufsize - 1) {
                        buf[i++] = *p;
                    }
                }
                len += 2;
                p++;
            }
        } else {
            // Ordinary character
            if (i  < bufsize - 1) {
                buf[i++] = *p;
            }
            len += 1;
            p++;
        }
    }

    //null terminate buf 
    if (bufsize > 0) {
        if (i < bufsize) {
            buf[i] = '\0';
        } else {
            buf[bufsize - 1] = '\0';
        }
    }



    return (int)len; //return length of string 

}


int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    /***** TODO: Your code goes here *****/
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(buf, bufsize, format, args);
    va_end(args);
    return ret;
}

// ok to assume printf output is never longer that MAX_OUTPUT_LEN
#define MAX_OUTPUT_LEN 1024

int printf(const char *format, ...) {
    /***** TODO: Your code goes here *****/
    char buffer[1024];
    size_t bufsize = sizeof(buffer);
    int ret;

    va_list args;
    va_start(args, format);

    
    ret = vsnprintf(buffer, bufsize, format, args);

    va_end(args);


    uart_putstring(buffer);

    return ret;

}


/* From here to end of file is some sample code and suggested approach
 * for those of you doing the disassemble extension. Otherwise, ignore!
 *
 * The struct insn bitfield is declared using exact same layout as bits are organized in
 * the encoded instruction. Accessing struct.field will extract just the bits
 * apportioned to that field. If you look at the assembly the compiler generates
 * to access a bitfield, you will see it simply masks/shifts for you. Neat!
 */
/*
static const char *reg_names[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                                    "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                                    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                                    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

struct insn  {
    uint32_t opcode: 7;
    uint32_t reg_d:  5;
    uint32_t funct3: 3;
    uint32_t reg_s1: 5;
    uint32_t reg_s2: 5;
    uint32_t funct7: 7;
};

void sample_use(unsigned int *addr) {
    struct insn in = *(struct insn *)addr;
    printf("opcode is 0x%x, reg_dst is %s\n", in.opcode, reg_names[in.reg_d]);
}
*/
