/* File: strings.c
 * ---------------
 * ***** TODO: add your file header comment here *****
 * 
 * Name: Nicole Mulvey
 * This file helps to format strings 
 */
#include "strings.h"

void *memcpy(void *dst, const void *src, size_t n) {
    /* Copy contents from src to dst one byte at a time */
    char *dst_temp = dst;
    const char *src_temp = src;
    while (n--) {
        *dst_temp++ = *src_temp++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    /***** TODO: Your code goes here *****/
    char *dst_temp = dst;
    unsigned char byte_val = val & 0xFF;

    for(size_t i = 0; i < n; i++){

        dst_temp[i] = byte_val;

    }

    return dst;
}

size_t strlen(const char *str) {
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2) {
    /***** TODO: Your code goes here *****/
    const unsigned char *pointer_s1 = (const unsigned char *) s1;
    const unsigned char *pointer_s2 = (const unsigned char *) s2;

    while( *pointer_s1 != '\0' && *pointer_s2 != '\0'){

        if (*pointer_s1 != *pointer_s2){
            return (unsigned char) *pointer_s1 - (unsigned char) *pointer_s2;
        }

        pointer_s1++;
        pointer_s2++;

    }

    return (unsigned char) *pointer_s1 - (unsigned char) *pointer_s2;
}

size_t strlcat(char *dst, const char *src, size_t dstsize) {
    /***** TODO: Your code goes here *****/

    int length_dst = strlen(dst);
    int length_src = strlen(src);
    size_t i = 0;
    while (i < dstsize && dst[i] != '\0'){

        i++;

    }   

    if(i == dstsize ){

        return dstsize + length_src;

    }

    size_t space_left = dstsize - length_dst - 1;

    size_t num_copy;
    if (space_left < length_src) {
        num_copy = space_left;
    } else {
        num_copy = length_src;
    }

    for (size_t j = 0; j < num_copy; j++){
        dst[length_dst + j ] = src[j];
    }
        
    dst[length_dst + num_copy] = '\0';
    return length_dst + length_src;
    

}

unsigned long strtonum(const char *str, const char **endptr) {
    /***** TODO: Your code goes here *****/

    int base = 10;
    const char *current = str;
    unsigned long result = 0;
    int parsed = 0;
   

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16;
        current += 2; 
    }

    while (*current != '\0'){
        
        int value = 0;

        if (base == 10){ 
        
            if (*current >= '0' && *current <= '9') {
            
                value = *current - '0';
            } else{
                break;
            }

        }else if (base == 16){

            if (*current >= '0' && *current <= '9'){

                value = *current - '0';

            }else if ( *current >= 'a' && *current <= 'f'){
                value = *current - 'a' + 10;

            }else if (*current >= 'A' && *current <= 'F'){
                value = *current - 'A' + 10;

            }else{
                break;
            }


        }
        
    
        result = result * base + value;

        current++;
        parsed++;

        

    }
   
    
    if(endptr !=  NULL){

        if(parsed > 0){
            *endptr = current;
        }else{

        *endptr = str;

        }
    }

    if (current == 0){
        return 0;
    } else {

        return result;
    }

}
