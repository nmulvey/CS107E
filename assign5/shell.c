/* File: shell.c
 * -------------
 * ***** TODO: add your file header comment here *****
 * Name: Nicole Mulvey
 * This is the shell driver 
 */
#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h"
#include "mango.h"
#include "keyboard.h"
#include "hstimer.h"
#include "interrupts.h"
#include "symtab.h"
#include "timer.h"
#include "memmap.h"


extern unsigned char __text_start;
extern unsigned char __text_end;

static int profiling = 0;  // 0 = off, 1 = on
static uint32_t *profile_counts = NULL;
static uintptr_t text_start = 0;
static uintptr_t text_end = 0;
static uintptr_t num_instructions = 0;

static uint32_t samples_collected = 0;
static uint32_t profiling_start_time = 0;


#define LINE_LEN 80

int cmd_history(int argc, const char *argv[]);
int cmd_profile(int argc, const char *argv[]);



// Module-level global variables for shell
static struct {
    input_fn_t shell_read;
    formatted_fn_t shell_printf;
} module;


// NOTE TO STUDENTS:
// Your shell commands output various information and respond to user
// error with helpful messages. The specific wording and format of
// these messages would not generally be of great importance, but
// in order to streamline grading, we ask that you aim to match the
// output of the reference version.
//
// The behavior of the shell commands is documented in "shell_commands.h"
// https://cs107e.github.io/header#shell_commands
// The header file gives example output and error messages for all
// commands of the reference shell. Please match this wording and format.
//
// Your graders thank you in advance for taking this care!

#define HISTORY_SIZE 10
#define LINE_LEN 80


//struct to deal with the history of the shell
static struct {
    char commands[HISTORY_SIZE][LINE_LEN]; 
    int next_num; 
    int count; 
    int current; 
} history = { .next_num = 1, .count = 0, .current = -1 };




static const command_t commands[] = {
    {"help",  "help [cmd]",  "print command usage and description", cmd_help},
    {"echo",  "echo [args]", "print arguments", cmd_echo},
    {"clear", "clear",       "clear screen (if your terminal supports it)", cmd_clear}, 
    {"reboot", "reboot",     "reboot the Mango Pi", cmd_reboot},
    {"peek",   "peek [addr]",       "print contents of memory at address", cmd_peek},
    {"poke",   "poke [addr] [val]",  "store value into memory at address",  cmd_poke},
    {"history", "history", "display command history", cmd_history},
    {"profile", "profile [on | off]", "control program profiler", cmd_profile},

    
};

int cmd_echo(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i)
        module.shell_printf("%s ", argv[i]);
    module.shell_printf("\n");
    return 0;
}

int cmd_history(int argc, const char *argv[]) {
    if (history.count == 0) {
        module.shell_printf("No commands in history.\n");
        return 0;
    }

    int oldest_cmd_num = history.next_num - history.count;
    

    for (int i = 0; i < history.count; i++) {
        int cmd_num = oldest_cmd_num + i;
        int index = (cmd_num - 1) % HISTORY_SIZE;
        module.shell_printf("%d\t%s\n", cmd_num, history.commands[index]);
    }
    return 0;
}

int cmd_reboot(int argc, const char *argv[]){

    module.shell_printf("Rebooting...\n");

    mango_reboot();

    return 0;
}

int cmd_peek(int argc, const char *argv[]){

    //check if enough arguments 
    if(argc != 2){

        module.shell_printf("error: peek expects 1 argument [addr]\n");

        return -1;

    }

    const char *addr_str = argv[1]; //get the address string 

    const char *end_ptr = NULL;

    unsigned long addr = strtonum(addr_str, &end_ptr); //convert to num

    //check if convert worked 
    if(*end_ptr != '\0'){

        module.shell_printf("error: peek cannot convert '%s'\n", addr_str);

        return -1;
    }

    //check if 4-byte alligned
    if (addr % 4 != 0){

        module.shell_printf("error: peek address must be 4-byte aligned\n");

        return -1;
    }

    unsigned int *ptr = (unsigned int *) addr; //cast address to pointer

    unsigned int value = *ptr; //get value 

    module.shell_printf("0x%08lx:  %08x\n", addr, value); 

    return 0;
}

