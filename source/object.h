/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#pragma once

#include <vector>
#include <string>
#include <functional>

#include "imageIO.h"
#include "mesh.h"

namespace gloo
{
namespace obj
{

// ================== Material ================================================================= //

struct Material
{
  glm::vec3 Ka;  // Ambient component.
  glm::vec3 Kd;  // Diffuse component.
  glm::vec3 Ks;  // Specular component.
  GLfloat shininess { 1 };  // Shininess coefficient for specular lighting.

  Material(const glm::vec3& Ka_, const glm::vec3& Kd_, const glm::vec3& Ks_, 
    float shininess_ = 1.0f)

  : Ka(Ka_), Kd(Kd_), Ks(Ks_), shininess(shininess_)
  {  }

  Material(glm::vec3&& Ka_, glm::vec3&& Kd_, glm::vec3&& Ks_,
    float shininess_ = 1.0f)
  : Ka(Ka_), Kd(Kd_), Ks(Ks_), shininess(shininess_)
  {  }

  ~Material() 
  { 
    // TODO: destruct future textures; 
  }

};  // Material

// ================== Object ========================================== //
//
// class Object implements a 3D model typically loaded from a .obj file.
//
// It contains groups - set of vertices with same material and 
// a library of materials - loaded from .mtl files.
// Each group contains a material index and also a Mesh; 
//
// class Object is designed so that the number of cache misses is 
// mitigated when accessing the geometry. 
//
// ==================================================================== //

class Object
{
public:

  struct Group  // Specifies per-group information.
  {
    Group(Mesh* mesh_, int materialIndex_, std::string&& name_)
    : mesh(mesh_), materialIndex(materialIndex_), name(name_)
    { }

    Mesh* mesh;         // Geometry data.
    std::string name;   // Group name - if written in the .obj file.
    int materialIndex;  // Index for the list of materials.
  };

  // 
  Object(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : mPipelineProgram(pipelineProgram), mProgramHandle(programHandle)
  { 
    mModelMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
  }
  // Render method.
  void Render() const;

  // Improved .obj loading method.
  bool LoadObjFile(const std::string& objFilePath, bool smoothNormals);

  // ASSIMP loading method - works with any kind of 3d model file.
  bool LoadFile(const std::string& filePath, bool smoothNormals);

  // WIREFRAME - variable color according to rgb lambda function.
  bool LoadParametricSurf(std::function<glm::vec3 (float, float)> surf, 
                          std::function<glm::vec3 (float, float)> rgbFunc, 
                          int numSampleU, int numSampleV, bool solid = false);

  // Loads its mesh from a parametric surface, surf : R^2 -> R^3.
  // Parameterization must be normalized from 0 to 1.
  // numSampleU and numSampleV detail how many samples of u, v are taken.
  // SOLID.
  // bool LoadParametricSurf(std::function<glm::vec3 (float, float)> surf, 
  //   int numSampleU, int numSampleV, );

  // TODO: Add primitive loading method here.

  // Computes intersection of C + t*Ray with geometry.
  bool RayIntersection(const glm::vec3& ray, const glm::vec3& C) const;       // TODO.
  bool FastRayIntersection(const glm::vec3& ray, const glm::vec3& C) const;   // TODO.

  // Getter and setters.
  void SetDataOwner(bool isOwner) { mOwnsData = isOwner; }
  void SetLighting(bool state) { mUsingLighting = state; };

  void SetPosition(GLfloat x, GLfloat y, GLfloat z);
  void SetRotation(GLfloat rx, GLfloat ry, GLfloat rz);
  void SetScale(GLfloat sx, GLfloat sy, GLfloat sz);

  void SetPosition(const glm::vec3& pos);
  void SetRotation(const glm::vec3& rot);
  void SetScale(const glm::vec3& scale);

  inline OpenGLMatrix& GetModelMatrix() { return mModelMatrix; }
  inline glm::vec3& GetPosition() { return mPos; }
  inline glm::vec3& GetRotation() { return mRot; }

  void SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, GLuint programHandle);

  ~Object();

private:
  void BuildUpGroup(std::vector<GLfloat>& groupPositions, 
                    std::vector<GLfloat>& groupTexCoords, 
                    std::vector<GLfloat>& groupNormals,
                    std::vector<GLuint>& groupIndices,
                    const char* name, int materialIndex = -1);

  BasicPipelineProgram* mPipelineProgram { nullptr };
  GLuint mProgramHandle { 0 };

  // Stores all geometry information per group - each one has a mesh and a material.
  std::vector<Group> mGroups;        // List of groups that share the same material.
  std::vector<Material> mMaterials;  // Material library.

  bool mOwnsData      { true };       // Tells whether the object has the original data (for copies).
  bool mUsingLighting { true };       // Tells if the object is using Phong shading model.
  mutable OpenGLMatrix mModelMatrix;  // It is an auxiliar object to handle transforms.

  glm::vec3 mPos    {0.0f, 0.0f, 0.0f};    // Center    (x, y, z).
  glm::vec3 mRot    {0.0f, 0.0f, 0.0f};    // Rotations (x, y, z).
  glm::vec3 mScale  {1.0f, 1.0f, 1.0f};    // Scaling parameters.
  
};  // class Object

// ============================================================================================= //

inline
void Object::SetPipelineProgramParam(BasicPipelineProgram *pipelineProgram, 
  GLuint programHandle) 
{ 
  mProgramHandle   = programHandle; 
  mPipelineProgram = pipelineProgram;
}

inline
void Object::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
  mPos[0] = x;
  mPos[1] = y;
  mPos[2] = z;
}

inline
void Object::SetRotation(GLfloat rx, GLfloat ry, GLfloat rz)
{
  mRot[0] = rx;
  mRot[1] = ry;
  mRot[2] = rz;
}

inline
void Object::SetScale(GLfloat sx, GLfloat sy, GLfloat sz)
{
  mScale[0] = sx;
  mScale[1] = sy;
  mScale[2] = sz;
}


inline
void Object::SetPosition(const glm::vec3& pos)
{
  mPos = pos;
}

inline
void Object::SetRotation(const glm::vec3& rot)
{
  mRot = rot;
}

inline
void Object::SetScale(const glm::vec3& scale)
{
  mScale = scale;
}

}  // namespace obj
}  // namespace gloo

// ============================================================================================= //
