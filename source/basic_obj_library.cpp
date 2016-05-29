#include "basic_obj_library.h"


#define INDEX(a, b) (((w) * (b)) + a)

namespace gloo
{

void AxisObject::Load()
{
  GLfloat positions[] = { 0.0f, 0.0f, 0.0f,
                          1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f,
                          0.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f
                         };
  GLfloat colors[] = { 1.0f, 0.0f, 0.0f,
                       1.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f,
                       0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 1.0f,
                       0.0f, 0.0f, 1.0f
                     };

  mMesh = new Mesh();
  mMesh->SetProgramHandle(mProgramHandle);
  mMesh->Load(  positions, // positions
                colors,    // colors
                nullptr,   // normals
                nullptr,   // texcoord
                nullptr,   // indices,
                6,  // numVertices
                0,  // numIndices
                GL_LINES,             // draw mode
                Mesh::kSubBuffered    // storage type
                );


  SceneObject::SetScale(5.0f, 5.0f, 5.0f);
  mIsMeshOwner = true;
}

void GridObject::Load(int w, int h)
{
  mWidth  = w;
  mHeight = h;
  int numVertices = (w * h);
  int numIndices  = (2 * w * h);

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices; 

  vertices.reserve(numVertices * 6);
  indices.reserve(numIndices);

  // Initialize vertices.
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      // Positions x, y, z.
      vertices.push_back(static_cast<float>(x - w/2)/w);
      vertices.push_back(0.0);
      vertices.push_back(static_cast<float>(y - h/2)/h);

      // Colors RGB.
      vertices.push_back(0.45f);
      vertices.push_back(0.45f);
      vertices.push_back(0.45f);
    }
  }

  // Initialize indices.
  // Wire frame Element array - indices are written in a zig-zag pattern,
  // first horizontally and then vertically. It uses GL_LINE_STRIP. 
  int index = 0;
  int x = 0, y = 0;
  int dx = 1, dy = -1;

  for (y = 0; y < h; y++)  // Horizontally.
  {
    x = ((dx == 1) ? 0 : w-1);
    for (int k = 0; k < w; k++, x += dx)
      indices.push_back(INDEX(x, y));
    dx *= -1;
  }

  // Start from the last point to allow continuity in GL_LINE_STRIP.
  x = ((dx == 1) ? 0 : w-1);
  y = h-1;  
  dy = -1;
  for (int i = 0; i < w; i++, x += dx)  // Vertically.
  {
    y = ((dy == 1) ? 0 : h-1);
    for (int j = 0; j < h; j++, y += dy)
      indices.push_back(INDEX(x, y));
    dy *= -1;
  }

  mMesh = new Mesh();
  mMesh->SetProgramHandle(mProgramHandle);
  mMesh->Load(&vertices[0], &indices[0], numVertices, numIndices, true, false, false, GL_LINE_STRIP);
  SceneObject::SetScale(w, 1.0f, h);
  mIsMeshOwner = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void TexturedSphere::Load(const std::string& fileName, bool completeDome, int detailLevel)
{
  mDetailLevel = detailLevel;
  int n = mDetailLevel + 1;
  int h = mDetailLevel + 1;
  int numVertices = (n * h);
  int numIndices  = 2*(h-2)*n + 2*n + 2*(h-2);

  std::vector<GLfloat> positions;
  std::vector<GLfloat> texCoords;
  std::vector<GLfloat> normals;
  std::vector<GLuint> indices;

  positions.reserve(3 * numVertices);
  texCoords.reserve(2 * numVertices);
  normals.reserve(3 * numVertices);
  indices.reserve(numIndices);

  GLfloat position[3];

  // Initialize vertices.
  for (int v = 0; v < h; v++)
  {
    for (int u = 0; u < n; u++)
    {
      int index = (v*n + u);
      GLfloat theta_u = (2*M_PI * u) / (n-1);
      GLfloat theta_v = (M_PI * v) / (h-1);

      position[0] = cos(theta_u) * sin(theta_v);
      position[2] = sin(theta_u) * sin(theta_v);
      position[1] = cos(theta_v);

      // Positions.
      positions.push_back(position[0]);
      positions.push_back(position[1]);
      positions.push_back(position[2]);

      // Normals.
      normals.push_back(2*position[0]);
      normals.push_back(2*position[1]);
      normals.push_back(2*position[2]);
      
      // Texture UV.
      texCoords.push_back(1 - static_cast<float>(u) / (n-1));
      if (completeDome)
      {
        texCoords.push_back(1 - static_cast<float>(v) / (h-1));
      }
      else
      {
        texCoords.push_back(2 - 2*static_cast<float>(v) / (h-1));
      }
    }
  }

  // Initialize indices.
  for (int v = 0; v < h-1; v++)
  {
    // Zig-zag pattern: alternate between top and bottom.
    for (int u = 0; u < n; u++)
    {
      indices.push_back((v+0)*n + u);
      indices.push_back((v+1)*n + u);
    }

    // Triangle row transition: handle discontinuity.
    if (v < h-2)
    {
      // Repeat last vertex and the next row first vertex to generate 
      // two invalid triangles and get continuity in the mesh.
      indices.push_back((v+1)*n + (n-1)); //INDEX(this->width-1, y+1);
      indices.push_back((v+1)*n + 0);     //INDEX(0, y+1);
    }
  }

  mMesh = new Mesh();
  mMesh->SetProgramHandle(mProgramHandle);
  mMesh->Load(&positions[0], 
              nullptr, 
              &normals[0], 
              &texCoords[0], 
              &indices[0], 
              numVertices, 
              numIndices, 
              GL_TRIANGLE_STRIP, 
              Mesh::kSubBuffered
            );

  mIsMeshOwner = true;

  mTexture = new Texture();
  mTexture->Load(fileName);
  mMaterial = new Material( glm::vec3(0.18), 
                            glm::vec3(0.8), 
                            glm::vec3(0.02) );

  mUsingLighting = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void TexturedTerrain::Load(const std::string& heightmapFileName, const std::string& textureFileName, 
  int w, int h)
{
  ImageIO* source = new ImageIO();
  bool loaded = false;

  // Try to load the heightmap.
  if (source->loadJPEG(heightmapFileName.c_str()) == ImageIO::OK)
  {
    loaded = true;
  }

  mWidth  = w;
  mHeight = h;
  int numVertices = (w * h);
  int numIndices = 2*(h-2)*w + 2*w + 2*(h-2);
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  vertices.reserve((3 + 3 + 2)*numVertices);
  indices.reserve(numIndices);

  // Initialize vertices.
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      vertices.push_back(static_cast<float>(x - w/2)/w);
      vertices.push_back(loaded ? (source->getPixel(x, y, 0)/4.0f - 255/4) : 0.0f);
      vertices.push_back(static_cast<float>(y - h/2)/h);

      // TODO: compute normals if heightmap is loaded.
      vertices.push_back(0);
      vertices.push_back(1);
      vertices.push_back(0);

      vertices.push_back(static_cast<float>(x));
      vertices.push_back(static_cast<float>(y));
    }
  }

  // Initialize indices.
  for (int y = 0; y < h-1; y++)
  {
    // Zig-zag pattern: alternate between top and bottom.
    for (int x = 0; x < w; x++)
    {
      indices.push_back(INDEX(x, y));
      indices.push_back(INDEX(x, y+1));
    }

    // Triangle row transition: handle discontinuity.
    if (y < h-2)
    {
      // Repeat last vertex and the next row first vertex to generate 
      // two invalid triangles and get continuity in the mesh.
      indices.push_back(INDEX(w-1, y+1));
      indices.push_back(INDEX(0, y+1));
    }
  }


  mMesh = new Mesh();
  mMesh->SetProgramHandle(mProgramHandle);
  mMesh->Load(&vertices[0], &indices[0], numVertices, numIndices, false, true, true, GL_TRIANGLE_STRIP);
  mIsMeshOwner = true;

  mTexture = new Texture();
  mTexture->Load(textureFileName);
  mMaterial = new Material( glm::vec3(0.18), 
                            glm::vec3(0.8), 
                            glm::vec3(0.02) );

  mUsingLighting = true;
  delete source;
}

}  // namespace gloo.
