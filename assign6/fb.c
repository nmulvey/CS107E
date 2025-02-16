/* File: fb.c
 * ----------
 * ***** TODO: add your file header comment here *****
 * Name: Nicole Mulvey
 * This code is for the framebuffer
 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"
#include "printf.h"

// module-level variables, you may add/change this struct as you see fit
static struct {
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
    void *framebuffer;     // pointer to framebuffer
    void *drawbuffer;      // pointer to second buffer
    int double_buf_active; 
} module;

void fb_init(int width, int height, fb_mode_t mode) {
    /***** TODO: Make changes to support double-buffered mode  *****/
    //free buffer if allocated
    if (module.framebuffer){
        free(module.framebuffer);
    }

    //free second buffer
    if (module.drawbuffer){
        free(module.drawbuffer);
    }

    // hdmi_resolution_id_t id = hdmi_best_match(width, height);
    // hdmi_init(id);


    module.width = width;
    module.height = height;
    module.depth = 4;
    int nbytes = module.width * module.height * module.depth;

    module.framebuffer = malloc(nbytes);
    memset(module.framebuffer, 0x0, nbytes);

    //see if double buffering is enabled 

    if (mode == FB_DOUBLEBUFFER){

        //allocate and inizalie second buffer
        module.drawbuffer = malloc(nbytes);
        memset(module.drawbuffer, 0x0, nbytes);

        //setting flag 
        module.double_buf_active = 1;
    } else {

        //in single buf mode
        module.drawbuffer = NULL;
        module.double_buf_active = 0;
    }

    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    
    de_init(module.width, module.height, hdmi_get_screen_width(), hdmi_get_screen_height());
    de_set_active_framebuffer(module.framebuffer);


}

int fb_get_width(void) {
    /***** TODO: Your code goes here *****/
    return module.width;
}

int fb_get_height(void) {
    /***** TODO: Your code goes here *****/
    return module.height;
}

int fb_get_depth(void) {
    /***** TODO: Your code goes here *****/
    return module.depth;
}

void* fb_get_draw_buffer(void){
    /***** TODO: Your code goes here *****/
    if (module.double_buf_active){

        return module.drawbuffer; //get second buffer

    } else{

        return module.framebuffer; //get frame buffer
    }
    
}

void fb_swap_buffer(void) {
    /***** TODO: Your code goes here *****/
    if(module.double_buf_active){
        //swap the two buffers

        void *temp = module.framebuffer;

        module.framebuffer = module.drawbuffer;

        module.drawbuffer = temp;

        de_set_active_framebuffer(module.framebuffer);

    }
}
