/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/
#pragma once

#include "scene_object.h"

/*********************************************************
* This file provides classes for rendering primitive objs
* such as axis, squares, mirrors, spheres/domes, terrain.
* 
* The meshes contain the geometry by itself (vertex info)
* and the inherit from class Mesh.
*
* The corresponding objects store not only the mesh, but
* also material, texture and motion attributes.
* 
* EVERYTHING here was made FROM SCRATCH and took several
* hours!
*
* Rodrigo Castiel, 02/23/2016.
*********************************************************/

// +----------------------------------------------------------+
// | Table of useful objects  | Lighting | Texture | Material |
// | 1. class AxisObject      |    off   |   not   |    no    |
// | 2. -----------------     |    ---   |   ---   |    --    |
// | 3. class GridObject      |    off   |   no    |    no    |
// | 4. class TexturedSphere  |    opt   |   yes   |    no    |
// | 5. class TexturedTerrain |    opt   |   yes   |    no    |
// +----------------------------------------------------------+
// Constructors' default is Object(pipelineProgram, programHandle)

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace gloo
{

class AxisObject : public SceneObject
{
public:
  AxisObject(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : SceneObject(pipelineProgram, programHandle) 
  { }

  void Load();

  virtual ~AxisObject() { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class GridObject : public SceneObject
{
public:
  GridObject(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : SceneObject(pipelineProgram, programHandle) 
  { }

  void Load(int w = 11, int h = 11);

  virtual ~GridObject() { }

private:
  int mWidth  { 11 };
  int mHeight { 11 };

};

class TexturedSphere : public SceneObject
{
public:
  TexturedSphere(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : SceneObject(pipelineProgram, programHandle) 
  { }

  void Load(const std::string& fileName, bool completeDome = true, int detailLevel = 32);

  virtual ~TexturedSphere()
  {
    delete mTexture;
    delete mMaterial;
  }
private:
  int mDetailLevel;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

class TexturedTerrain : public SceneObject
{
public:
  TexturedTerrain(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
  : SceneObject(pipelineProgram, programHandle)
  { }

  void Load(const std::string& heightmapFileName, const std::string& textureFileName, 
    int w = 21, int h = 21);

  virtual ~TexturedTerrain()
  {
    delete mTexture;
    delete mMaterial;
  }

private:
  int mWidth  { 21 };
  int mHeight { 21 };

};

}  // namespace gloo.