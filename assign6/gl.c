/* File: gl.c
 * ----------
 * ***** TODO: add your file header comment here *****
 * name; Nicole Mulvey
 * 
 * 
 */
#include "gl.h"
#include "font.h"

void gl_init(int width, int height, gl_mode_t mode) {
    
    fb_init(width, height, mode);
}

int gl_get_width(void) {
    /***** TODO: Your code goes here *****/
    return fb_get_width();
}

int gl_get_height(void) {
    /***** TODO: Your code goes here *****/
    return fb_get_height();
}

//gets the color in a 32 bit format with the last bytes set to 0xFF
color_t gl_color(uint8_t r, uint8_t g, uint8_t b) {
    /***** TODO: Your code goes here *****/
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

void gl_swap_buffer(void) {
    /***** TODO: Your code goes here *****/
    fb_swap_buffer();
}

void gl_clear(color_t c) {
    /***** TODO: Your code goes here *****/

    //setting frambuffer width and height
    int width = gl_get_width();
    int height = gl_get_height();

    //get the pixel data 
    uint32_t *framebuffer = (uint32_t *) fb_get_draw_buffer();

    //iterate over row
    for (int y = 0; y < height; y++){

        //iterate over columns
        for (int x = 0; x< width; x++){

            framebuffer[y * width + x] = c; //set pixel to color
        }
    }
}

void gl_draw_pixel(int x, int y, color_t c) {
    /***** TODO: Your code goes here *****/

    if (x < 0 || x >= gl_get_width() || y < 0 || y >= gl_get_height()){
        return;
    }

    int width = gl_get_width();
    uint32_t *framebuffer = (uint32_t *) fb_get_draw_buffer();

    framebuffer [y * width + x] = c;
}

color_t gl_read_pixel(int x, int y) {
    /***** TODO: Your code goes here *****/

    //checking if coordinates are in bounds
    if (x < 0 || x >= gl_get_width() || y < 0 || y >= gl_get_height()){
        return 0;
    }
    
    int width = gl_get_width(); 

    //set buffer pixel data
    uint32_t *framebuffer = (uint32_t *) fb_get_draw_buffer();

    //get color of pixel
    color_t color = framebuffer[y * width + x];

    return color; //return the colro
}

void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    /***** TODO: Your code goes here *****/

    //get the width and height
    int width = gl_get_width();
    int height = gl_get_height();

    int x_start = 0; //x start coordinate
    int y_start = 0; //y start coordinate
    int x_end = 0; //x end coordinate
    int y_end = 0; //y end coordinate

    uint32_t *framebuffer = (uint32_t *) fb_get_draw_buffer();

    //aadjust x_start 
    if (x < 0){
        x_start = 0;
    }else{
        x_start = x;
    }

    //adujst y start 
    if (y < 0){
        y_start = 0;
    }else{
        y_start = y;
    }

    //adjust x_end based off width 
    if ((x + w) > width){
        x_end = width;
    }else{
        x_end = (x + w);
    }

    //adjust y_end based off height
    if ((y + w) > height){
        y_end = height;
    }else{
        y_end = (y + h);
    }

    //see if the coordinates are invalid 
    if (x_start >= x_end || y_start >= y_end){
        return;
    }


    //iterate over bounds 
    for (int row = y_start; row < y_end; row++){

        for(int col = x_start; col < x_end; col++){
            //set pixel color 
            framebuffer[row * width + col] = c;
        }
    }
}

void gl_draw_char(int x, int y, char ch, color_t c) {
    /***** TODO: Your code goes here *****/

    //set width and height 
    int char_width = font_get_glyph_width();
    int char_height = font_get_glyph_height();
    int width = gl_get_width();
    int height = gl_get_height();

    //total size of buffer
    int char_size = char_height * char_width;

    //buffer to hold bitmap
    uint8_t buf[char_size];

    //get the chaar and see if it is in list
    if (!font_get_glyph(ch, buf, char_size)){

        return;
    }

    //iterate over rows 
    for (int i = 0; i < char_height; i++){
        //current row 
        int src_y = y + i;

        if (src_y < 0 || src_y >= height){
            continue;
        }

        //iterate over each column 
        for(int j = 0; j < char_width; j++){

            //current column
            int src_x = j + x;

            if (src_x < 0 || src_x >= width){
                continue;
            }

            //calc buf index
            int index = i * char_width + j;

            //if set then draw 
            if( buf[index] == 0xFF){
                //draw pixel 
                gl_draw_pixel(src_x, src_y, c);
            }
        }
    }



}

void gl_draw_string(int x, int y, const char* str, color_t c) {
    /***** TODO: Your code goes here *****/


    //get width of character
    int char_width = gl_get_char_width();
    int width = gl_get_width();

    //iterate over each string until \0 
    while (*str){
        if (x + char_width <= 0) {
            // Character is entirely left of the framebuffer
            x += char_width;
            str++;
            continue;
        }

        if (x >= width) {
            // No more characters can be drawn on the framebuffer
            break;
        }
        gl_draw_char(x, y, *str, c);
        x += char_width;
        str++;
    }
}

