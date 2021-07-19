#include "scale.hh"
#include <iostream>
#include <vector>
#include <stb_image_write.hh>

using namespace std;

Scale::Scale() : Scale(1.0) { }
Scale::Scale(float resizeFactor)
{
    resize_factor = resizeFactor;
}
Scale::Scale(int screenWidth, int screenHeight,float resizeFactor,GLuint shaderRes)
{
    resize_factor = resizeFactor;
    resize(screenWidth, screenHeight, shaderRes);
}
Scale::~Scale()
{
    dispose_framebuffer();
}

float Scale::get_render_width() const
{
    return screen_width * resize_factor;
}
float Scale::get_render_height() const
{
    return screen_height * resize_factor;
}
void Scale::set_resize_factor(float factor)
{
    old_factor = resize_factor;
    resize_factor = factor;
}
void Scale::resize(int screenWidth, int screenHeight,GLuint shaderRes)
{
    dispose_framebuffer();
    screen_width = screenWidth;
    screen_height = screenHeight;
    float resolution[2] = {get_render_width(), get_render_height()};
    glUniform2fv(shaderRes,1,resolution);
    initialize_framebuffer();
    old_factor = resize_factor;
}

void Scale::flush(GLuint shaderRes)
{
    resize(screen_width,screen_height,shaderRes);
}
float Scale::get_resize_factor() const
{
    return resize_factor;
}

void Scale::initialize_framebuffer()
{
    glGenFramebuffers(1, &fb);
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(   GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            get_render_width(), get_render_height(),
            0, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, get_render_width(), get_render_height());
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

}

void Scale::dispose_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteFramebuffers(1,&fb);
    glDeleteTextures(1, &color);
    glDeleteRenderbuffers(1, &depth);
}

void Scale::begin(GLuint shaderRes) {
    begin(screen_width,screen_height,shaderRes);
}

void Scale::begin(int screenWidth, int screenHeight,GLuint shaderRes)
{
    if (old_factor != resize_factor || fb == 0 || screenWidth != screen_width || screenHeight != screen_height) resize(screenWidth, screenHeight,shaderRes);

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glViewport(0,0, get_render_width(), get_render_height());

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Scale::end()
{
    const float aspect = (float)screen_width/(float)screen_height;
    
    //Draw framebuffer to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, screen_width, screen_height);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, color);

    glBlitFramebuffer(0, 0, get_render_width(), get_render_height(), 0, 0, screen_width, screen_height,
                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

}

void Scale::save_to_file(const char* path)
{
    int width = get_render_width();
    int height = get_render_height();

    vector<uint8_t> buffer(width * height * 3);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,buffer.data());
    stbi_write_png(path,width,height,3,buffer.data(),width * 3);

}
