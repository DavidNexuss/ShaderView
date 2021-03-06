#include <iostream>
#include <csignal>
#include <memory>
#include <GLFont.h>
#include <FTLabel.h>
#include "init.hh"
#include "main.hh"
#include "scale.hh"

#ifndef __MINGW32__
#include "reload.hh"
#endif

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 450;
const float SCROLL_FACTOR = 0.05f;

float g_time = 0.0f;
float delta = 0.01f;

float zoom_level = 1.0f;
float current_zoom_level = 1.0f;

bool reload_shader = false;
bool zoom = false;
/*** Input callbacks ***/

GLuint iResolution;
GLuint iZoom;

float resolution[2];
float mouse_position[2];

shared_ptr<GLFont> font;
unique_ptr<FTLabel> errorLabel = nullptr;
int fontSize  = 13;

bool errored = false;
bool reschanged = false;

Scale scale(1.0);
void window_size_callback(GLFWwindow* window,int width,int height)
{
    reschanged = true;
    resolution[0] = width;
    resolution[1] = height;

    glViewport(0,0, resolution[0], resolution[1]);
    glUniform2fv(iResolution,1,resolution);

    if (errorLabel != nullptr)
    {
        errorLabel->setWindowSize(width, height);
        errorLabel->setSize(resolution[0] - 20, resolution[1] - 20);
    }
}

GLuint iMouse;
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_position[0] = xpos / resolution[0] - 0.5f;
    mouse_position[1] = ypos / resolution[1] - 0.5f;
    glUniform2fv(iMouse,1,mouse_position);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (zoom)
    {
        zoom_level += yoffset * SCROLL_FACTOR;

    }else 
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
    if (label_changed) 
        errorLabel->setPixelSize(fontSize);
        
}

/** Screen mesh **/

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,

    -1.0f, -1.0f,0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f,-1.0f,0.0f,
};

struct Mesh
{
    GLuint vao,vertexbuffer;

    inline void bind() const { glBindVertexArray(vao); }
    inline void draw() const
    {
        //glBindVertexArray(vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
               0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
        );
        glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
    }
};

Mesh createScreenMesh()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    return {vao, vertexbuffer};
}

// -1 Reload
// 0 Quit
int load_shader(const char* fragment_shader_path,GLFWwindow* window,GLuint& programID)
{
    char* buffer = nullptr;
    programID = LoadShaders(fragment_shader_path,buffer);
    if (programID == 0)
    {
        cerr << "Error loading shaders." << endl;
        cerr << buffer << endl;

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
        do
        {
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

int draw_loop(GLFWwindow* window,const Mesh& screenMesh,GLuint programID)
{
    //Enable shader
    glUseProgram( programID );
    //Uniform initialization
    //Search for fixed uniforms 
    GLuint iTime = glGetUniformLocation(programID,"iTime");
    iResolution = glGetUniformLocation(programID,"iResolution");
    iMouse = glGetUniformLocation(programID,"iMouse");
    iZoom = glGetUniformLocation(programID,"iZoom");

    //Needed to correct window resolution to same as uniform iResolution
    glViewport(0, 0, resolution[0], resolution[1]); 
    
    //Uniform set
    glUniform2fv(iResolution,1,resolution);
    glUniform2fv(iMouse,1,mouse_position);
    glUniform1f(iZoom,zoom_level);


    //Search for texture channel uniforms and try to load the textures
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

    scale.flush(iResolution);
    do{

        glUniform1f(iTime,g_time);
        glUniform1f(iZoom,current_zoom_level);

        scale.begin(resolution[0],resolution[1],iResolution);
        screenMesh.draw();
        scale.end();

        glfwSwapBuffers(window);
        glfwPollEvents();

        g_time += delta;
        if (reload_shader)
        {
            reload_shader = false;
            scale.dispose_framebuffer();
            return 2;
        }

        current_zoom_level += (zoom_level - current_zoom_level) * 0.1f;
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

        cout << endl << "Use mouse scroll to change iTime speed, press control to change between zoom and speed increment" << endl;
        return 0;
    }

    bool no_decoration_flag = false;
    if (argc > base_index and string(argv[base_index]) == "--nodec")
    {
        no_decoration_flag = true;
        base_index++;
    }

    GLFWwindow* window;
    int width = WIDTH, height = HEIGHT;

    //Initialize every 
    if (initialize_GLFW() or initialize_window(window,width,height,no_decoration_flag) or initialize_GLEW(window))
    {
        cerr << "Failed to initialize GLFW" << endl;
        return 1;
    }

    initialize_keys(window);
    
    /**Set callbacks **/
    glfwSetWindowSizeCallback(window,window_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    /**Init window size**/
    window_size_callback(window,width,height);

    /**Create screen mesh**/
    Mesh screenMesh = createScreenMesh();

    font = shared_ptr<GLFont>(new GLFont("mono.ttf"));
    const char* fragment_shader_path = argc > base_index ? argv[base_index] : "fragment.glsl";

    #ifndef __MINGW32__
    InotifyHandler handler(fragment_shader_path);   //Autoreload shaders
    #endif
    
    int exit_code = 2;
    while(exit_code == 2)
    {
        GLuint programID;
        while((exit_code = load_shader(fragment_shader_path,window,programID))) 
        {
            if (exit_code == 2) break;
        }
        exit_code = draw_loop(window,screenMesh,programID);
        glDeleteProgram(programID);
    }
    return exit_code;
}
