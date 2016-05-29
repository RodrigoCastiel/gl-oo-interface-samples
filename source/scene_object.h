/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#pragma once

#include <iostream>
#include <string>

#include "imageIO.h"
#include "mesh.h"

namespace gloo
{

struct Texture;
struct Material;

/////////////////////////////////////////////////////////////////////////////////////////////////

struct Material
{
  glm::vec3 mKa;  // Ambient component.
  glm::vec3 mKd;  // Diffuse component.
  glm::vec3 mKs;  // Specular component.

  Material(const glm::vec3& Ka, const glm::vec3& Kd, const glm::vec3& Ks)
  : mKa(Ka), mKd(Kd), mKs(Ks)
  {  }

  Material(glm::vec3&& Ka, glm::vec3&& Kd, glm::vec3&& Ks)
  : mKa(Ka), mKd(Kd), mKs(Ks)
  {  }

  void Bind(GLuint programHandle);
};  // Material

struct Texture
{
  GLuint mBuffer { 0 }; // Texture buffer object id.  

  inline void Bind(GLuint programHandle) { glBindTexture(GL_TEXTURE_2D, mBuffer); }
  inline bool Valid() { return (mBuffer != 0); }

  void Load(int width, int height);
  void Load(ImageIO* source);
  bool Load(const std::string& fileName);

}; // Texture.

///////////////////////////////////////////////////////////////////////////////////////////////////

class SceneObject
{
public:
  // Constructor.
  SceneObject(BasicPipelineProgram* pipelineProgram, GLuint programHandle) 
  : mPipelineProgram(pipelineProgram), mProgramHandle(programHandle)
  { 
    mModelMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
  }

  // Render method - must be called in Scene::Render().
  virtual void Render() const;
  virtual void Animate();

  // Load method - must be called after constructor to initialize everything;
  virtual void Load() { }

  // Query methods.
  inline bool IsInitialized() const { return (mMesh != nullptr);     }
  inline bool HasMaterial()   const { return (mMaterial != nullptr); }
  inline bool HasTexture()    const { return (mTexture  != nullptr); }

  virtual bool IntersectRay(const glm::vec3& ray, const glm::vec3& C) const;

  // Getter and setters.
  void SetMeshOwner(bool isOwner) { mIsMeshOwner = isOwner; }
  void SetPosition(GLfloat x, GLfloat y, GLfloat z);
  void SetRotation(GLfloat rx, GLfloat ry, GLfloat rz);
  void SetLinVelocity(GLfloat dx, GLfloat dy, GLfloat dz);
  void SetRotVelocity(GLfloat drx, GLfloat dry, GLfloat drz);

  void SetScale(GLfloat sx, GLfloat sy, GLfloat sz);
  void SetLighting(bool state) { mUsingLighting = state; };
  inline virtual void SetMesh(Mesh* mesh) { mMesh = mesh; }
  inline virtual void SetTexture(Texture* texture)    { mTexture  = texture;  }
  inline virtual void SetMaterial(Material* material) { mMaterial = material; }

  inline virtual Mesh* GetMesh() { return mMesh; }
  inline OpenGLMatrix& GetModelMatrix() { return mModelMatrix; }

  inline glm::vec3& GetPosition() { return mPos; }
  inline glm::vec3& GetRotation() { return mRot; }

  void SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, GLuint programHandle);

  virtual ~SceneObject();

protected:
  // SceneObject does not own material, texture and mesh pointers.
  Material* mMaterial { nullptr };
  Texture*  mTexture  { nullptr };
  Mesh* mMesh { nullptr };

  BasicPipelineProgram* mPipelineProgram { nullptr };
  GLuint mProgramHandle { 0 };

  bool mIsMeshOwner   { false };
  bool mUsingLighting { false };

  mutable OpenGLMatrix mModelMatrix;  // Changes everytime.

  glm::vec3 mPos    {0.0f, 0.0f, 0.0f};    // Center position.
  glm::vec3 mRot    {0.0f, 0.0f, 0.0f};    // Rotations.
  glm::vec3 mScale  {1.0f, 1.0f, 1.0f};    // Scaling parameters.

  glm::vec3 mLinVelocity {0.0f, 0.0f, 0.0f};  // Linear velocity.
  glm::vec3 mRotVelocity {0.0f, 0.0f, 0.0f};  // Rotational velocity.
  
};  // class SceneObject

///////////////////////////////////////////////////////////////////////////////////////////////////

inline
void SceneObject::SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, 
  GLuint programHandle) 
{ 
  mProgramHandle   = programHandle; 
  mPipelineProgram = pipelineProgram;
}

inline
void SceneObject::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
  mPos[0] = x;
  mPos[1] = y;
  mPos[2] = z;
}

inline
void SceneObject::SetRotation(GLfloat rx, GLfloat ry, GLfloat rz)
{
  mRot[0] = rx;
  mRot[1] = ry;
  mRot[2] = rz;
}

inline 
void SceneObject::SetLinVelocity(GLfloat dx, GLfloat dy, GLfloat dz)
{
  mLinVelocity[0] = dx;
  mLinVelocity[1] = dy;
  mLinVelocity[2] = dz;
}

inline
void SceneObject::SetRotVelocity(GLfloat drx, GLfloat dry, GLfloat drz)
{
  mRotVelocity[0] = drx;
  mRotVelocity[1] = dry;
  mRotVelocity[2] = drz;
}

inline
void SceneObject::SceneObject::SetScale(GLfloat sx, GLfloat sy, GLfloat sz)
{
  mScale[0] = sx;
  mScale[1] = sy;
  mScale[2] = sz;
}

}  // namespace gloo

///////////////////////////////////////////////////////////////////////////////////////////////////
