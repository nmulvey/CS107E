/* File: test_gl_console.c
 * -----------------------
 * ***** TODO: add your file header comment here *****
 * name: Nicole Mulvey
 * 
 * This contains all the tests for the gl, fb, and console
 */
#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"
#include "malloc.h"



static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in tio to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}


static void test_fb(void) {
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);

    void *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    pause("Now displaying 500 x 500 screen of light gray pixels");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    cptr = fb_get_draw_buffer();
    nbytes =  fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 white pixels");

    cptr = fb_get_draw_buffer();
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 dark gray pixels");

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer();
        timer_delay_ms(250);
    }
}

static void test_gl(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55)); // create purple color


    
    color_t actual_color = gl_read_pixel(WIDTH - 10, HEIGHT - 10);

    printf("Expected Color: 0x%08X\n", GL_GREEN);
    printf("Actual Color:   0x%08X\n", actual_color);

    // Draw green pixel in lower right
    gl_draw_pixel(WIDTH-10, HEIGHT-10, GL_GREEN);
    assert(gl_read_pixel(WIDTH-10, HEIGHT-10) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH/2 - 100, HEIGHT/2 - 50, 200, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");
}

static void test_console(void) {
    gl_init(640, 480, GL_DOUBLEBUFFER);
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");

    // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction
    pause("Console printfs");

    // Clear all lines
    console_printf("\f");

    // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
    pause("Console clear");
}

/* TODO: Add tests to test your graphics library and console.
   For the graphics library, test both single & double
   buffering and confirm all drawing is clipped to bounds
   of framebuffer
   For the console, make sure to test wrap of long lines and scrolling.
   Be sure to test each module separately as well as in combination
   with others.
*/

void test_gl_draw_string(void) {

    const int WIDTH = 800;
    const int HEIGHT = 600;

    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    //clear to black 
    gl_clear(GL_BLACK);


    //set color to white
    color_t text_color = GL_WHITE;

    //get dimensions
    int char_width = gl_get_char_width();

    int char_height = gl_get_char_height();

    //display message
    const char *message = "Congrats, you did it!";

    //find wdith and height 
    int message_length = strlen(message);

    int text_width = message_length * char_width;

    int text_height = char_height;

    //starting post
    int x = (WIDTH - text_width) / 2;

    int y = (HEIGHT - text_height) / 2;

    // Draw the message on the screen
    gl_draw_string(x, y, message, text_color);


    pause("Congralatory message displaying");


}


void test_single_buffer(void) {

    gl_init(640, 480, GL_SINGLEBUFFER);

    gl_clear(GL_BLACK);

    gl_draw_rect(-50, -50, 200, 200, GL_RED);   

    gl_draw_pixel(-10, -10, GL_WHITE); 
    gl_draw_pixel(650, 490, GL_WHITE);  
    gl_draw_pixel(320, 240, GL_WHITE);  

    
    gl_swap_buffer();

    pause("Single Buffering Test");

}

void test_console_wrap_and_scroll(void) {
    
    gl_init(640, 480, GL_DOUBLEBUFFER);
    console_init(10, 40, GL_WHITE, GL_BLACK);

    
    console_clear();

    //very long text..
    console_printf("This is a very long line that should wrap around the console because it exceeds the number of columns in the console display. It will test whether the console correctly wraps text to the next line without truncation.\n");

    //gl_swap_buffer();
    pause("Console Wrap Test: Observe if long lines wrap correctly.");

    //scrolling test 
    for (int i = 1; i <= 15; i++) {
        console_printf("Line %d: Testing console scrolling.\n", i);
    }

    gl_swap_buffer();
    pause("Console Scroll Test.");
}


void test_gl_draw_line(void) {
  
    gl_init(640, 480, GL_DOUBLEBUFFER);

    
    gl_clear(GL_BLACK);
    
    gl_draw_line(50, 100, 590, 100, GL_RED);

    gl_draw_line(590, 120, 50, 120, GL_GREEN);
    
    gl_draw_line(320, 50, 320, 430, GL_BLUE);
    
    gl_draw_line(340, 430, 340, 50, GL_YELLOW);
    
    gl_draw_line(50, 50, 590, 430, GL_CYAN);

    gl_draw_line(590, 50, 50, 430, GL_MAGENTA);
    
    gl_draw_line(50, 240, 590, 340, GL_WHITE);
    
    gl_draw_line(240, 50, 340, 430, GL_RED);
    
    gl_draw_line(400, 430, 500, 50, GL_GREEN);

    gl_draw_line(50, 380, 590, 280, GL_BLUE);

    
    gl_draw_line(-50, -50, 700, 500, GL_YELLOW); 
    gl_draw_line(320, 240, 800, 600, GL_MAGENTA); 



    // Swap buffers to display the drawn lines
    gl_swap_buffer();

    // Pause to observe the output
    pause("Test gl_draw_line: Observe if all lines are drawn correctly with different slopes and directions.");
}

void test_gl_draw_triangle(void) {
    gl_init(640, 480, GL_DOUBLEBUFFER);
    gl_clear(GL_BLACK);

    
    gl_draw_triangle(100, 150, 300, 350, 500, 100, GL_RED);

    gl_swap_buffer();
    // pause("Observe the filled triangle on the screen.");
}



void main(void) {
    timer_init();
    uart_init();
    printf("Executing main() in test_gl_console.c\n");

    
    //test_fb();
    //test_gl();
    //test_gl_draw_string();

    //test_graphics_single_buffer();
    
    //test_single_buffer();

    //test_console_wrap_and_scroll();
    //test_console();

    // int start_time = timer_get_ticks();

    
    //test_gl_draw_line();

    // test_gl_draw_triangle();

    // int end_time = timer_get_ticks();

    // int elapsed_ticks = end_time - start_time;

    // int time = elapsed_ticks / TICKS_PER_USEC;

    // console_init(3, 8, GL_WHITE, GL_BLACK);
    // console_printf("abc\n");
    // console_printf("def\b\b\bghi\n");

    test_framebuffer_dimensions();

    // pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");

    // console_clear();
    // console_printf("Z\n");

    


    // printf("Elapsed time: %d microseconds\n", time);


    printf("Completed main() in test_gl_console.c\n");
}
