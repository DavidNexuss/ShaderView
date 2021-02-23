#include "util.hh"

extern GLuint fb,color,depth;

void initialize_framebuffer();
void dispose_framebuffer();

bool is_framebuffer();

void enable_framebuffer();
void disable_framebuffer(int width, int height);

void set_framebuffer_size(int width,int height);
