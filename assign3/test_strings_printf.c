/* File: test_strings_printf.c
 * ---------------------------
 * ***** TODO: add your file header comment here *****
 * 
 * Nicole Mulvey
 * This file contains the tests I implmented 
 */
#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

// Prototypes copied from printf.c to allow unit testing of helper functions
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);


static void test_memset(void) {
    char buf[25];
    size_t bufsize = sizeof(buf);

    memset(buf, '@', bufsize); // fill buffer with repeating value
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == '@'); // confirm value

    memset(buf, 0, bufsize); // fill buffer with 0
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0); // confirm value

    memset(buf, 'A', bufsize); // fill buffer with repeating value
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 'A'); // confirm value

    //see if buffer of 0 length does nothing 
    memset(buf, 'T', 0); 
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 'A');

    //see if half buffsize will work
    memset(buf, 'N', bufsize / 2);
    for (int i = 0; i < bufsize / 2; i++)
        assert(buf[i] == 'N'); 

    memset(buf, 'N', bufsize / 2);
    for (int i = bufsize /2; i < bufsize; i++)
        assert(buf[i] == 'N');

    //see if max size byte
    memset(buf, 0xFF, bufsize);
    for(size_t i = 0; i < bufsize; i++)
        assert(buf[i] == (char)0xFF);
    

    //setting to empty string
    //set to null /0 
    

}

static void test_strcmp(void) {
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);

    assert(strcmp("test", "test") == 0);
    assert(strcmp("test", "testing") < 0);
    assert(strcmp("exam", "test") < 0); 

    assert(strcmp("a", "a") == 0);
    assert(strcmp("a", "ball") < 0);
    assert(strcmp("apples", "b") < 0); 

    assert(strcmp("keyboard", "apron") > 0); 

    //just null termintor 

}

static void test_strlcat(void) {

    //Given Tests
    char buf[20];
    size_t bufsize = sizeof(buf);
    memset(buf, '@', bufsize); // init array contents with fixed repeat value

    buf[0] = '\0'; // null at first index makes empty string
    assert(strlen(buf) == 0);
    strlcat(buf, "CS", bufsize); // append CS
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", bufsize); // append 107e
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);

    //My Tests

    //bufsize with stuff in it 
    buf[0] = 'H';
    buf[1] = 'e';
    buf[2] = 'l';
    buf[3] = 'l';
    buf[4] = 'o';
    buf[5] = '\0';
    assert(strlen(buf) == 5);
    strlcat(buf, " World", bufsize); 
    assert(strlen(buf) == 11);
    assert(strcmp(buf, "Hello World") == 0);

    //dstsize of 0
    buf[0] = '\0'; 
    size_t result = strlcat(buf, "Test", 0);
    assert(result == strlen("Test")); 
    assert(strlen(buf) == 0); 
    

}

static void test_strtonum(void) {
    long val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks";
    const char *rest = NULL;

    val = strtonum(input, &rest);
    assert(val == 107);
    // rest was modified to point to first non-digit character
    assert(rest == &input[3]);

    val = strtonum("0x5A", NULL);
    assert(val == 90 );

    val = strtonum("0X5A", NULL);
    assert(val == 90 );

    // val = strtonum("abc", NULL);
    // assert(val == 0 );
    // val = strtonum("", NULL);
    // assert(val == 0 );


}

static void test_helpers(void) {
    char buf[32];
    size_t bufsize = sizeof(buf);
    memset(buf, '@', bufsize); // init array contents with fixed repeat value

    num_to_string(209, 10, buf);
    assert(strcmp(buf, "209") == 0);
    num_to_string(209, 16, buf);
    assert(strcmp(buf, "d1") == 0);

    assert(strcmp(decimal_string(-99), "-99") == 0);
    assert(strcmp(hex_string(0x107e), "107e") == 0);

    //test if binary is not accepted
    num_to_string(15, 2, buf);
    assert(strcmp(buf, "1111") == 0);

    //negative number tets
    assert(strcmp(decimal_string(-2147483648), "-2147483648") == 0);

    assert(strcmp(decimal_string(2147483647), "2147483647") == 0);

    //test hex_string function with zero
    assert(strcmp(hex_string(0), "0") == 0);

    num_to_string(35, 36, buf);
    assert(strcmp(buf, "z") == 0);
}

