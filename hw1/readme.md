# Displaying and Operating on an Eight Model with OpenGL

![](effects.gif)

## Usage
On Mac, we can directly run this program by `./a` under this directory.

To compile and run, use 
~~~
g++ -std=c++11 -stdlib=libc++ -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework GLUT -o a main.cpp ../glad.c
~~~

## Open an obj file
`readObjFile()` in `parse_obj.cpp` is the function I wrote in order to read the `.obj` file. This is a bit of a hassel but trivial in terms of its ideas. There seems no light-weight, open-source library out there for the parsing of `obj` files.

## Functionalities
As shown in the gif above, this project enables several functions in the displaying window.

* object rotation (up, down, left, right, M, N keys)
* object movement (W, S, A, D, Z, X keys)
* display mode selection (P, F, L, O keys)
* color gradual change (R, G, B keys)

### Display Mode Selection
P: only display points
F: display triangles, in random colors
L: only display lines
O: display both triangle faces and lines

### Color Gradual Change
R, G, B represent red, green, and blue elements in the color, respectively. When we keep pressing R, for example, the red element will be fluctuating in a sinusoidal pattern. We can also press several keys at the same time.

### Miscellaneous
**Problem:**
There is a problem with this openGL lib in GLFW. `glPolygonMode(GL_FRONT, GL_LINE);` does not work. In face, only the mode with `GL_FRONT_AND_BACK` works. Therefore, we avoided using the front-only mode for displaying.