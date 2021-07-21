#include <iostream>
#include <csignal>
#include <memory>
#include <GLFont.h>
#include <FTLabel.h>
#include "init.hh"
#include "main.hh"
#include "scale.hh"
#include "native.hh"
#include "window.hh"
#include "camera.hh"
#include "mesh.hh"
#include "cubemap.hh"
#include "profile.hh"

#ifndef __MINGW32__
#include "reload.hh"
#endif

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 450;
const float SCROLL_FACTOR = 0.05f;

float delta = 0.01f;

float zoom_level = 1.0f;

bool reload_shader = false;
bool zoom = false;

shared_ptr<GLFont> font;
unique_ptr<FTLabel> errorLabel = nullptr;
int fontSize  = 13;

bool errored = false;
bool reschanged = false;

Scale scale(1.0);
ShaderWindow mainWindow;
Camera camera;
Mesh screenMesh;
CubeMap cubeMap(4096);

void window_size_callback(GLFWwindow* window,int width,int height)
{
    glViewport(0,0, width, height);

    ProfileManager::currentProfile.resolution = glm::vec2(width, height);
    ProfileManager::currentProfile.flushResolution();

    reschanged = true;

    if (errorLabel != nullptr)
    {
        errorLabel->setWindowSize(width, height);
        errorLabel->setSize(width - 20, height - 20);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    const glm::vec2& resolution = ProfileManager::currentProfile.resolution;
    glm::vec2 mouse = glm::vec2(xpos / resolution[0] - 0.5f,ypos / resolution[1] - 0.5f);
    ProfileManager::currentProfile.mouse = mouse;
    ProfileManager::currentProfile.flushMouse();

    camera.inputDirection(mouse[0],mouse[1]);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (zoom)   //Perform zoom
    {
        zoom_level += yoffset * SCROLL_FACTOR;

    }else //Speed up simulation
    delta += yoffset * SCROLL_FACTOR;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    static float old_delta;
    bool label_changed = false;
    if (key == GLFW_KEY_SPACE)
         if (delta != 0.0f)
         {
             old_delta = delta;
             delta = 0.0f;
         } else delta = old_delta;
    else if (key == GLFW_KEY_LEFT_CONTROL)
    {
        zoom = not zoom;
    }
    else if(key == GLFW_KEY_KP_ADD)
    {
        if (errored)
        {
            fontSize += 1;
            label_changed = true;
        } else
        scale.set_resize_factor(scale.get_resize_factor() + 0.05);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT)
    {
        if (errored)
        {
            fontSize -= 1;
            label_changed = true;
        }else 
        scale.set_resize_factor(scale.get_resize_factor() - 0.05);
    }
    else if (key == GLFW_KEY_R)
    {
       // mainWindow.toggleBackgroundMode();
    }
    else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
    {   
        camera.setCubeView(key - GLFW_KEY_0);
    }
    else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9)
    {
        camera.setCubeView(key - GLFW_KEY_KP_0);
    }
    else if (key == GLFW_KEY_P)
    {
        scale.save_to_file("test.png");
    }
    else if (key == GLFW_KEY_I)
    {
        cubeMap.bake(camera,screenMesh,ProfileManager::currentProfile.iResolution);
        ProfileManager::currentProfile.flushUniforms();
        ProfileManager::currentProfile.setViewport();
    }
    if (label_changed) 
        errorLabel->setPixelSize(fontSize);
        
}

// -1 Reload
// 0 Quit
int load_shader(const char* fragment_shader_path,ShaderWindow& shaderwindow,GLuint& programID)
{
    char* buffer = nullptr;
    programID = LoadShaders(fragment_shader_path,buffer);
    if (programID == 0)
    {
        cerr << "Error loading shaders." << endl;
        cerr << buffer << endl;

        const glm::vec2& resolution = ProfileManager::currentProfile.resolution;

        errorLabel = unique_ptr<FTLabel>(new FTLabel(
          font,
          buffer,
          20,
          20,
          resolution[0],
          resolution[1],
          resolution[0],
          resolution[1]
        ));

        errorLabel->setPosition(20, 20);
        errorLabel->setSize(resolution[0] - 20, resolution[1] - 20);
        errorLabel->setPixelSize(fontSize);
        errorLabel->setIndentation(50);
        errorLabel->setAlignment(FTLabel::FontFlags::LeftAligned);
        errorLabel->setColor(1.0, 1.0, 1.0, 1.0);
        errored = true;
        GLFWwindow* window = shaderwindow.native();
        do
        {
            window = shaderwindow.native();
            glClear(GL_COLOR_BUFFER_BIT);
            errorLabel->render();
            glfwSwapBuffers(window);
            glfwPollEvents();
            if (reload_shader)
            {
                reload_shader = false;
                delete buffer;
                errored = false;
                return 1;
            }
        }
        while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
        
        delete buffer;
        errored = false;
        return 2;
    }
    return 0;
}

