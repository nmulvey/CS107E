Include information for grader about your assign6 here


I helped other debug and talked about ideas with them. 

Extension:

Here are the sources I used:

https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)

https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage.html

https://en.wikipedia.org/wiki/Line_drawing_algorithm

https://medium.com/@dillihangrae/scanline-filling-algorithm-852ad47fb0dd

2. For the line drawing, I did  a modified version of the Digital Differential Analyzer algorithm because it was extremely easy to understand and uses basic math to do a line drawing. 

When trying to make my line drawing algorithm I needed to calculate the differenves between the x and y coordinates. Then I needed to determine the number of steps by taking the absolute values of dx and dy. I then incrament by a number of steps and round to the nearest integer if a fraction, and keep drawing small lines between until I am at the end coordinates.

3. For the Triangle filling method I relied heavily on my draw_line function. I found the scanline filling to make the most sense when looking into how to make a traingle. It finds the point in the scanline with the lines of the triangles edges. Using interpolation, and it then sorts the points from left to right. Using the points it then draws the line and does it for the entire triangle. 

4. Used this link to update the makefiles: https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html

When running my code with hardware float I got approximately: 226425 microseconds for the runtime. Without support for hardware float I got approxiamtely: 227049 microseconds 


