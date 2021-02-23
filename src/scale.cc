#include "scale.hh"

GLuint fb = 0;
GLuint color = 0;
GLuint depth = 0;

int fbo_width = 512;
int fbo_height = 512;

void set_framebuffer_size(int width, int height)
{
    if (is_framebuffer()) dispose_framebuffer();

    fbo_width = width;
    fbo_height = height;

    initialize_framebuffer();
}

void initialize_framebuffer()
{
    glGenFramebuffers(1, &fb);
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(   GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            fbo_width, fbo_height,
            0, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

}

void dispose_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteFramebuffers(1,&fb);
    glDeleteTextures(1, &color);
    glDeleteRenderbuffers(1, &depth);
}
bool is_framebuffer()
{
    return fb != 0;
}

void enable_framebuffer()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glViewport(0,0, fbo_width, fbo_height);

    glClearColor(1,1,1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void disable_framebuffer(int width, int height)
{
    const float aspect = (float)width/(float)height;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, width, height);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glClearColor(1.,1.,1.,0.);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, color);

    glBlitFramebuffer(0, 0, fbo_width, fbo_height, 0, 0, width, height,
                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

}
