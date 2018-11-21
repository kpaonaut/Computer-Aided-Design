### tricky problem with `glm::perspective`:

`glm::perspective(FoV angle in radians, aspect ratio, near plane, far plane)` usage explained:
Here, near plane and far plane are the __relative__ depth value from the camera. `near plane` has to be greater than 0, otherwise openGL depth check will fail. Also, `far plane` value cannot be too large, otherwise nothing will be displayed. See [this thread](https://stackoverflow.com/questions/41935764/opengl-depth-test-is-not-working) for details.

### tricky problem with `textShader.Use()`:

There is another problem with the library which cost me a lot of time to debug. When I am using two different shaders for text and image rendering separately, the command `glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "view");, 1, GL_FALSE, glm::value_ptr(view));` only passes the value `view` into the vertexShader after the command `textShader.Use()` is invoked. Otherwise, the `view` matrix in the vertex shader of `textShader` will not be properly assigned! This is unreasonable given that `glGetUniformLocation(textShader.Program, "view")` has already specified which shader we want to pass the view matrix to.