int cmd_poke(int argc, const char *argv[]){

    //check if 2 arguments are provided
    if (argc != 3){

        module.shell_printf("error: poke expects 2 arguments [addr] and [val]\n");

        return -1;

    }

    const char *addr_str = argv[1]; //get first arg

    const char *val_str = argv[2]; //get second arg

    const char *end_ptr_addr = NULL;

    const char *end_ptr_val = NULL;

    unsigned long addr = strtonum(addr_str, &end_ptr_addr); //convert

    //check if string was converted
    if (*end_ptr_addr != '\0') {
        
        module.shell_printf("error: poke cannot convert '%s'\n", addr_str);
        
        return -1;
    }

    //check if 4-byte aligned
    if (addr % 4 != 0) {
        
        module.shell_printf("error: poke address must be 4-byte aligned\n");

        return -1;
    }

    unsigned long val = strtonum(val_str, &end_ptr_val); //convert to num

    //check if converted succesfully 
    if (*end_ptr_val != '\0') {
        
        module.shell_printf("error: poke cannot convert '%s'\n", val_str);
        
        return -1;
    }

    unsigned int *ptr = (unsigned int *)addr;
    
    *ptr = (unsigned int)val;

    return 0;

}


int cmd_help(int argc, const char *argv[]) {
    /***** TODO: Your code goes here *****/
    //(void)commands; // to quiet compiler warning before you have implemented this function

    size_t num_comm = sizeof(commands) / sizeof(commands[0]);

    if (argc == 1){

        for(size_t i = 0; i < num_comm; i++){

            module.shell_printf("%s\t%s\n", commands[i].usage, commands[i].description);
        }
        return 0;

    }else{

        const char *cmd_name = argv[1];

        for(size_t i = 0; i < num_comm; i++){
            if(strcmp(cmd_name, commands[i].name) == 0){
                module.shell_printf("%s\t%s\n", commands[i].usage, commands[i].description);
                return 0;
            }
        }

        module.shell_printf("error: no such command '%s' \n", cmd_name);
        return -1;
    }

    
}

int cmd_clear(int argc, const char* argv[]) {
    module.shell_printf("\f");   // formfeed character
    return 0;
}

void shell_init(input_fn_t read_fn, formatted_fn_t print_fn) {
    module.shell_read = read_fn;
    module.shell_printf = print_fn;
}

