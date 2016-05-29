/*******************************************
*
*  CSCI420 - Computer Graphics USC
*  Author: Rodrigo Castiel
*
*  This class implements the Phong Lighing
*  model.
*
********************************************/
#pragma once

#include "openGLHeader.h"
#include "glutHeader.h"

#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

namespace gloo
{

class Light
{
public:
  Light(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : mPipelineProgram(pipelineProgram), mProgramHandle(programHandle)
  { } 

  // Sets the light in the environment in camera coordinates (sends to shader uniform).
  void Position(OpenGLMatrix& viewMatrix, int id);

  // Updates (for an animation).
  void Animate();

  // Getters/Setters.
  void SetComponents(GLfloat La, GLfloat Ld, GLfloat Ls);
  void SetPosition(const glm::vec3& pos);
  void SetDirection(const glm::vec3& dir);

  // SetPipelineProgramParam must be called before rendering.
  inline void SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, GLuint programHandle) 
  { 
    mProgramHandle   = programHandle;
    mPipelineProgram = pipelineProgram;
  }

private:
  glm::vec3 mPos {0.0f,  200.0f, 0.0f};  // Center in world coordinates.
  glm::vec3 mDir {0.0f, 1.0f,  0.0f};    // Direction vector in world coordinates.

  GLfloat mLa {0.1f};  // Ambient component.
  GLfloat mLd {0.8f};  // Diffuse component.
  GLfloat mLs {0.1f};  // Specular component.

  BasicPipelineProgram *mPipelineProgram {nullptr};
  GLuint mProgramHandle {0};
};

inline 
void Light::SetComponents(GLfloat La, GLfloat Ld, GLfloat Ls)
{
  mLa = La;
  mLd = Ld;
  mLs = Ls;
}

inline 
void Light::SetPosition(const glm::vec3& pos)
{
  mPos = pos;
}

inline
void Light::SetDirection(const glm::vec3& dir)
{
  mDir = dir;
}

}  // namespace gloo.
