#include "object.h"
#include "utilities.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <fstream>

#include "assimp/Importer.hpp"      // 3D files importer.
#include "assimp/scene.h"           // Output data structure.
#include "assimp/postprocess.h"     // Post processing flags.

#define INDEX(a, b) (((w) * (b)) + a)

namespace gloo
{

namespace obj
{

// ================= Renderer ===================== //
void Object::Render() const
{
  glDisable(GL_TEXTURE_2D);
  GLuint lightOnLoc = glGetUniformLocation(mProgramHandle, "light_on");
  GLuint matLoc = glGetUniformLocation(mProgramHandle, "material_on");
  GLuint locKa  = glGetUniformLocation(mProgramHandle, "material.Ka");
  GLuint locKd  = glGetUniformLocation(mProgramHandle, "material.Kd");
  GLuint locKs  = glGetUniformLocation(mProgramHandle, "material.Ks");
  GLuint texLoc = glGetUniformLocation(mProgramHandle, "tex_on");

  glUniform1i(lightOnLoc, mUsingLighting);
  glUniform1i(matLoc, 1);
  glUniform3f(locKa, 0.1f, 0.1f, 0.1f);
  glUniform3f(locKd, 0.2f, 0.6f, 0.6f);
  glUniform3f(locKs, 0.01f, 0.01f, 0.01f);
  glUniform1i(texLoc, 0);

  mModelMatrix.LoadIdentity();
  mModelMatrix.Translate(mPos[0], mPos[1], mPos[2]);
  mModelMatrix.Rotate(mRot[2], 0, 0, 1);
  mModelMatrix.Rotate(mRot[0], 1, 0, 0);
  mModelMatrix.Rotate(mRot[1], 0, 1, 0);
  mModelMatrix.Scale(mScale[0], mScale[1], mScale[2]);

  mPipelineProgram->SetModelMatrix(mModelMatrix);

  for (auto& group : mGroups)
  {
    // TODO: set material per group.
    group.mesh->Render();
  }
}

// ================= .obj Loader ================== //

void Object::BuildUpGroup(std::vector<GLfloat>& groupPositions, 
                          std::vector<GLfloat>& groupTexCoords, 
                          std::vector<GLfloat>& groupNormals,
                          std::vector<GLuint>& groupIndices,
                          const char* name, int materialIndex)
{
  // Create new group - mesh, material index and name.
  Mesh* mesh = new Mesh(mProgramHandle);

  mesh->Load(  &groupPositions[0], // Positions
               nullptr,            // Colors
               (groupNormals.size() != 0)   ? &groupNormals[0]   : nullptr,     // Normals
               (groupTexCoords.size() != 0) ? &groupTexCoords[0] : nullptr,     // Texture coords.
               (groupIndices.size() != 0)   ? &groupIndices[0]   : nullptr,     // Indices.
               groupPositions.size()/3, groupIndices.size(), GL_TRIANGLES, Mesh::kSubBuffered
            );

  mGroups.emplace_back(mesh, materialIndex, name);
}

// assimp loading method - works with any kind of 3d model file.
bool Object::LoadFile(const std::string& filePath, bool smoothNormals)
{
  // Load using Assimp::Importer.
  Assimp::Importer importer;
  aiPostProcessSteps postProcessNormal 
      = smoothNormals ? aiProcess_GenSmoothNormals : aiProcess_GenNormals;

  const aiScene* scene = importer.ReadFile(filePath.c_str(), 
      aiProcess_Triangulate | postProcessNormal | aiProcess_FlipUVs);

  // If successfully loaded into scene...
  if (scene != nullptr)
  {
    // For each mesh, create a group.
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
      const aiMesh* mesh = scene->mMeshes[i];
      std::vector<GLfloat> groupPositions;
      std::vector<GLfloat> groupTexCoords;
      std::vector<GLfloat> groupNormals;
      std::vector<GLuint> groupIndices;

      // Save vertices into temporary arrays.
      for (int j = 0; j < mesh->mNumVertices; j++)
      {
        const aiVector3D* pos = &(mesh->mVertices[j]);
        const aiVector3D* nor = mesh->HasNormals() ? &(mesh->mNormals[j]) : nullptr;
        const aiVector3D* tex = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : nullptr;

        groupPositions.push_back(pos->x);
        groupPositions.push_back(pos->y);
        groupPositions.push_back(pos->z);

        if (nor)
        {
          groupNormals.push_back(nor->x);
          groupNormals.push_back(nor->y);
          groupNormals.push_back(nor->z);
        }

        if (tex)
        {
          groupTexCoords.push_back(tex->x);
          groupTexCoords.push_back(tex->y);  
        }
      }

      for (unsigned int j = 0 ; j < mesh->mNumFaces ; j++) 
      {
        const aiFace& face = mesh->mFaces[j];
        groupIndices.push_back(face.mIndices[0]);
        groupIndices.push_back(face.mIndices[1]);
        groupIndices.push_back(face.mIndices[2]);
      }

      // At last, build the entire group.
      Object::BuildUpGroup(groupPositions, groupTexCoords, groupNormals, groupIndices,
                           mesh->mName.C_Str(), mesh->mMaterialIndex);
    }

    // TODO: Initialize material list.

    return true;
  }
  else
  {
    std::cerr << "ERROR Couldn't load scene/mesh at " << filePath << "\n" << importer.GetErrorString();
    return false;
  }
}

// ==================== Parametric Surface Loading ====================== //


bool Object::LoadParametricSurf(std::function<glm::vec3 (float, float)> surf, 
                                std::function<glm::vec3 (float, float)> rgbFunc, 
                                int numSampleU, int numSampleV, bool solid)
{
  int w = numSampleU;
  int h = numSampleV;
  int numVertices = (w * h);
  int numIndices = 0;
  GLenum drawMode = GL_LINE_STRIP;

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  vertices.reserve(numVertices * 6);

  // Initialize vertices.
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float u = static_cast<float>(x)/(w-1);
      float v = static_cast<float>(y)/(h-1);

      glm::vec3 surf_uv = surf(u, v);

      // Positions x, y, z.
      vertices.push_back(surf_uv[0]);
      vertices.push_back(surf_uv[1]);
      vertices.push_back(surf_uv[2]);

      // Colors RGB.
      const glm::vec3 rgb = rgbFunc(u, v);
      vertices.push_back(rgb[0]);
      vertices.push_back(rgb[1]);
      vertices.push_back(rgb[2]);
    }
  }

  
  // Initialize indices.
  if (!solid)  // WIREFRAME.
  {
    numIndices  = (2 * w * h);
    indices.reserve(numIndices);
    
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
  }
  else  // SOLID.
  {
    numIndices  = 2*(h-2)*w + 2*w + 2*(h-2);
    indices.reserve(numIndices);
    drawMode = GL_TRIANGLE_STRIP;

    for (int v = 0; v < h-1; v++)
    {
      // Zig-zag pattern: alternate between top and bottom.
      for (int u = 0; u < w; u++)
      {
        indices.push_back((v+0)*w + u);
        indices.push_back((v+1)*w + u);
      }

      // Triangle row transition: handle discontinuity.
      if (v < h-2)
      {
        // Repeat last vertex and the next row first vertex to generate 
        // two invalid triangles and get continuity in the mesh.
        indices.push_back((v+1)*w + (w-1)); //INDEX(this->width-1, y+1);
        indices.push_back((v+1)*w + 0);     //INDEX(0, y+1);
      }
    }
  }

  Mesh* mesh = new Mesh(mProgramHandle);
  mesh->Load(&vertices[0], &indices[0], numVertices, numIndices, true, false, false, drawMode);
  mUsingLighting = false;
  mGroups.emplace_back(mesh, 0, "Main surface");

  return true;
}

