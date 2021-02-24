# ShaderView, a shader renderer

ShaderViewer is a simple utility to render glsl shader with the minimum overhead and setup posible,
it will pass to the shader some uniforms and textures.

![What is this](showcase.png)
One shader renderered with this program.

### Made with some code from the website http://www.opengl-tutorial.org

### Future features
Autogenerate uniform stubs in glsl shader to guarantee compatibility with ShaderToy shaders

Add FPS counter and window size label

### Compiling
Compiles using make and requires opengl glew and glm installed
Requires ttf called mono.ttf int the build directory
