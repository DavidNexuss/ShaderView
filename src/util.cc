#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stb_image.h>
#include <cstring>
#include "util.hh"

using namespace std;

GLuint LoadShaders(const char * fragment_file_path,char* &errorBuffer)     //Will return 1 if theres an error with shader loading
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Fragment Shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream;
    FragmentShaderStream.open(fragment_file_path);
	if(FragmentShaderStream.is_open()){
		stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}else{
		string errorMsg = "Impossible to open file " + string(fragment_file_path);
		errorBuffer = new char[errorMsg.length() + 1];
		memcpy(errorBuffer,errorMsg.c_str(),errorMsg.length());
		
		cerr << "Impossible to open " << fragment_file_path << endl;
        return 0;
    }

	GLint Result = GL_FALSE;
	int InfoLogLength;
	
    static const char* VertexSourcePointer = "#version 330 core \n \
    layout(location = 0) in vec3 vertexPosition_modelspace; \n \
    void main() \n \
    { \n \
        gl_Position.xyz = vertexPosition_modelspace; \n \
        gl_Position.w = 1.0; \n \
    } \n";

	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if ( InfoLogLength > 0 ){
		errorBuffer = new char[InfoLogLength+1];
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, errorBuffer);
		return 0;
	}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path);
	
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		errorBuffer = new char[InfoLogLength+1];
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, errorBuffer);
		return 0;
	}

	// Link the program
	//printf("Linking program\n");
	
    GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		errorBuffer = new char[InfoLogLength+1];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, errorBuffer);
        return 0;
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

#include "basicRead.hh"

GLuint loadTexture(const char* filename)   //Returns 0 if there is an with the texture loading
{
    std::vector<uint8_t> buffer = Nutils::readFromFile(filename);
    if(buffer.size() == 0) { 
        std::cerr << "Empty buffer" << std::endl;
    }
    else { 
        std::cerr << "Reading with " << buffer.size() << std::endl;
    }

    int width, height,nrChannels;
    unsigned char* data = stbi_load_from_memory(buffer.data(),buffer.size(), &width, &height, &nrChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);     //will return 0 if there is an error with texture generation

    if (texture != 0) {
        glActiveTexture(GL_TEXTURE0 + texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, width, height,0,nrChannels == 3 ? GL_RGB : GL_RGBA,GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cerr << "Failed to create OpenGL texture" << endl;
    }

    stbi_image_free(data);

    return texture;
}
