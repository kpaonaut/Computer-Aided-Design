### tricky problem with `glm::perspective`:

`glm::perspective(FoV angle in radians, aspect ratio, near plane, far plane)` usage explained:
Here, near plane and far plane are the __relative__ depth value from the camera. `near plane` has to be greater than 0, otherwise openGL depth check will fail. Also, `far plane` value cannot be too large, otherwise nothing will be displayed. See [this thread](https://stackoverflow.com/questions/41935764/opengl-depth-test-is-not-working) for details.