bool Object::LoadParametricSurfSolid( std::function<glm::vec3 (float, float)> surf, 
                                      std::function<glm::vec3 (float, float)> normal,
                                      int numSampleU, int numSampleV)
{
  int w = numSampleU;
  int h = numSampleV;
  int numVertices = (w * h);
  int numIndices  = 2*(h-2)*w + 2*w + 2*(h-2);
  GLenum drawMode = GL_TRIANGLE_STRIP;

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  vertices.reserve(numVertices * 6);
  indices.reserve(numIndices);

  // Initialize vertices.
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float u = static_cast<float>(x)/(w-1);
      float v = static_cast<float>(y)/(h-1);
      glm::vec3 surf_uv = surf(u, v);
      glm::vec3 nor = normal(u, v);

      // Positions x, y, z.
      vertices.push_back(surf_uv[0]);
      vertices.push_back(surf_uv[1]);
      vertices.push_back(surf_uv[2]);

      // Normals.
      vertices.push_back(nor[0]);
      vertices.push_back(nor[1]);
      vertices.push_back(nor[2]);
    }
  }
  
  // Initialize indices.
  for (int v = 0; v < h-1; v++)
  {
    // Zig-zag pattern: alternate between top and bottom.
    for (int u = 0; u < w; u++)
    {
      indices.push_back((v+0)*w + u);
      indices.push_back((v+1)*w + u);
    }

    // Triangle row transition: handle discontinuity.
    if (v < h-2)
    {
      // Repeat last vertex and the next row first vertex to generate 
      // two invalid triangles and get continuity in the mesh.
      indices.push_back((v+1)*w + (w-1)); //INDEX(this->width-1, y+1);
      indices.push_back((v+1)*w + 0);     //INDEX(0, y+1);
    }
  }

  Mesh* mesh = new Mesh(mProgramHandle);
  mesh->Load(&vertices[0], &indices[0], numVertices, numIndices, false, true, false, drawMode);
  mUsingLighting = true;
  mGroups.emplace_back(mesh, 0, "Main surface");

  return true;
}

