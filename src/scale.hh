#pragma once
#include "util.hh"
#include <string>

class Scale
{
    GLuint fb = 0,color = 0,depth = 0;

    int screen_width;
    int screen_height;

    float resize_factor;
    float old_factor;

    int fbo_width;
    int fbo_height;
    
    void initialize_framebuffer();

    void resize(int screenWidth, int screenHeight,GLuint shaderRes);

    public:
    Scale();
    Scale(float resize_factor);
    Scale(int screenWidth,int screenHeight,float resizeFactor,GLuint shaderRes = 0);
    ~Scale();

    void dispose_framebuffer();
    float get_render_width() const;
    float get_render_height() const;
    void set_resize_factor(float factor);
    float get_resize_factor() const;

    void flush(GLuint shaderRes);
    void begin(GLuint shaderRes);
    void begin(int screenWidth,int screenHeight,GLuint shaderRes);
    void end();

    void save_to_file(const std::string& path);
};