void shell_bell(void) {
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize) {
    /***** TODO: Your code goes here *****/
    memset(buf, 0, bufsize); //clear the buffer...

    buf[0] = '\0';

    size_t pos = 0; //current position in buf

    size_t len = 0; //len of input

    int ch;

    while(1){

        ch = keyboard_read_next(); //read character from input

        if (ch == '\n'){

            module.shell_printf("\n"); //echo newline

            break;
        }

        //handle the left arrow key 
        if (ch == PS2_KEY_ARROW_LEFT){

            if (pos > 0){

                pos--;

                module.shell_printf("\033[D");

            }else{
                shell_bell();
            }

            continue;

        //handle mocing right 
        }else if (ch == PS2_KEY_ARROW_RIGHT){

            if(pos < len){
                pos++;

                module.shell_printf("\033[C");

            }else{
                shell_bell();
            }
        //handles the commands of the history 
        }else if (ch == PS2_KEY_ARROW_UP){

            if (history.count == 0){
                shell_bell();

            } else{

                if (history.current == -1) {
                    history.current = history.count - 1; //start from last command
                } else if (history.current > 0) {

                    history.current--; //go to older one
                } else {
                    shell_bell();
                    continue;
                }

                //clear the input
                while (pos > 0) {
                    module.shell_printf("\033[D");
                    pos--;
                }
                for (size_t i = 0; i < len; i++) {
                    module.shell_printf(" ");
                }
                for (size_t i = 0; i < len; i++) {
                    module.shell_printf("\033[D");
                }

                //get commands from history
                int cmd_num = history.next_num - history.count + history.current;
                int cmd_index = (cmd_num - 1) % HISTORY_SIZE;

                size_t cmd_len = strlen(history.commands[cmd_index]);
                if (cmd_len >= bufsize) {
                    cmd_len = bufsize - 1; //buffer no overflow
                }
                memcpy(buf, history.commands[cmd_index], cmd_len);
                buf[cmd_len] = '\0'; 
                
                len = cmd_len;
                pos = cmd_len;

                
                module.shell_printf("%s", buf);
            }

            continue;

        }else if (ch == PS2_KEY_ARROW_DOWN) {
            //handle down arrow key
            if (history.count == 0 || history.current == -1) {
                shell_bell();
            } else {
                //move to newer command
                if (history.current < history.count - 1) {
                    history.current++;

                } else {
                    //reset to og 
                    history.current = -1;
                    

                    //clear current line
                    while (pos > 0) {
                        module.shell_printf("\033[D");
                        pos--;
                    }


                    for (size_t i = 0; i < len; i++) {

                        module.shell_printf(" ");
                    }
                    for (size_t i = 0; i < len; i++) {

                        module.shell_printf("\033[D");
                    }
                    //reset buffer
                    buf[0] = '\0';
                    len = 0;
                    pos = 0;
                    continue;
                }

                
                while (pos > 0) {

                    module.shell_printf("\033[D");

                    pos--;
                }
                for (size_t i = 0; i < len; i++) {

                    module.shell_printf(" ");
                }
                for (size_t i = 0; i < len; i++) {

                    module.shell_printf("\033[D");
                }

                //copy from history
                int cmd_num = history.next_num - history.count + history.current;

                int cmd_index = (cmd_num - 1) % HISTORY_SIZE;

                size_t cmd_len = strlen(history.commands[cmd_index]);

                if (cmd_len >= bufsize) {

                    cmd_len = bufsize - 1; 
                }
                memcpy(buf, history.commands[cmd_index], cmd_len);
                buf[cmd_len] = '\0'; 

                len = cmd_len;
                pos = cmd_len;

                
                module.shell_printf("%s", buf);
            }
            continue;


        }else if (ch == '\b'){ //handles backspace 

            if (pos > 0){

                module.shell_printf("\b \b"); 

                for (size_t i = pos - 1; i < len - 1; i++) {

                    buf[i] = buf[i + 1];
                }
                
                len--;
                pos--; //move back one position

                //module.shell_printf("\033[D");

                for (size_t i = pos; i < len; i++) {

                    module.shell_printf("%c", buf[i]);
                }
                if (len > 0){
                    module.shell_printf(" ");
                }
                
             
                for (size_t i = len + (len > 0 ? 1 : 0); i > pos; i--) {
                module.shell_printf("\b");
        }
                
            } else{

                shell_bell();

            }

            continue;


        }else if (ch == '\t') {
         //handle tab and auto-complete 

            size_t word_start = pos;

            while (word_start > 0 && buf[word_start - 1] != ' ') {

                word_start--;
            }
            size_t word_len = pos - word_start;

            //extract for matching
            char current_word[LINE_LEN];

            memcpy(current_word, &buf[word_start], word_len);

            current_word[word_len] = '\0';

            size_t num_commands = sizeof(commands) / sizeof(commands[0]);

            int match_count = 0;

            const char* match = NULL;

            //do matching on prefix
            for (size_t i = 0; i < num_commands; i++) {

                const char *cmd_name = commands[i].name;

                size_t cmd_name_len = strlen(cmd_name);

                if (cmd_name_len >= word_len) {

                    int is_match = 1;

                    for (size_t j = 0; j < word_len; ++j) {

                        if (cmd_name[j] != current_word[j]) {

                            is_match = 0;

                            break;
                        }
                    }
                    
                    //first matching command
                    if (is_match) {

                        match_count++;

                        if (match_count == 1) {

                            match = cmd_name;

                        } else {
                            //multiple found 

                            match = NULL;
                        }
                    }
                }
            }

            if (match_count == 1 && match != NULL) {
                //auto complete only if 1 match found
                size_t remaining_len = strlen(match) - word_len;

                if (len + remaining_len < bufsize - 1) {

                    //shift buffer to auto compl

                    for (size_t i = len; i >= pos; i--) {

                        buf[i + remaining_len] = buf[i];
                    }

                    //insert
                    memcpy(&buf[pos], &match[word_len], remaining_len);

                    len += remaining_len;
                    pos += remaining_len;

                    buf[len] = '\0';  
                    

                    module.shell_printf("%s", &match[word_len]);
                } else {

                    shell_bell();
                }
            } else if (match_count > 1) {
                //show all copletions
                module.shell_printf("\n");

                for (size_t i = 0; i < num_commands; i++) {

                    const char *cmd_name = commands[i].name;
                    size_t cmd_name_len = strlen(cmd_name);

                    if (cmd_name_len >= word_len) {

                        int is_match = 1;
                        for (size_t j = 0; j < word_len; ++j) {

                            if (cmd_name[j] != current_word[j]) {

                                is_match = 0;

                                break;
                            }
                        }
                        if (is_match) {

                            module.shell_printf("%s  ", cmd_name);
                        }
                    }
                }
                module.shell_printf("\n");

               
                module.shell_printf("\r\033[K");
                module.shell_printf("Pi> [%x]$ %s", history.next_num, buf);

                //move back to correct position
                size_t chars_to_move_back = len - pos;
                for (size_t i = 0; i < chars_to_move_back; ++i) {

                    module.shell_printf("\033[D"); 
                }
            } else {
                shell_bell();
            }
            continue;
        }else if(ch >= 0x20 && ch <= 0x7F){ //check if charcter is printable

            if(pos < bufsize -1 ){

                for (size_t i = len; i > pos; i--) {
                    buf[i] = buf[i - 1];
                }

                buf[pos] = ch; //add charcter to buffer

                 len++;
                pos++;
                
                for (size_t i = pos - 1; i < len; i++) {
                    module.shell_printf("%c", buf[i]);
                }

                //move cursor back 
                for (size_t i = len; i > pos; i--) {
                    module.shell_printf("\033[D");
                }
                

            }else{

                shell_bell();
            }
            continue;
        } else {

            continue;
        }
    }

    buf[len] = '\0';
}