int draw_loop(ShaderWindow& shaderwindow,const Mesh& screenMesh,GLuint programID)
{

    glUseProgram( programID );

    ProfileManager::currentProfile.loadUniforms(programID);
    ProfileManager::currentProfile.setViewport(); 
    ProfileManager::currentProfile.flushUniforms();

    camera.setUniformID(ProfileManager::currentProfile.iCameraTransform);
    camera.update();

    int idx = 0;
    int texture_slot_id = glGetUniformLocation(programID,string("iChannel" + std::to_string(idx)).c_str() );

    while(texture_slot_id != -1)
    {
        string file_path = "iChannel" + std::to_string(idx) + ".png";
        GLuint texture = loadTexture(file_path.c_str());

        if (texture == 0)
        {
            cerr << "Error loading required texture " << texture_slot_id << endl;
            return 1;
        }
        
        glUniform1i(texture_slot_id,texture);
        idx++;
        texture_slot_id = glGetUniformLocation(programID,string("iChannel" + std::to_string(idx)).c_str() );
    }

    scale.flush(ProfileManager::currentProfile.iResolution);
    GLFWwindow* window = shaderwindow.native();
    do{

        window = shaderwindow.native();
        ProfileManager::currentProfile.flushTime();
        ProfileManager::currentProfile.flushZoom();

        scale.begin(ProfileManager::currentProfile.resolution[0],ProfileManager::currentProfile.resolution[1],ProfileManager::currentProfile.iResolution);
        screenMesh.draw();
        scale.end();

        glfwSwapBuffers(window);
        glfwPollEvents();

        ProfileManager::currentProfile.time += delta;
        ProfileManager::currentProfile.zoom += (zoom_level - ProfileManager::currentProfile.zoom) * 0.1f;


        if (reload_shader) {
            reload_shader = false;
            scale.dispose_framebuffer();
            return 2;
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    return 0;

}

int main(int argc, char *argv[])
{
    int base_index = 1;
    if (argc > base_index and string(argv[base_index]) == "--help")
    {
        cout << "Usage: " << argv[0] << " [fragment_shader_path]" << endl;
        cout << "If a shader file is not specified, the program will search for fragment.glsl in the current directory as its default shaders" << endl;
        cout << "For texture use uniform sampler2D iChannel* The program will search for textures as iChannel0.png iChannel1.png iChannel2.png..." << endl;

        cout << "The program will reload the shader if exists a modification in the shader source file" << endl << endl;

        cout << "Supported uniforms: " << endl;
        cout << "iResolution(for screen resolution)" << endl;
        cout << "iTime(for execution time)" << endl;
        cout << "iMouse(for mouse position) between [-0.5,0.5]" << endl;
        cout << "iChannel(for texture channel)" << endl;
        cout << "iZoom(for zoom level)" << endl;
        cout << "iCameraTransform(for first person camera transform)" << endl;

        cout << endl << "Use mouse scroll to change iTime speed, press control to change between zoom and speed increment" << endl;
        return 0;
    }

    bool no_decoration_flag = false;
    if (argc > base_index and string(argv[base_index]) == "--nodec")
    {
        no_decoration_flag = true;
        base_index++;
    }

    initialize_GLFW();
    
    mainWindow = ShaderWindow({
        .name = "ShaderView",
        .width = WIDTH,
        .height = HEIGHT,
        .no_decoration_flag = false,
        .background = false,
        .windowSizeCallback = window_size_callback,
        .cursorPosCallback = cursor_position_callback,
        .scrollCallback = scroll_callback,
        .keyCallback = key_callback
    });

    mainWindow.create();
    if(argc > 2)
    {
        unsigned long parent = stoul(argv[2]);
        std::cerr << "Reparenting to: " << parent << std::endl;
        reparentWindow(mainWindow.native(),parent);
    }

    /**Create screen mesh**/
    screenMesh = Mesh::createScreenMesh();

    font = shared_ptr<GLFont>(new GLFont("mono.ttf"));
    const char* fragment_shader_path = argc > base_index ? argv[base_index] : "fragment.glsl";

    #ifndef __MINGW32__
    InotifyHandler handler(fragment_shader_path);   //Autoreload shaders
    #endif
    
    int exit_code = 2;
    while(exit_code == 2)
    {
        GLuint programID;
        while((exit_code = load_shader(fragment_shader_path,mainWindow,programID))) 
        {
            if (exit_code == 2) break;
        }
        exit_code = draw_loop(mainWindow,screenMesh,programID);
        glDeleteProgram(programID);
    }
    return exit_code;
}
