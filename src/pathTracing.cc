#include "pathTracing.hh"
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>

extern "C" { 
#include <scene.h>
#include <examples/examples.h>
}
#include "profile.hh"


static GLuint cudaTexture;

SceneDesc defaultDesc() {

  SceneDesc sceneDesc;
  sceneDesc.maxMeshes           = 300;
  sceneDesc.maxObjects          = 400;
  sceneDesc.maxMaterials        = 300;
  sceneDesc.maxTextures         = 10;
  sceneDesc.maxVertexBuffer     = 10;
  sceneDesc.maxIndexBuffer      = 10;
  sceneDesc.frameBufferWidth    = 500;
  sceneDesc.frameBufferHeight   = 500;
  sceneDesc.numThreads          = 4;
  sceneDesc.iterationsPerThread = 32;
  sceneDesc.rayDepth            = 4;
  sceneDesc.framesInFlight      = 1;
  sceneDesc.frameDelta          = 0.1;
  sceneDesc.fWriteClamped       = 1;
  return sceneDesc;
}


Scene scene;
int pathTracingLoop(ShaderWindow &shaderwindow, const TMesh &screenMesh, GLuint programID) { 

  glUseProgram(programID);
  glBindTexture(GL_TEXTURE_2D, cudaTexture);
  GLFWwindow *window = shaderwindow.native();
  screenMesh.bind();
  do {
    sceneRun(&scene);
    sceneDownload(&scene);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scene.desc.frameBufferWidth, scene.desc.frameBufferHeight, GL_RGB, GL_FLOAT, sceneGetFrame(&scene, 0));
    ProfileManager::currentProfile.loadUniforms(programID);
    ProfileManager::currentProfile.setViewport();
    ProfileManager::currentProfile.flushUniforms();

    screenMesh.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  while (glfwWindowShouldClose(window) == 0);

  return 0;
}

void pathTracingInit(int argc, char** argv) { 
  glGenTextures(1, &cudaTexture);
  glBindTexture(GL_TEXTURE_2D, cudaTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SceneDesc sceneDesc = defaultDesc();
  scene = sceneCreate(sceneDesc);
  scene2(&scene);
  sceneUpload(&scene);

  SceneInput inp = sceneInputHost(&scene);
  scene2Loop(inp.constants);

  sceneUploadObjects(&scene);
  sceneRun(&scene);
  sceneDownload(&scene);
  sceneWriteFrame(&scene, "result.png", 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, sceneDesc.frameBufferWidth, sceneDesc.frameBufferHeight, 0, GL_RGB, GL_FLOAT, sceneGetFrame(&scene,0));
}
