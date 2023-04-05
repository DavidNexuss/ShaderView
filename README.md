# ShaderView, a shader renderer

ShaderViewer is a simple utility to render glsl shader with the minimum overhead and setup posible,
it will pass to the shader some uniforms and textures.

![What is this](showcase/showcase.png)
One shader renderered with this program.

*Made with some code from the website http://www.opengl-tutorial.org*

## Build instructions
Just run:

``` sh
./make.sh
```

and if everything goes right, you should see a shaderview executable in the project root

Then run:

``` sh
./shaderview shaders/someshader.glsl
```