int gl_get_char_height(void) {
    /***** TODO: Your code goes here *****/
    
    return font_get_glyph_height();
}

int gl_get_char_width(void) {
    /***** TODO: Your code goes here *****/
    return font_get_glyph_width();
}

void gl_draw_line(int x1, int y1, int x2, int y2, color_t c){

    //differences in x and y
    int dx = x2 - x1;

    int dy = y2 -y1;

    //absoulte values
    int abs_dx = 0;
    int abs_dy = 0;
    int x = 0;
    int y = 0;
    int xe = 0;
    int ye = 0;
    int step = 0;
    

    //abs val of x and y
    if (dx >= 0){

        abs_dx = dx;
        
    } else{
        abs_dx = -dx;
    }

    if (dy >= 0){

        abs_dy = dy;
        
    } else{
        abs_dy = -dy;
    }

    //check if more horizontal 
    if (abs_dx >= abs_dy){

        //line is vertical
        if (dx == 0){

            x = x1;
            if (y1 < y2){
                y = y1;
            }else{
                y = y2;
            }

            if (y1 > y2){
                ye = y1;
            }else{
                ye = y2;
            }
            //draw vertical line
            while (y <= ye){
                gl_draw_pixel(x, y, c);
                
                y++;
            }            
        }else {
            //find slope and find direction
            float slope = (float) dy / (float) dx;

            if (dx > 0){
                step = 1;
            }else{
                step = -1;
            }

            x = x1;
            y = y1;

            //draw line on x-axis
            while (x != x2){
                gl_draw_pixel(x, y, c);

                x+= step;

                y = y1 + (int) ((x - x1) * dy) / dx;
            }

            gl_draw_pixel(x2, y2, c);
        }
    } else {

        //check if line is horizontal 
        if ( dy == 0){

            if (x1 < x2){
                x = x1;
            }else{
                x = x2;
            }

            y = y1;

            if (x1 > x2){
                xe = x1;
            }else{
                xe = x2;
            }

            while (x <= xe){
                gl_draw_pixel(x, y, c);
                x++;
            }
        } else {
            //find slope for horizontal line
            float slope = (float) dx / (float) dy;

            if ( dy > 0 ){
                step = 1;
            }else{
                step = -1;
            }

            x = x1;
            y = y1;

            while ( y != y2){
                gl_draw_pixel(x, y, c);

                y += step;

                x = x1 + (int) ((y - y1) * dx) / dy;
            }
            gl_draw_pixel(x, y, c);
        }

        
    }

}

void gl_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c) {
    //draw outline of triangle using line
    gl_draw_line(x1, y1, x2, y2, c);
    gl_draw_line(x2, y2, x3, y3, c);
    gl_draw_line(x3, y3, x1, y1, c);

    //min and max of points
    int min_y, max_y;

//find min y
    if (y1 < y2) {
        if (y1 < y3) {
            min_y = y1;
        } else {
            min_y = y3;
        }
    } else {
        if (y2 < y3) {
            min_y = y2;
        } else {
            min_y = y3;
        }
    }

    //find the max y
    if (y1 > y2) {
        if (y1 > y3) {
            max_y = y1;
        } else {
            max_y = y3;
        }
    } else {
        if (y2 > y3) {
            max_y = y2;
        } else {
            max_y = y3;
        }
    }

    //for every line between min y and max y check for intersections
    for (int y = min_y; y <= max_y; y++) {

        int x_values[3];
        int count = 0;

        //(x1, y1) to (x2, y2)
        if ((y >= y1 && y < y2) || (y >= y2 && y < y1)) {

            if (y2 != y1) {

                int x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                x_values[count++] = x;
            }
        }

        //(x2, y2) to (x3, y3)
        if ((y >= y2 && y < y3) || (y >= y3 && y < y2)) {

            if (y3 != y2) {

                int x = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
                x_values[count++] = x;
            }
        }

        //(x3, y3) to (x1, y1)
        if ((y >= y3 && y < y1) || (y >= y1 && y < y3)) {

            if (y1 != y3) {

                int x = x3 + (x1 - x3) * (y - y3) / (y1 - y3);
                x_values[count++] = x;
            }
        }

        //if two intersect draw a line
        if (count >= 2) {
            
            //find min and max x 
            int x_min = x_values[0];

            int x_max = x_values[0];

            for (int i = 1; i < count; i++) {

                if (x_values[i] < x_min)
                    x_min = x_values[i];

                if (x_values[i] > x_max)
                    x_max = x_values[i];
            }
            
            gl_draw_line(x_min, y, x_max, y, c);
        }
    }
}



