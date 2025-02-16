/* File: console.c
 * ---------------
 * ***** TODO: add your file header comment here *****
 * Name: Nicole Mulvey
 * This file holds the code for the console
 */
#include "console.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"
#include "font.h"

static void process_char(char ch);
#define LINE_SPACING 5 
   

// module-level variables, you may add/change this struct as you see fit!
static struct {
    color_t bg_color, fg_color;
    int line_height;
    int num_rows; //number of rows
    int num_cols; //number of columns
    int cursor_row; //current cursor row
    int cursor_col; //current cursor col
    char **contents;  //2D array 
} module;

void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    // Please add this amount of vertical space between console rows
    int glyph_width = font_get_glyph_width();
    int glyph_height = font_get_glyph_height();
    int total_width = ncols * glyph_width;
    int total_height = nrows * (glyph_height + LINE_SPACING);

    gl_init(total_width, total_height, GL_DOUBLEBUFFER);
    

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.fg_color = foreground;
    module.bg_color = background;
    /***** TODO: Your code goes here *****/

    //inital cursor pos
    module.cursor_row = 0;
    module.cursor_col = 0;

    //setting rows and cols for console
    module.num_rows = nrows;
    module.num_cols = ncols;

    //allocating mem for array 
    module.contents = malloc(nrows * sizeof(char *));

    for (int i = 0; i < nrows; i++){

        module.contents[i] = malloc(ncols * sizeof(char));

        memset(module.contents[i], ' ', ncols);
    }

    console_clear();
}

void console_clear(void) {
    /***** TODO: Your code goes here *****/

    //erase all the characters
    for( int i = 0; i < module.num_rows; i++){
        
        memset(module.contents[i], ' ', module.num_cols);
    }

    //reset the cursir 
    module.cursor_row = 0;
    module.cursor_col = 0;

    //clear the color 
    gl_clear(module.bg_color);

    //swap the buffer
    gl_swap_buffer();
}

int console_printf(const char *format, ...) {
    /***** TODO: Your code goes here *****/

    //buffer to hold the string
    const size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    va_list args;
    va_start(args, format);

    //format string
    int len = vsnprintf(buffer, BUFFER_SIZE, format, args);

    va_end(args);


    //process each charcter in the string
    for (int i = 0; buffer[i] != '\0'; i++){

        process_char(buffer[i]);
    }

    //clear display
    gl_clear(module.bg_color);



    for (int row = 0; row < module.num_rows; row++){
        //y pos
        int y = row * module.line_height;

        for (int col = 0; col < module.num_cols; col++){

            char ch = module.contents[row][col];
            int x = col * gl_get_char_width(); //x pos

            if (ch != ' '){
                //draw character if not a space
                gl_draw_char(x, y, ch, module.fg_color);
            }
        }
    }

    gl_swap_buffer();

    return len;

	
}

//scroll up helper method 
static void scroll_up(void){

    //move the contents of each row to the line above 
    for (int row = 0; row < module.num_rows - 1; row++){

        memcpy(module.contents[row], module.contents[row + 1], module.num_cols);

    }

    //clear content of last row 
    memset(module.contents[module.num_rows - 1], ' ', module.num_cols);
}

static void process_char(char ch) {
    /***** TODO: Your code goes here *****/
    // Implementing this helper function is recommended
    // if ordinary char: insert ch into contents at current cursor position and
    // advance cursor
    // else if special char, do special handling
    
    if (ch == '\b') {
        //backspace 
        if (module.cursor_col > 0) {

            module.cursor_col--;

        } else if (module.cursor_row > 0) {

            module.cursor_row--;

            module.cursor_col = module.num_cols - 1;
        }
        //overwrite char at new position 

        module.contents[module.cursor_row][module.cursor_col] = ' ';

    } else if (ch == '\n') {
        //handle newline cursor goes to the beginning of next line
        module.cursor_col = 0;

        module.cursor_row++;

        if (module.cursor_row >= module.num_rows) {

            scroll_up();
            module.cursor_row = module.num_rows - 1;

        }
    } else if (ch == '\f') {
        
        console_clear();

    } else {
        //this is for ordinary characters
        if (module.cursor_col >= module.num_cols) {
            //if it exceeds number of cols 
            module.cursor_col = 0;

            module.cursor_row++;

            if (module.cursor_row >= module.num_rows) {

                scroll_up();

                module.cursor_row = module.num_rows - 1;
            }
        }
        module.contents[module.cursor_row][module.cursor_col] = ch;

        module.cursor_col++;
    }
}
