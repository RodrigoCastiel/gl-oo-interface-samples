/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#pragma once

#include <cmath>

#include <vector>
#include <string>
#include <iostream>

#include "openGLHeader.h"
#include "glutHeader.h"

#include "openGLMatrix.h"
#include "basicPipelineProgram.h"


//  +-------------------------------------------------+
//  |  This class implements a generic 3d mesh to     |
//  |  be rendered using OpenGL core profile.         |
//  |  Its vertices are tightly packed:               |
//  |  [(x y z) (r g b) (nx ny nz) (tx ty)]           |
//  |  This is not related to materials or texture    |
//  |                                                 |
//  |  It is optimized by storing only what it uses.  |
//  |  That is, if it doesn't have texture, the tex-  |
//  |  ture coordinates aren't allocated. The same    |
//  |  with colors and normals.                       |
//  |  It also uses element arrays and the draw mode  |
//  |  is configurable.                               |
//  +-------------------------------------------------+
   

namespace gloo 
{

class Mesh
{
public:
  enum StorageType  // Tells how vertices are stored in GPU.
  {
    kTightlyPacked,   // All per-vertex data is stored together (i.e. array of vertices).
    kSubBuffered,     // Each attribute is store in a different sub buffer (i.e. arrays 
                      // of pos, rgb, uv, ...).
  };

  // Constructors.
  Mesh() { }
  Mesh(GLuint programHandle) : mProgramHandle(programHandle) { }

  // TODO: COMMENT!!
  void Render() const;   // Renders the geometry at the current origin.

  // Loads from different buffers - not provided data array must be set as nullptr.
  // positions must be non-null. 
  // indices can be nullptr - in this case, default elements are used (0, 1, 2, ...).
  bool Load(const GLfloat* positions, // Format: { (x, y, z) }
            const GLfloat* colors,    // Format: { (r, g, b) }
            const GLfloat* normals,   // Format: { (nx, ny, nz) }
            const GLfloat* texCoords, // Format: { (u, v) }
            const GLuint* indices,    // Format: { i0, i1, i2, ... }.
            int numVertices,          // Number of vertices.
            int numIndices,           // Number of indices/elements.
            GLenum drawMode         = GL_TRIANGLES,   // GL_LINES, GL_LINE_STRIP, GL_POINTS, GL_TRIANGLES, ...
            StorageType storageType = kTightlyPacked  // Tells the storage type in buffer. See enum StorageType.
          );

  // Loads from tightly packed GLfloat array.
  bool Load(const GLfloat* vertices,  // Format: { (x,y,z) [r, g, b] [nx, ny, nz], [u, v] } per vertex.
            const GLuint* indices,    // Example: { 0, 1, 4, 3, ... }. Indices start from 0.
            int numVertices,          // Number of vertices.
            int numIndices,           // Number of indices/elements.
            bool hasColors,           // Tells if per-vertex color information is being provided.
            bool hasNormals,          // Tells if per-vertex normal information is being provided.
            bool hasTexCoord,         // Tells if per-vertex uv coordinates are being provided. 
            GLenum drawMode = GL_TRIANGLES  // GL_LINES, GL_LINE_STRIP, GL_POINTS, GL_TRIANGLES, ...
          );

  // Reloads the geometry. To keep some data constant, just specify nullptr.
  // Use for updating positions or colors, or normals, or texCoords.
  // It copies as many bytes as specified by Load/Preallocate functions.
  // It doesn't allow modifying new vertices or any attribute not specified.
  void Reload(const GLfloat* positions, // Format: { (x, y, z) }
              const GLfloat* colors,    // Format: { (r, g, b) }
              const GLfloat* normals,   // Format: { (nx, ny, nz) }
              const GLfloat* texCoords  // Format: { (u, v) }
            );

  // Reloads geometry from tightly packed GLfloat array.
  void Reload(const GLfloat* vertices);  // Format: { (x,y,z) [r, g, b] [nx, ny, nz], [u, v] } per vertex.

  // Preallocates the tightly packed geometry arrays. It is meant to be used with generic shapes.
  // You may want to use PositionAt, ColorAt, NormalAt, TexCoordAt and IndexAt to edit the geometry.
  // After calling preallocate and initializing vertices, you must call Upload to send the buffers to GPU.
  void Preallocate(int numVertices, int numIndices, bool hasColors, bool hasNormals, bool hasTexCoord);