int shell_evaluate(const char *line) {
    /***** TODO: Your code goes here *****/

    const int MAX_ARGS = 32;

    char *argv[MAX_ARGS]; //arg vector 

    int argc = 0;   //arg count

    size_t len = strlen(line) + 1; //length of input

    char *line_copy = malloc(len); //allocate memory of input line

    //handle memory allocation fail
    if(line_copy == NULL){

        module.shell_printf("error: malloc failed\n");

        return -1;
    }

    memcpy(line_copy, line, len); //copy input 

    char *p = line_copy; //pointer to traverse line

    while (*p != '\0'){

        //skip whitespace at beginning
        while (*p == ' ' || *p == '\t' || *p == '\n'){

            p++;

        }

        if(*p == '\0'){

            break;

        }

        if (argc >= MAX_ARGS) { //check if too many 

            module.shell_printf("error: too many arguments\n");

            free(line_copy); //free mem

            return -1;
        }

        argv[argc++] = p;
        
        //move to the end of current arg
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') {

            p++;

        }
       

        if (*p != '\0') {

            *p = '\0'; //terminate current arg

            p++;
        }
    }

    if (argc == 0) {  //no command entered 

        free(line_copy);

        return 0;
    }

    size_t num_commands = sizeof(commands) / sizeof(commands[0]); //num of commands

    size_t i;

    for (i = 0; i < num_commands; ++i) { //iterate through commands to find match

        if (strcmp(argv[0], commands[i].name) == 0) {//command found
            
            int result = commands[i].fn(argc, (const char **)argv);

            free(line_copy);

            return result;
        }
    }

    //if cannot find print error
    module.shell_printf("error: no such command '%s'\n", argv[0]);

    free(line_copy);

    return -1;


}

void shell_run(void) {
    module.shell_printf("Welcome to the CS107E shell.\nRemember to type on your PS/2 keyboard!\n");
    

    while (1) {
        char line[LINE_LEN];

        history.current = -1; //reset 
        int current_command_number = history.next_num;

        module.shell_printf("Pi> [%x]$ ", current_command_number);
      
        shell_readline(line, sizeof(line));

        if (strlen(line) > 0) {
            int index = (history.next_num - 1) % HISTORY_SIZE; //determine indx
            
            size_t len = strlen(line);
            if (len >= LINE_LEN) {
                len = LINE_LEN - 1; 
            }
            memcpy(history.commands[index], line, len); //store
            history.commands[index][len] = '\0';

            if (history.count < HISTORY_SIZE) { //see if it is full
                history.count++; 
            }
            history.next_num++;
        }
        shell_evaluate(line);
    }
}

static inline uintptr_t read_mepc(void) {
    uintptr_t mepc_value;
   
    asm volatile("csrr %0, mepc" : "=r"(mepc_value));

    return mepc_value;
}


