#include "cubemap.hh"

void CubeMap::bake(Camera camera,Mesh& mesh,GLuint shaderRes)
{
    for (int i = 0; i < 6; i++)
    {
        camera.setCubeView(cameraDirections[i]);
        scales[i].flush(shaderRes);
        scales[i].begin(resolution,resolution,shaderRes);
        mesh.draw();
        scales[i].save_to_file(std::string(std::to_string(i) + ".png").c_str());
        scales[i].end();
    }
}