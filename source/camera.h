/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#pragma once

#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"


namespace gloo
{

class SceneObject;

// TODO: DOCUMENT EVERYTHING !!!

class Camera
{
public:
  enum CameraType { EDITOR, FPV };

  Camera(BasicPipelineProgram* pipelineProgram, GLuint programHandle) 
  : mPipelineProgram(pipelineProgram), mProgramHandle(programHandle)
  { } 

  virtual void Project(int x, int y, int w, int h);  // Must be call in ReshapeFunc: it sets proj.  
  virtual void Position();  // Must be call before rendering objects: it sets view matrix.
  virtual void Animate();   // Must be call before Camera::Position(): it animates the camera.

  // Computes the line which goes from camera origin to mouse coords at projection plane.
  glm::vec3 ComputeRayAt(float x_v, float y_v, float w, float h);
  glm::vec3 GetCenterCoordinates();
  
  void Scale(GLfloat d_sx, GLfloat d_sy, GLfloat d_sz);
  void Rotate(GLfloat d_rx, GLfloat d_ry, GLfloat d_rz);
  void Translate(GLfloat dx, GLfloat dy, GLfloat dz);
  OpenGLMatrix& GetViewMatrix() { return mViewMatrix; }
  OpenGLMatrix& GetProjMatrix() { return mProjMatrix; }

  void LookAt(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upVec);
  void SetPosition(GLfloat x, GLfloat y, GLfloat z);
  void SetRotation(GLfloat rx, GLfloat ry, GLfloat rz);
  
  void SetCameraType(CameraType type) { mType = type; }
  void SetFocusObject(SceneObject* focusObject) { mFocusObject = focusObject; }

  CameraType   GetCameraType()  { return mType;        }
  SceneObject* GetFocusObject() { return mFocusObject; }

  void SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, GLuint programHandle);


private:
  // View parameters
  glm::vec3 mPos {   0.0f, 0.0f, 15.0f};      // Center coordinates.
  glm::vec3 mRot {   0.0f, 0.0f,  0.0f};      // Rotation angles.
  glm::vec3 mDir {   0.0f, 0.0f, -1.0f};      // Direction.
  glm::vec3 mUpVec { 0.0f, 1.0f,  0.0f};      // Up vector.
  glm::vec3 mScale { 1.0f, 1.0f,  1.0f};      // Scales.
  SceneObject* mFocusObject { nullptr };      // The object at which the camera is looking.
  CameraType mType { EDITOR  };   // Type of camera.

  // Projection parameters
  GLfloat mFovy   {M_PI/3.0f};
  GLfloat mAspect {1280.0f/720.0f};
  GLfloat mFarZ   {2000.0f};
  GLfloat mNearZ  { 0.5f}; 

  // Matrices.
  OpenGLMatrix mViewMatrix;
  OpenGLMatrix mProjMatrix;

  // Shaders handle and program
  BasicPipelineProgram *mPipelineProgram {nullptr};
  GLuint mProgramHandle                  {0};
};

inline
void Camera::LookAt(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upVec)
{
  mPos = pos + upVec * 1.25f;
  mDir = dir;
  mUpVec = upVec;
}

inline 
void Camera::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
  mPos[0] = x;
  mPos[1] = y;
  mPos[2] = z;
}
inline 
void Camera::SetRotation(GLfloat rx, GLfloat ry, GLfloat rz)
{
  mRot[0] = rx;
  mRot[1] = ry;
  mRot[2] = rz;
}
inline 
void Camera::SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, GLuint programHandle) 
{ 
  mProgramHandle   = programHandle; 
  mPipelineProgram = pipelineProgram;
}

}  // namespace gloo.