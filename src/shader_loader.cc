#include "shader_loader.hh"
 /*
ShaderLoader::ShaderLoader(const char* fontPath)
{
    fontSize = 13;
}

ShaderLoader::~ShaderLoader()
{
}

GLuint ShaderLoader::load_shader(const char* fontPath,bool& reload_shader)
{
    char* buffer = nullptr;
    programID = LoadShaders(fragment_shader_path,buffer);
    if (programID == 0)
    {
        cerr << "Error loading shaders." << endl;
        cerr << buffer << endl;

        int width,height;
        glfwGetFramebufferSize(window, &width, &height);
        errorLabel = unique_ptr<FTLabel>(new FTLabel(
          font,
          buffer,
          20,
          20,
          width,
          height,
          width,
          height
        ));

        errorLabel->setPosition(20, 20);
        errorLabel->setSize(width - 20, height - 20);
        errorLabel->setPixelSize(fontSize);
        errorLabel->setIndentation(50);
        errorLabel->setAlignment(FTLabel::FontFlags::LeftAligned);
        errorLabel->setColor(1.0, 1.0, 1.0, 1.0);
        is_errored = true;
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
                is_errored = false;
                return 1;
            }
        }
        while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
        
        delete buffer;
        is_errored = false;
        return 2;
    }
    return 0;
}

void ShaderLoader::update_screen_size(int width, int height)
{
    if (errorLabel != nullptr)
    {
        errorLabel->setWindowSize(width, height);
        errorLabel->setSize(resolution[0] - 20, resolution[1] - 20);
    }
}

void ShaderLoader::update_font_size(int newSize)
{

}

int ShaderLoader::get_font_size() const
{
    return fontSize;
} */
