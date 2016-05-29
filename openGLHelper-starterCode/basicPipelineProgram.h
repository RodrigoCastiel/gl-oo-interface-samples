#pragma once
#include "pipelineProgram.h"
#include "openGLMatrix.h"

class BasicPipelineProgram : public PipelineProgram
{
public:
  int Init(const char * shaderBasePath);  // init the program, "shaderBasePath" is the path to the folder containing the shaders
  
  void SetViewMatrix(OpenGLMatrix& viewMatrix);
  void SetModelMatrix(OpenGLMatrix& modelMatrix);
  void SetProjectionMatrix(OpenGLMatrix& projectionMatrix);

protected:
  virtual int SetShaderVariableHandles();

  GLint h_projectionMatrix; // handle to the P matrix in the shader
  GLint h_modelMatrix;  // handle to M matrix in the shader
  GLint h_viewMatrix;   // handle to V matrix in the shader

  // note: we can use the general syntax: h_name is a handle to the shader variable "name"
};