// ================= Ray Intersection =============== //

bool Object::RayIntersection(const glm::vec3& ray, const glm::vec3& C) const
{
  // TODO: walk through triangle array and compute intersection.
  return true;
}

// ================= Destructor ================== //

Object::~Object()
{
  if (mOwnsData)
  {
    for (auto& group : mGroups)
    {
      delete group.mesh;
    }

    // TODO: check if deleting materials is needed.

  }
}

// ============ Texture Load Methods ============= //
void Texture::Load(int width, int height, GLenum slot)
{
  if (mBuffer != 0)
  {
    glDeleteTextures(1, &mBuffer);
  }

  glGenTextures(1, &mBuffer);
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_2D, mBuffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D( GL_TEXTURE_2D,  // Target.
                0,              // Detail level - original.
                GL_RGB,         // How the colors are stored.
                width,   // Width.
                height,  // Height.
                0,                    // Border must be 0. 
                GL_RGB,               // Format (?).  
                GL_UNSIGNED_BYTE,        // Data type.
                nullptr                  // Buffer address.
  );
}

void Texture::Load(ImageIO* source, GLenum slot)
{
  if (mBuffer != 0)  // There was a texture previously loaded.
  {
    glDeleteTextures(1, &mBuffer);
  }

  glGenTextures(1, &mBuffer);
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_2D, mBuffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int bytesPerPixel = source->getBytesPerPixel();

  glTexImage2D( GL_TEXTURE_2D,  // Target.
                0,              // Detail level - original.
                GL_RGB,  // How the colors are stored.
                source->getWidth(),   // Width.
                source->getHeight(),  // Height.
                0,                    // Border must be 0. 
                GL_RGB,  // Format (?).  
                GL_UNSIGNED_BYTE,               // Data type.
                source->getPixels()             // Buffer address.
  );

}

