/* File: timer_asm.s
 * ------------------
 * Nicole Mulvey
 */

.attribute arch, "rv64imac_zicsr"

.globl timer_get_ticks
timer_get_ticks:
    /***** TODO: Your code goes here *****/
    # Take care to use correct dest register for return value
    # so as to interoperate with C calling conventions
    csrr a0, 0xC01
    ret 
