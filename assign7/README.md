Include information for grader about your assign7 here

Profiler output:

Pi> [5]$ profile off
profile: 14299 msecs elapsed, collected 28598 samples

Counts | pc           | <fn>
-------+--------------+-----------------
12944  | 0x40006f5c   | <rb_empty+24>
6318   | 0x40006f48   | <rb_empty+4>
993    | 0x40004f78   | <font_get_glyph+260>
727    | 0x40003db0   | <gl_draw_char+192>
671    | 0x400039e0   | <gl_get_width+24>
412    | 0x40003db4   | <gl_draw_char+196>
343    | 0x40003304   | <gl_get_height+24>
338    | 0x40003b60   | <gl_draw_pixel+112>
322    | 0x40004cc0   | <timer_delay_us+44>
260    | 0x400018c8   | <ps2_read+44>
257    | 0x40003ab0   | <gl_clear+72>
254    | 0x40000538   | <gl_draw_pixel+48>
237    | 0x40003b20   | <gl_draw_pixel+60>
237    | 0x40003ddc   | <gl_draw_char+236>
233    | 0x40003b4c   | <gl_draw_pixel+92>
227    | 0x40003bac   | <gl_clear+68>
227    | 0x40003b54   | <gl_draw_pixel+100>
224    | 0x40003b44   | <gl_draw_pixel+84>
220    | 0x40003b3c   | <gl_draw_pixel+76>

I have not tried to improve preformance yet, but it gave me great insight on to what was slowing my program down. One of the things I learned is that my ring buffer is causing the most bottlenecking. I really enjoyed learning about this. 