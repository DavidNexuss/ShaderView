#pragma once
#include <init.hh>
#include <string>

struct ShaderWindowDescriptor
{
    std::string name;
    int width,height;
    bool no_decoration_flag;
    bool background;

    GLFWwindowsizefun windowSizeCallback;
    GLFWcursorposfun cursorPosCallback;
    GLFWscrollfun scrollCallback;
    GLFWkeyfun keyCallback;
};

class ShaderWindow
{
    GLFWwindow* window = nullptr;
    ShaderWindowDescriptor descriptor;
    bool shouldRecreate = false;

    static void destroy(GLFWwindow*& window);

    public:
    ShaderWindow();
    ShaderWindow(const ShaderWindowDescriptor& descriptor);
    ~ShaderWindow();

    void create();
    void toggleBackgroundMode();

    inline bool isBackground() const { return descriptor.background; };
    inline GLFWwindow* native() 
    {
        if(shouldRecreate) {
            shouldRecreate = 0;
            create();
        }
        return window; 
    }
};