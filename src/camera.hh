#pragma once 
#include <glm/glm.hpp>
#include <glm/ext.hpp>


constexpr int cameraDirections[6] = {5,4,6,8,2,1};
struct Camera
{
    GLuint cameraTransformID;
    glm::mat4 cameraTransform;

    double x,y,z;

    Camera() {
        cameraTransformID = -1;
        cameraTransform = glm::mat4(1.0f);

    }
    inline void setUniformID(GLuint uniformID) {
        cameraTransformID = uniformID;
    }

    inline void inputDirection(double x,double y,double z = 0.0) {
        if(cameraTransformID < 0) return;
        this->x = x;
        this->y = y;
        this->z = z;

        compute();
        update();
    }
    
    inline void compute()
    {
        cameraTransform = glm::mat4(1.0f);
        cameraTransform = glm::rotate<float>(cameraTransform,z * M_PI * 2,glm::vec3(0,0,1));
        cameraTransform = glm::rotate<float>(cameraTransform,x * M_PI * 2,glm::vec3(0,1,0));
        cameraTransform = glm::rotate<float>(cameraTransform,y * M_PI * 2,glm::vec3(1,0,0));



    }
    inline void update() {
        glUniformMatrix4fv(cameraTransformID,1,false,&cameraTransform[0][0]);
    }

    inline void setCubeView(int id)
    {
        switch(id)
        {
            case 5: inputDirection(0.0,0.0)  ;break;
            case 4: inputDirection(-0.25,0.0) ;break;
            case 6: inputDirection(0.25,0.0)  ;break;
            case 8: inputDirection(0.0,0.25)  ;break;
            case 2: inputDirection(0.0,-0.25) ;break;
            case 1: inputDirection(-0.5,0.0) ;break;
        }
    }
};