bool Texture::Load(const std::string& filePath, GLenum slot)
{
  bool successful = false;
  ImageIO* source = new ImageIO();
  if (source->loadJPEG(filePath.c_str()) == ImageIO::OK)
  {
    Texture::Load(source, slot);
    successful = true;
  }
  else
  {
    std::cerr << "WARNING Texture file in " << filePath << " could not be loaded.\n";
  }

  delete source;
  return successful;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TO REVIEW - SIMPLE .OBJ LOADER

bool Object::LoadObjFile(const std::string& objFilePath, bool smoothNormals)
{
  std::ifstream input(objFilePath, std::ios::in);

  if (!input.is_open())
  {
    std::cerr << "ERROR Couldn't load the .obj file at " << objFilePath << ".\n";
    return false;
  }

  std::string line, name;

  const int kNumIndices = 100;
  std::vector<GLfloat> positions;  // List of all positions (x, y, z).
  std::vector<GLfloat> texCoords;  // List of all texture (u, v).
  std::vector<GLfloat> normals;    // List of all normals (nx, ny, nz).

  std::vector<GLfloat> groupPositions;  // List of current group positions.
  std::vector<GLfloat> groupTexCoords;  // List of current group tex coords.
  std::vector<GLfloat> groupNormals;    // List of current group normals.
  std::vector<GLuint> groupIndices;

  bool buildingGroup = false;
  int currentLine = 0;

  // Parse each line.
  while (std::getline(input, line))
  {
    line = tool::RemoveRepeatedCharacters(line, ' ');

    // New group.
    if (line.substr(0, 2) == "g ")
    {
      if (buildingGroup)  // Finish storing previous group.
      {
        Object::BuildUpGroup(groupPositions, groupTexCoords, groupNormals, 
                             groupIndices, name.c_str());
        groupPositions = std::vector<GLfloat>();
        groupTexCoords = std::vector<GLfloat>();
        groupNormals   = std::vector<GLfloat>();
      }
      else
      {
        buildingGroup = true;
      }

      std::istringstream v(line.substr(2));
      v >> name;  // Read group name (it can be void).
    }
    // New vertex.
    else if (line.substr(0, 2) == "v ")
    {
      std::istringstream v(line.substr(2));
      float x, y, z;

      // Read three coordinates.
      v >> x >> y >> z;
      
      if (!v.fail())
      {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
      }
      else
      {
        std::cerr << "ERROR [.obj Parsing] Expected vertex coordinates (3)." << std::endl;
        return false;
      }
    }
    // New texture coordinates (u, v).
    else if (line.substr(0, 3) == "vt ")
    {
      std::istringstream v(line.substr(3));
      float x, y;

      // Read two coordinates.
      v >> x >> y;

      if (!v.fail())
      {
        texCoords.push_back(x);
        texCoords.push_back(y);
      }
      else
      {
        std::cerr << "ERROR [.obj Parsing] Expected uv texture coordinates (2)." << std::endl;
        return false;
      }
        
    }
    // New normal.
    else if (line.substr(0, 3) == "vn ")
    {
      std::istringstream v(line.substr(2));
      float x, y, z;

      // Read three coordinates.
      v >> x >> y >> z;
      
      if (!v.fail())
      {
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
      }
      else
      {
        std::cerr << "ERROR [.obj Parsing] Expected normal components (3)." << std::endl;
        return false;
      }

    }
    // New face.
    else if (line.substr(0, 2) == "f ")
    {
      std::vector<std::string> tokens = tool::SplitString(line, std::vector<std::string>({" ", "\r", "\n"}));
      std::vector<glm::vec3> faceVertices;
      int currentVertex = 0;

      // Read remaining tokens (exclude "f")/
      for (int i = 1; i < tokens.size(); i++)
      {
        std::vector<std::string> indices_single = tool::SplitString(tokens[i], "/");
        std::vector<std::string> indices_double = tool::SplitString(tokens[i], "//");

        if (indices_double.size() > 1)  // No texture coordinates (v // n).
        {
          //std::cout << indices_double[0] << "//" << indices_double[1] << " ";

          int i_v = std::stoi(indices_double[0]) - 1;
          int i_n = std::stoi(indices_double[1]) - 1;

          faceVertices.push_back(glm::make_vec3(&positions[3*i_v]));
          groupPositions.push_back(positions[3*i_v + 0]);
          groupPositions.push_back(positions[3*i_v + 1]);
          groupPositions.push_back(positions[3*i_v + 2]);

          groupNormals.push_back(normals[3*i_n + 0]);
          groupNormals.push_back(normals[3*i_n + 1]);
          groupNormals.push_back(normals[3*i_n + 2]);        

        }
        else if ((indices_single.size() == 1) 
              && (indices_double.size() == 1))  // No texture coordinates and no normals (v / t / n).
        {
          // int i_v = std::stoi(indices_double[0]) - 1;

          // groupPositions.push_back(positions[3*i_v + 0]);
          // groupPositions.push_back(positions[3*i_v + 1]);
          // groupPositions.push_back(positions[3*i_v + 2]);
        }
        else  // v
        {
          //std::cout << indices_single[0] << "/" << indices_single[1] << "/" << indices_single[2] << " ";
          int i_v = std::stoi(indices_single[0]) - 1;
          int i_t = std::stoi(indices_single[1]) - 1;
          int i_n = std::stoi(indices_single[2]) - 1;

          faceVertices.push_back(glm::make_vec3(&positions[3*i_v]));
          groupPositions.push_back(positions[3*i_v + 0]);
          groupPositions.push_back(positions[3*i_v + 1]);
          groupPositions.push_back(positions[3*i_v + 2]);

          groupTexCoords.push_back(texCoords[2*i_t + 0]);
          groupTexCoords.push_back(texCoords[2*i_t + 1]);

          groupNormals.push_back(normals[3*i_n + 0]);
          groupNormals.push_back(normals[3*i_n + 1]);
          groupNormals.push_back(normals[3*i_n + 2]);
        }

        currentVertex++;
      }

      if (!smoothNormals)
      {
        // Compute normal per triangle.
        glm::vec3 n = tool::TriangleNormal(faceVertices[0], faceVertices[1], faceVertices[2]);
        // groupNormals.push_back(n[0]);
        // groupNormals.push_back(n[1]);
        // groupNormals.push_back(n[2]);
      }

    }
    // ADD MORE OPTIONS AND COMMANDS HERE.
    else
    {
      // Ignore everything else.
    }
  }

  Object::BuildUpGroup(groupPositions, groupTexCoords, groupNormals, groupIndices, name.c_str());

  return true;
}


}  // namespace obj

}  // namespace gloo






