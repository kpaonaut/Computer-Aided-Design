# Bezier Plane
![](effect.gif)

In this project, we draw a [Bezier spline](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) plane according to 25 control points.

We also utilize the **tessellation** introduced by openGL V4.0+.

## Maths

The maths behind the Bezier plane is easy to understand. The 1-D Bezier is just a weighted sum of all control points. Similarly, we can expand the formula from 1-D to 2-D. In 2-D, a point on the plane is a weighted sum along the Y-axis of {the weighte sum of all control points along X-axis}. Calculation is carried out in `main.tes.glsl`.