  void Upload();  // Sends the geometry to the graphics card.
  void Update();  // Resends the geometry again to the graphics card.

  // Query methods.
  inline bool IsInitialized() const { return mInitialized; }
  inline bool HasTexCoord()   const { return mHasTexCoord; }
  inline bool HasNormals()    const { return mHasNormals;  }
  inline bool HasColors()     const { return mHasColors;   }

  // Getter and setters.
  inline StorageType GetStorageType() const { return mStorageType; }
  inline int GetNumVertices() const { return mNumVertices; }
  inline int GetNumIndices()  const { return mNumIndices;  }
  inline int GetVertexSize()  const { return mVertexSize;  } 

  inline void SetDrawMode(GLenum mode) { mDrawMode = mode; };
  inline void SetProgramHandle(GLuint programHandle) { mProgramHandle = programHandle; }

  // Geometry access: don't attempt to access if the corresponding query methods return false.
  // NOTE: Tightly packed vertices access.
  GLfloat* PositionAt(int index);   // Address to vertex[index] 3D position.
  GLfloat* ColorAt(int index);      // Address to vertex[index] RGB color.
  GLfloat* NormalAt(int index);     // Address to vertex[index] 3D normal vector.
  GLfloat* TexCoordAt(int index);   // Address to vertex[index] 2D texture coordinates.

  // NOTE: Sub-buffered vertices access.
  GLfloat* SBPositionAt(int index);   // Address to vertex[index] 3D position.
  GLfloat* SBColorAt(int index);      // Address to vertex[index] RGB color.
  GLfloat* SBNormalAt(int index);     // Address to vertex[index] 3D normal vector.
  GLfloat* SBTexCoordAt(int index);   // Address to vertex[index] 2D texture coordinates.

  GLuint*  IndexAt(int index);      // Address to element_array[index].

  // Destructor.
  ~Mesh();

protected:
  // Provides a link to the shaders.
  GLuint mProgramHandle;

  // True if it has any geometry loaded in it.
  bool mInitialized { false };

  // Tells geometry information.
  bool mHasTexCoord { false };
  bool mHasNormals  { false };
  bool mHasColors   { false };

  // The vertices, normals, texture and colors array and element array.
  GLfloat* mVertices { nullptr };
  GLuint*  mIndices  { nullptr };
  int mNumVertices { 0 };
  int mNumIndices  { 0 };
  int mVertexSize  { 0 };

  // OpenGL Buffers parameters.
  StorageType mStorageType { kTightlyPacked };
  GLenum mDrawMode { GL_TRIANGLES };
  GLuint mEab { 0 };
  GLuint mVao { 0 };  
  GLuint mVbo { 0 };
}; // Mesh.

/* Tightly packed access to vertices array */

inline 
GLfloat* Mesh::PositionAt(int index)
{
  return &mVertices[mVertexSize * index];
}

inline 
GLfloat* Mesh::ColorAt(int index)
{
  return &mVertices[mVertexSize * index + 3];
}

inline 
GLfloat* Mesh::NormalAt(int index)
{
  return &mVertices[mVertexSize*index + (3 + 3*mHasColors)];
}

inline 
GLfloat* Mesh::TexCoordAt(int index)
{
  return &mVertices[mVertexSize*index + (3 + 3*mHasColors + 3*mHasNormals)];
}

inline
GLuint* Mesh::IndexAt(int index)
{
  return &mIndices[index];
}

/* Sub-buffered access to vertices array */

inline 
GLfloat* Mesh::SBPositionAt(int index)
{
  return &mVertices[3*index];
}

inline 
GLfloat* Mesh::SBColorAt(int index)
{
  return &mVertices[3*mNumVertices + 3*index];
}

inline 
GLfloat* Mesh::SBNormalAt(int index)
{
  return &mVertices[3*mNumVertices + 3*mNumVertices*mHasColors + 3*index];
}

inline 
GLfloat* Mesh::SBTexCoordAt(int index)
{
  return &mVertices[3*mNumVertices*(1 + mHasColors + mHasTexCoord) + 2*index];
}

} // namespace gloo.