void profiler_interrupt_handler(void) {
    if (!profiling) return;


    uintptr_t pc = read_mepc();  //get interrupt 

    if (pc >= text_start && pc < text_end) {
        uintptr_t index = (pc - text_start) / 4;  //find pc index

        profile_counts[index]++;

        samples_collected++;
    }

    //clear it 
    hstimer_interrupt_clear(HSTIMER0);
}

void profiler_interrupt_handler_wrapper(void *aux_data) {

    profiler_interrupt_handler();
}


void profiler_init(void) {
    text_start = (uintptr_t)&__text_start;

    text_end = (uintptr_t)&__text_end;

    num_instructions = (text_end - text_start) / 4;

    profile_counts = malloc(num_instructions * sizeof(uint32_t));

    if (profile_counts == NULL) {
        
        module.shell_printf("error: failed to allocate memory for profiler.\n");
        return;
    }

    memset(profile_counts, 0, num_instructions * sizeof(uint32_t));

    //generate interupt
    hstimer_init(HSTIMER0, 500);  // Use HSTIMER0, interval of 500 microseconds

    //get the inerrupt register handler 
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, profiler_interrupt_handler_wrapper, NULL);

    
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0);

    //get global interrupts
    interrupts_global_enable();
}

int cmd_profile(int argc, const char *argv[]) {
    //display current profiling status and results if profilinf on
    if (argc == 1) {

        if (profiling) {

            uint32_t elapsed_ticks = timer_get_ticks() - profiling_start_time;

            uint32_t elapsed_time = elapsed_ticks / (TICKS_PER_USEC * 1000);  

            module.shell_printf("profile: %u msecs elapsed, collected %u samples\n", elapsed_time, samples_collected);

        } else {
            //print if off
            module.shell_printf("profile is currently off.\n");
        }
        return 0;
    }

    //print error if too many arguments
    if (argc != 2) {

        module.shell_printf("error: usage: profile [on | off]\n");

        return -1;
    }

    //profiling is on
    if (strcmp(argv[1], "on") == 0) {

        if (profiling) { //if activated again print error

            module.shell_printf("error: profiling is already on.\n");

            return -1;
        }

        //start profiling 
        profiler_init();
        profiling_start_time = timer_get_ticks();
        samples_collected = 0;
        profiling = 1;

       //get timer
        hstimer_enable(HSTIMER0);

        module.shell_printf("profiling started.\n");
        return 0;

    //if profiling gets turned off 
    } else if (strcmp(argv[1], "off") == 0) {

        if (!profiling) {//error to check if off

            module.shell_printf("error: profiling is already off.\n");
            return -1;
        }

        profiling = 0;

        
        hstimer_disable(HSTIMER0);

        uint32_t elapsed_ticks = timer_get_ticks() - profiling_start_time;

        uint32_t elapsed_time = elapsed_ticks / (TICKS_PER_USEC * 1000);  

        //display results
        module.shell_printf("profile: %d msecs elapsed, collected %d samples\n", elapsed_time, samples_collected);
        module.shell_printf("\n  Counts  |    pc        <fn>\n");
        module.shell_printf("---------------------------------------\n");

        //top 20 below!

        //get the data that was profiled
        for (int i = 0; i < 20; i++) {
            uint32_t max_count = 0;
            uint32_t max_index = 0;

            for (uint32_t j = 0; j < num_instructions; j++) {

                if (profile_counts[j] > max_count) {

                    max_count = profile_counts[j];
                    max_index = j;
                }
            }

            if (max_count == 0) break;  

            //find program counter address
            uint32_t pc = text_start + max_index * 4;

            char label[64]; //buffer for current address
            symtab_label_for_addr(label, sizeof(label), pc);

            symbol_t symbol;

            if (symtab_symbol_for_addr(pc, &symbol)) {
                size_t offset = pc - symbol.addr;

                module.shell_printf("   %d  |  0x%x  <%s>\n", max_count, (unsigned int)pc, label);

            } else {

                module.shell_printf("   %d  |  0x%x  <unknown>\n", max_count, (unsigned int)pc);
            }

           

            profile_counts[max_index] = 0; //reset to find next xount 
        }


        free(profile_counts);

        profile_counts = NULL;

        return 0;

    } else {

        module.shell_printf("error: usage: profile [on | off]\n");

        return -1;
    }
}