static void test_snprintf(void) {
    char buf[100];
    size_t bufsize = sizeof(buf);
    memset(buf, '@', bufsize); // init array contents with fixed repeat value

    // Try no formatting codes
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "winky") == 5);
    assert(snprintf(buf, 2, "winky") == 5);

    // From here it is up to you...!


    snprintf(buf, 5, "Hello, %s!", "World");
    //write "Hell" and null terminate
    assert(strcmp(buf, "Hell") == 0);
    assert(strlen(buf) == 4);

    //test padding with spaces for %d
    snprintf(buf, bufsize, "%5d", 42);
    assert(strcmp(buf, "   42") == 0);
    assert(snprintf(buf, bufsize, "%5d", 42) == 5);

    //test padding 
    snprintf(buf, bufsize, "%05x", 0x1A3);
    assert(strcmp(buf, "001a3") == 0);
    assert(snprintf(buf, bufsize, "%05x", 0x1A3) == 5);

    //test %ld 
    snprintf(buf, bufsize, "%7ld", 123456L);
    assert(strcmp(buf, " 123456") == 0);
    assert(snprintf(buf, bufsize, "%7ld", 123456L) == 7);

    //test %lx 
    snprintf(buf, bufsize, "%08lx", 0xABCDUL);
    assert(strcmp(buf, "0000abcd") == 0);
    assert(snprintf(buf, bufsize, "%08lx", 0xABCDUL) == 8);

    //test %s
    snprintf(buf, bufsize, "%10s", "test");
    assert(strcmp(buf, "      test") == 0);
    assert(snprintf(buf, bufsize, "%10s", "test") == 10);

    //test %c
    snprintf(buf, bufsize, "%5c", 'A');
    assert(strcmp(buf, "    A") == 0);
    assert(snprintf(buf, bufsize, "%5c", 'A') == 5);

    //test zero 
    snprintf(buf, bufsize, "%0d", 123);
    assert(strcmp(buf, "123") == 0);
    assert(snprintf(buf, bufsize, "%0d", 123) == 3);

    
}


void test_failed_tests() {
    char buf[1024];
    size_t bufsize = sizeof(buf);
    int ret;

    ret = snprintf(buf, 20, "%p", (void *)0x12340);
    assert(strcmp(buf, "0x00012340") == 0);
    assert(ret == 10);

    ret = snprintf(buf, 20, "%p", (void *)0x100200300400L);
    assert(strcmp(buf, "0x100200300400") == 0);
    assert(ret == 14);

    ret = snprintf(buf, 20, "%p", (void *)0x7f94);
    assert(strcmp(buf, "0x00007f94") == 0);
    assert(ret == 10);

    ret = snprintf(buf, 20, "%p", ((void *)0));
    assert(strcmp(buf, "0x00000000") == 0);
    assert(ret == 10);

    ret = snprintf(buf, 20, "%6p", (void *)0x7f94);
    assert(strcmp(buf, "0x007f94") == 0);
    assert(ret == 8);

    ret = snprintf(buf, 20, "%16p", (void *)0x7f94);
    assert(strcmp(buf, "0x0000000000007f94") == 0);
    assert(ret == 18);
}


// This function just here as code to disassemble for extension
int sum(int n) {
    int result = 6;
    for (int i = 0; i < n; i++) {
        result += i * 3;
    }
    return result + 729;
}

void test_disassemble(void) {
    const unsigned int add =  0x00f706b3;
    const unsigned int xori = 0x0015c593;
    const unsigned int bne =  0xfe061ce3;
    const unsigned int sd =   0x02113423;

    // formatting code %pI accesses the disassemble extension.
    // If extension not implemented, regular version of printf
    // will simply output pointer address followed by I
    // e.g.  "... disassembles to 0x07ffffd4I"
    printf("Encoded instruction %08x disassembles to %pI\n", add, &add);
    printf("Encoded instruction %08x disassembles to %pI\n", xori, &xori);
    printf("Encoded instruction %08x disassembles to %pI\n", bne, &bne);
    printf("Encoded instruction %08x disassembles to %pI\n", sd, &sd);

    unsigned int *fn = (unsigned int *)sum; // disassemble instructions from sum function
    for (int i = 0; i < 10; i++) {
        printf("%p:  %08x  %pI\n", &fn[i], fn[i], &fn[i]);
    }
}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    // test_memset();
    // test_strcmp();
    //test_strlcat();
    test_strtonum();
    //test_helpers();
    //test_snprintf();
    //test_snprintf_pointers();
    //test_vsnprintf();
    // test_disassemble(); // uncomment if you implement extension

    // TODO: Add more and better tests!

    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}
