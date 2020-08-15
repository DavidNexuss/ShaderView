#include <iostream>
#include <csignal>

#include "init.hh"
#include "reload.hh"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 450;

float g_time = 0.0f;
float delta = 0.01f;

/*** Signal Handler ***/

/*** Input callbacks ***/

GLuint iResolution;
float resolution[2];
float mouse_position[2];
void window_size_callback(GLFWwindow* window,int width,int height)
{

    resolution[0] = width;
    resolution[1] = height;

    glUniform2fv(iResolution,1,resolution);
    glViewport(0, 0, width, height);

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
    delta += yoffset * 0.05f;
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

int draw_loop(GLFWwindow* window,const char* fragment_shader_path,InotifyHandler& handler)
{
    GLuint programID = LoadShaders(fragment_shader_path);
    if (programID == 0)
    {
        cerr << "Error loading shaders." << endl;
        return 1;
    }

    GLuint vertexbuffer; // Generate 1 buffer, put the resulting identifier in vertexbuffer

    glGenBuffers(1, &vertexbuffer); // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW); // Give our vertices to OpenGL.

    //Search for uniforms
    GLuint iTime = glGetUniformLocation(programID,"iTime");
    iResolution = glGetUniformLocation(programID,"iResolution");
    iMouse = glGetUniformLocation(programID,"iMouse");

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
    );
        // Draw the triangle !
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glDisableVertexAttribArray(0);


    glUseProgram( programID );

    glUniform2fv(iResolution,1,resolution); //TODO: Refactor duplicated code
    glUniform2fv(iMouse,1,mouse_position);
    glViewport(0, 0, resolution[0], resolution[1]);

    do{
        glClear( GL_COLOR_BUFFER_BIT);

        glUniform1f(iTime,g_time);

        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();

        g_time += delta;
        if (handler.reload_shader)
        {
            handler.reload_shader = false;
            return 2;
        }
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    return 0;

}
int main(int argc, char *argv[])
{

    if (argc > 1 and string(argv[1]) == "--help")
    {
        cout << "Usage: " << argv[0] << " [fragment_shader_path]" << endl;
        cout << "If a shader file is not specified, the program will search for fragment.glsl" << endl;
        cout << "in the current directory as its default shaders" << endl << endl;

        cout << "Supported uniforms: iResolution, iTime and iMouse" << endl;
        cout << "Use mouse scroll to change iTime speed" << endl;
        return 0;
    }

    GLFWwindow* window;
    int width = WIDTH, height = HEIGHT;

    //Initialize every 
    if (initialize_GLFW() or initialize_window(window,width,height) or initialize_GLEW(window))
    {
        cerr << "Failed to initialize GLFW" << endl;
        return 1;
    }

    initialize_keys(window);
    
    /**Set callbacks **/
    glfwSetWindowSizeCallback(window,window_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /**Init window size**/
    window_size_callback(window,width,height);

    //Create VAO 
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    

    const char* fragment_shader_path = argc > 1 ? argv[1] : "fragment.glsl";
    InotifyHandler handler(fragment_shader_path);

    int exit_code = 2;
    while(exit_code == 2)
    {
        exit_code = draw_loop(window,fragment_shader_path,handler);
    }
    return exit_code;
}
