#pragma once
#include "scale.hh"
#include "camera.hh"
#include "mesh.hh"

#include <vector>
#include <memory>

struct CubeMap
{
    std::vector<Scale> scales;

    int resolution;
    CubeMap(int resolution) : scales(6)
    {
        this->resolution = resolution;
    }

    void bake(Camera camera,TMesh& mesh,GLuint shaderRes);
};
