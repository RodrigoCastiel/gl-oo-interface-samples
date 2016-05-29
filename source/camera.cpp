#include "camera.h"

#include "scene_object.h"

namespace gloo
{

void Camera::Scale(GLfloat d_sx, GLfloat d_sy, GLfloat d_sz) 
{
  mScale[0] *= (1.0f + d_sx);
  mScale[1] *= (1.0f + d_sy);
  mScale[2] *= (1.0f + d_sz);
}

void Camera::Rotate(GLfloat d_rx, GLfloat d_ry, GLfloat d_rz) 
{
  mRot[0] += d_rx;
  mRot[1] += d_ry;
  mRot[2] += d_rz;
}

void Camera::Translate(GLfloat dx, GLfloat dy, GLfloat dz)
{
  mPos[0] += dx;
  mPos[1] += dy;
  mPos[2] += dz;
}

void Camera::Project(int x, int y, int w, int h)
{
  mAspect = (GLfloat) (w-x) / (GLfloat) (h-y);
  mProjMatrix.SetMatrixMode(OpenGLMatrix::Projection);
  mProjMatrix.LoadIdentity();
  mProjMatrix.Perspective(mFovy, mAspect, mNearZ, mFarZ);
  mPipelineProgram->SetProjectionMatrix(mProjMatrix);
}

void Camera::Position()
{
  mPipelineProgram->SetViewMatrix(mViewMatrix);
}

void Camera::Animate()
{
  mViewMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
  mViewMatrix.LoadIdentity();

  if (mType == EDITOR)
  {
    mViewMatrix.Translate(-mPos[0], -mPos[1], -mPos[2]);
    mViewMatrix.Rotate(-mRot[2], 0, 0, 1);
    mViewMatrix.Rotate(-mRot[0], 1, 0, 0);
    mViewMatrix.Rotate(-mRot[1], 0, 1, 0);
    mViewMatrix.Scale(mScale[0], mScale[1], mScale[2]);
  }
  else
  {
    mViewMatrix.LookAt(          mPos[0],           mPos[1],           mPos[2],
                       mPos[0] + mDir[0], mPos[1] + mDir[1], mPos[2] + mDir[2],
                              mUpVec[0],          mUpVec[1],         mUpVec[2]);
  }

  // If it is linked to some object, position it correctly.
  if (mFocusObject)  
  {
    glm::vec3& X = mFocusObject->GetPosition();
    glm::vec3& R = mFocusObject->GetRotation(); 
    
    mViewMatrix.Rotate(-R[2], 0, 0, 1);
    mViewMatrix.Rotate(-R[0], 1, 0, 0);
    mViewMatrix.Rotate(-R[1], 0, 1, 0);
    mViewMatrix.Translate(-X[0], -X[1], -X[2]);
  }
}

glm::vec3 Camera::ComputeRayAt(float x_v, float y_v, float w, float h) 
{
  // Compute boundaries of near clip plane.
  float x_max = 1.0f;
  float y_max = 1.0f;

  // Convert viewport coordinates to projection plane coordinates.
  y_v = (h - y_v);
  float xp = ((x_v / w) * (2*x_max)) - x_max;
  float yp = ((y_v / h) * (2*y_max)) - y_max; 

  const glm::mat4& V = mViewMatrix.GetGLMatrix();
  const glm::mat4& P = mProjMatrix.GetGLMatrix();

  glm::vec4 ray = glm::inverse(P*V) * glm::vec4(xp, yp, 1.0, 1.0);

  return glm::vec3(ray[0], ray[1], ray[2]);
}

glm::vec3 Camera::GetCenterCoordinates() 
{ 
  const glm::mat4& V = mViewMatrix.GetGLMatrix();
  glm::vec4 C = glm::inverse(V) * glm::vec4(0, 0, 0, 1);
  return glm::vec3(C[0], C[1], C[2]);
}

} // namespace gloo.