# OpenGL-Fractal-Generator
A C++ program that renders fractals using OpenGL.
The program generates the image data using CPU multi-threading,
it then creates a texture from the image data and draws the texture onto a quad using OpenGL.

A video of the program can be seen here: https://www.youtube.com/watch?v=Ofb5bC0LnsQ

The program starts by showing if the config file was loaded properly,
and if it was, it shows the loaded values the program will use. 
(An explanation for each config value is explained that the bottom)

After the program shows you the loaded config values, you must press any key to continue.

It will then create a window and calculate a test image of the fractal, 
so you can see how long it took to calculate one frame of the fractal at max iterations (for consistent tests), 
the time it took is displayed in the console.

If you then enter a key to continue after the test is run,
then you will start the animated display mode of the fractal.

The fractal will be recalculated and displayed every frame.
In this mode the iterations starts at 0, and is increased by 0.2 every frame
until the iterations hits the max iterations set in the config file.
So you can see the fractal animate as the number of iterations is constantly increasing.

This mode also allows you to move around the fractal using input.

The input controls are as follows:
a          - Moves left.
d          - Moves right.
w          - Moves up.
s          - Moves down.
left shift - Zooms out of the fractal.
space      - Zooms into the fractal.




The loaded config values are as follows:

Iterations: 
The number of times the complex number is multiplied by itself,
before we decide whether it is inside the Mandelbrot set.

Threads: 

The number of threads the Threadpool will create for the WorkQueue to use.
This is equivalent to the number of logical cores the program will use when calculating the fractal. 
If the value in the config file is above the supported amount or less than 1,
the value will be set to the max amount of logical cores the hardware supports.

Vec2 optimization:

This optimization uses a custom struct of a 2D point vector for the complex variable, instead of using std::complex.
I'm not sure exactly why it works, but during my tests it could give up to a 1000% increase in performance.
If this is set to false, it will use std::complex instead.


Pixel skipping optimizations:

The pixel skipping optimization is something I thought of based on an assumption about how I thought fractals might work.
My assumption was that if the current pixel was surrounded by pixels all having the same colour,
then the current pixel will also be that colour.

So we can skip the costly iterative calculation for some pixels, 
by checking whether the 4 surrounding pixels have the same colour, 
and setting the current pixel to that colour if they are all the same.

I then realised that for this optimization to work, the 4 orthogonally adjacent pixels of the current pixel,
will need to have already been calculated to know what colour they have been assigned.

The way I did this, was that I calculated every second pixel, 
over two different iterations, so that it was calculated in a checkerboard pattern.
The first iteration calculates the black squares of the checkerboard, 
and the second iteration calculates the white squares of the checkerboard.

The first iteration doesn't use any form of pixel skipping, because the surrounding pixels haven't been calculated yet.
But we can use the pixel skipping during the second iteration. 
So at most this can skip the costly iterative calculation for slightly less than 50% of the pixels
