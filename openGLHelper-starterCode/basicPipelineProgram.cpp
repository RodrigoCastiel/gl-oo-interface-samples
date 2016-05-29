#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "basicPipelineProgram.h"
using namespace std;


int BasicPipelineProgram::Init(const char * shaderBasePath) 
{
  // Try to build and set variable handles.
  if (BuildShadersFromFiles(shaderBasePath, "basic.vertexShader.glsl", "basic.fragmentShader.glsl") != 0
   && SetShaderVariableHandles() == 0)
  {
    cout << "Failed to build the pipeline program." << endl;
    return 1;
  }

  cout << "Successfully built the pipeline program." << endl;
  return 0;
}

void BasicPipelineProgram::SetViewMatrix(OpenGLMatrix& viewMatrix)
{
  // pass "v" to the pipeline program, as the view matrix
  GLfloat v[16];
  viewMatrix.GetMatrix(v);
  GLboolean isRowMajor = GL_FALSE; 
  glUniformMatrix4fv(this->h_viewMatrix, 1, isRowMajor, v);
}

void BasicPipelineProgram::SetModelMatrix(OpenGLMatrix& modelMatrix)
{
  // pass "m" to the pipeline program, as the model matrix
  GLfloat m[16];
  modelMatrix.GetMatrix(m);
  GLboolean isRowMajor = GL_FALSE; 
  glUniformMatrix4fv(this->h_modelMatrix, 1, isRowMajor, m);
}

void BasicPipelineProgram::SetProjectionMatrix(OpenGLMatrix& projectionMatrix)
{
  // pass "m" to the pipeline program, as the projection matrix
  GLfloat p[16];
  projectionMatrix.GetMatrix(p);
  GLboolean isRowMajor = GL_FALSE; 
  glUniformMatrix4fv(this->h_projectionMatrix, 1, isRowMajor, p);
}

int BasicPipelineProgram::SetShaderVariableHandles() 
{
  this->h_projectionMatrix = glGetUniformLocation(this->programHandle, "P");
  this->h_modelMatrix      = glGetUniformLocation(this->programHandle, "M");
  this->h_viewMatrix       = glGetUniformLocation(this->programHandle, "V");

  if (this->h_projectionMatrix == GL_INVALID_OPERATION)
  {
    cout << "Projection matrix location in shader wasn't found. " << endl;
    return 0;
  }
  else if (this->h_modelMatrix == GL_INVALID_OPERATION)
  {
    cout << "Model matrix location in shader wasn't found. " << endl;
    return 0;
  }
  else if (this->h_viewMatrix == GL_INVALID_OPERATION)
  {
    cout << "View matrix location in shader wasn't found. " << endl;
    return 0;
  }
  else
  {
    return 1;  
  }
}

