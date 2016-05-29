#include "mesh.h"

#include <fstream>
#include <sstream>
#include <cstdio>

namespace gloo
{

void Mesh::Render() const
{
  if (IsInitialized()) 
  {
    glBindVertexArray(mVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEab);

    glDrawElements(
     mDrawMode,         // mode.
     mNumIndices,       // number of vertices.
     GL_UNSIGNED_INT,   // type.
     (void*)0           // element array buffer offset.
    );
  }
}

bool Mesh::Load(const GLfloat* positions,
                const GLfloat* colors, 
                const GLfloat* normals,
                const GLfloat* texCoords,
                const GLuint* indices,
                int numVertices, 
                int numIndices,
                GLenum drawMode, 
                StorageType storageType
              ) 
{ 
  if (!positions || numVertices <= 0)
  {
    return false;
  }

  mDrawMode = drawMode;
  mStorageType = storageType;
  mNumVertices = numVertices;
  mNumIndices  = (numIndices <= 0 || indices == nullptr) ? numVertices : numIndices;

  mHasColors   = (colors    != nullptr);
  mHasNormals  = (normals   != nullptr);
  mHasTexCoord = (texCoords != nullptr);
  mVertexSize  = 3 + 3*mHasColors + 3*mHasNormals + 2*mHasTexCoord;

  mVertices = new GLfloat [mVertexSize * mNumVertices];
  mIndices  = new GLuint  [mNumIndices];

  if (mStorageType == kTightlyPacked)
  {
    // Initialize vertices buffer array. 
    for (int i = 0; i < mNumVertices; i++)
    {
      float* position = PositionAt(i);
      float* texCoord = TexCoordAt(i);
      float* normal   = NormalAt(i); 
      float* color    = ColorAt(i);

      memcpy(position, positions + 3*i, sizeof(GLfloat)*3);
      if (HasColors())
      {
        memcpy(color, colors + 3*i, sizeof(GLfloat)*3);
      }
      if (HasNormals())
      {
        memcpy(normal, normals + 3*i, sizeof(GLfloat)*3);
      }
      if (HasTexCoord())
      {
        memcpy(texCoord, texCoords + 2*i, sizeof(GLfloat)*2);
      }
    }
  }
  else
  {
    GLfloat* dest = mVertices;
    memcpy(dest, positions, 3*sizeof(GLfloat)*mNumVertices);
    dest += 3*mNumVertices;
    if (HasColors())
    {
      memcpy(dest, colors, 3*sizeof(GLfloat)*mNumVertices);
      dest += 3*mNumVertices;
    }
    if (HasNormals())
    {
      memcpy(dest, normals, 3*sizeof(GLfloat)*mNumVertices);
      dest += 3*mNumVertices;
    }
    if (HasTexCoord())
    {
      memcpy(dest, texCoords, 2*sizeof(GLfloat)*mNumVertices);
      dest += 2*mNumVertices;
    }
  }

  // Initialize element array (indices array).
  if (indices)  // Element array provided.
  {
    memcpy(mIndices, indices, sizeof(GLuint)*mNumIndices);
  }
  else  // Element array wasn't provided -- build it up.
  {
    for (int i = 0; i < mNumIndices; i++)
    {
      mIndices[i] = i;
    }
  }

  mInitialized = true;
  Mesh::Upload();
  return true;
}

bool Mesh::Load(const GLfloat* vertices, const GLuint* indices, 
                int numVertices,         int numIndices, 
                bool hasColors,          bool hasNormals, 
                bool hasTexCoord,        GLenum drawMode
              )
{
  // Validate input - vertices must be non-null.
  if (!vertices || numVertices <= 0)
  {
    return false;
  }

  mDrawMode = drawMode;
  mNumVertices = numVertices;
  mNumIndices  = (numIndices <= 0 || indices == nullptr) ? numVertices : numIndices;

  mHasColors   = hasColors;
  mHasNormals  = hasNormals;
  mHasTexCoord = hasTexCoord;
  mVertexSize  = 3 + 3*hasColors + 3*hasNormals + 2*hasTexCoord;

  mVertices = new GLfloat [mVertexSize * mNumVertices];
  mIndices  = new GLuint  [mNumIndices];

  // Initialize vertices buffer array. 
  memcpy(mVertices, vertices, sizeof(GLfloat)*mVertexSize*mNumVertices);

  // Initialize element array (indices array).
  if (indices)  // Element array provided.
  {
    memcpy(mIndices, indices, sizeof(GLuint)*mNumIndices);
  }
  else  // Element array wasn't provided -- build it up default elements.
  {
    for (int i = 0; i < mNumIndices; i++)
    {
      mIndices[i] = i;
    }
  }

  mInitialized = true;
  Mesh::Upload();
  return true;
}

// Reloads the geometry. To keep some data constant, just specify nullptr.
void Mesh::Reload(const GLfloat* positions,
                  const GLfloat* colors,   
                  const GLfloat* normals,  
                  const GLfloat* texCoords 
                )
{
  if (!mInitialized)
  {
    std::cerr << "ERROR The mesh must be initialized before creating buffer objects.\n";
    return;
  }

  if (mStorageType == kTightlyPacked)
  {
    for (int i = 0; i < mNumVertices; i++)
    {
      float* position = PositionAt(i);
      float* texCoord = TexCoordAt(i);
      float* normal   = NormalAt(i); 
      float* color    = ColorAt(i);

      if (positions)
      {
        memcpy(position, positions + 3*i, sizeof(GLfloat)*3);
      }
      if (colors && HasColors())
      {
        memcpy(color, colors + 3*i, sizeof(GLfloat)*3);
      }
      if (normals && HasNormals())
      {
        memcpy(normal, normals + 3*i, sizeof(GLfloat)*3);
      }
      if (texCoords && HasTexCoord())
      {
        memcpy(texCoord, texCoords + 2*i, sizeof(GLfloat)*2);
      }
    }
  }
  else  // Sub-buffered.
  {
    GLfloat* dest = mVertices;
    if (positions)
    {
      memcpy(dest, positions, 3*sizeof(GLfloat)*mNumVertices);
      dest += 3*mNumVertices;
    }
    if (colors && HasColors())
    {
      memcpy(dest, colors, 3*sizeof(GLfloat)*mNumVertices);
      dest += 3*mNumVertices;
    }
    if (normals && HasNormals())
    {
      memcpy(dest, normals, 3*sizeof(GLfloat)*mNumVertices);
      dest += 3*mNumVertices;
    }
    if (texCoords && HasTexCoord())
    {
      memcpy(dest, texCoords, 2*sizeof(GLfloat)*mNumVertices);
      dest += 2*mNumVertices;
    }
  }

  Mesh::Upload();
}

// Reloads geometry from tightly packed GLfloat array.
void Mesh::Reload(const GLfloat* vertices)
{
  if (vertices)
  {
    memcpy(mVertices, vertices, sizeof(GLfloat) * mVertexSize * mNumVertices);
    Mesh::Upload();
  }
}

void Mesh::Upload()
{
  if (mProgramHandle == 0)
  {
    std::cerr << "ERROR Program Handle must be set before creating buffer objects.\n";
    return;
  }

  if (!mInitialized)
  {
    std::cerr << "ERROR The mesh must be initialized before creating buffer objects.\n";
    return;
  }

  // Specify how the arguments will be passed to shaders.
  GLuint locTexCoordAttrib = glGetAttribLocation(mProgramHandle, "in_tex_coord");
  GLuint locPositionAttrib = glGetAttribLocation(mProgramHandle, "in_position");
  GLuint locNormalAttrib   = glGetAttribLocation(mProgramHandle, "in_normal");
  GLuint locColorAttrib    = glGetAttribLocation(mProgramHandle, "in_color");

  // Generate Buffers.
  glGenVertexArrays(1, &mVao);
  glGenBuffers(1, &mVbo);
  glGenBuffers(1, &mEab);

  // Specify VAO.
  glBindVertexArray(mVao);
  
  // Upload indices to GPU.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEab);  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndices, GL_STATIC_DRAW);

  // Enable/Disable each vertex attribute.
  glEnableVertexAttribArray(locPositionAttrib);
  if (HasColors())
  {
    glEnableVertexAttribArray(locColorAttrib);
  }
  else
  {
    glDisableVertexAttribArray(locColorAttrib);
  }
  
  if (HasNormals())
  {
    glEnableVertexAttribArray(locNormalAttrib);
  }
  else
  {
    glDisableVertexAttribArray(locNormalAttrib);
  }

  if (HasTexCoord())
  { 
    glEnableVertexAttribArray(locTexCoordAttrib);
  }
  else
  {
    glDisableVertexAttribArray(locTexCoordAttrib);
  }

  // Upload vertices to GPU.
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(GL_ARRAY_BUFFER, mVertexSize * mNumVertices * sizeof(GLfloat), mVertices, GL_STATIC_DRAW);

  if (mStorageType == kTightlyPacked)
  {
    GLfloat stride = sizeof(GLfloat) * mVertexSize;
    glVertexAttribPointer(locPositionAttrib, 3, GL_FLOAT, GL_FALSE, stride, 0);
    
    glVertexAttribPointer(locColorAttrib, 3, GL_FLOAT, GL_FALSE, stride, 
      (void*)(sizeof(GLfloat) * 3));

    glVertexAttribPointer(locNormalAttrib, 3, GL_FLOAT, GL_FALSE, stride, 
      (void*)(sizeof(GLfloat) * (3 + 3*mHasColors)));

    glVertexAttribPointer(locTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, stride, 
      (void*)(sizeof(GLfloat) * (3 + 3*mHasColors + 3*mHasNormals)) );
  }
  else   // Sub buffered storage type.
  {
    glVertexAttribPointer(locPositionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribPointer(locColorAttrib,    3, GL_FLOAT, GL_FALSE, 0, 
      (void*)(sizeof(GLfloat) * 3*mNumVertices));
    
    glVertexAttribPointer(locNormalAttrib,   3, GL_FLOAT, GL_FALSE, 0, 
      (void*)(sizeof(GLfloat) * (3 + 3*mHasColors)*mNumVertices));

    glVertexAttribPointer(locTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0,   
      (void*)(sizeof(GLfloat) * (3 + 3*mHasColors + 3*mHasNormals)*mNumVertices));
  }
}

// ============================================================================================= //

void Mesh::Preallocate(int numVertices, int numIndices, bool hasColors, 
                       bool hasNormals, bool hasTexCoord)
{
  mNumVertices = numVertices;
  mNumIndices  = numIndices;

  mHasColors   = hasColors;
  mHasNormals  = hasNormals;
  mHasTexCoord = hasTexCoord;

  mVertexSize = (3 + 3*hasColors + 3*hasNormals + 2*hasTexCoord);

  mVertices = new GLfloat [mVertexSize * mNumVertices];
  mIndices  = new GLuint  [mNumIndices];

  mInitialized = true;
}

void Mesh::Update()
{
  // Update vertices to GPU.
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(GL_ARRAY_BUFFER, mVertexSize * mNumVertices * sizeof(GLfloat), mVertices, GL_STATIC_DRAW);
}

// ============================================================================================= //

Mesh::~Mesh()
{
  if (mInitialized)
  {
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
    glDeleteBuffers(1, &mEab);
  }

  delete [] mVertices;
  delete [] mIndices;
}

} // namespace gloo.