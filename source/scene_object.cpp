#include "scene_object.h"

namespace gloo
{
  
void SceneObject::Render() const
{
  if (IsInitialized())
  {
    mPipelineProgram->SetModelMatrix(mModelMatrix);

    GLuint matLoc = glGetUniformLocation(mProgramHandle, "material_on");
    if (HasMaterial())
    {
      mMaterial->Bind(mProgramHandle);
      glUniform1i(matLoc, 1);
    }
    else
    {
      glUniform1i(matLoc, 0); 
    }

    GLuint texLoc = glGetUniformLocation(mProgramHandle, "tex_on");
    if (HasTexture() && mTexture->Valid())
    {
      glEnable(GL_TEXTURE_2D);
      mTexture->Bind(mProgramHandle);
      glUniform1i(texLoc, 1);
    }
    else
    {
      glDisable(GL_TEXTURE_2D);
      glUniform1i(texLoc, 0);
    }

    GLuint lightOnLoc = glGetUniformLocation(mProgramHandle, "light_on");
    glUniform1i(lightOnLoc, mUsingLighting);

    mMesh->Render();
  }
}

void SceneObject::Animate()
{
  // Update position and rotation from corresponding velocities.
  mPos += mLinVelocity;
  mRot += mRotVelocity;

  mModelMatrix.LoadIdentity();
  mModelMatrix.Translate(mPos[0], mPos[1], mPos[2]);
  mModelMatrix.Rotate(mRot[2], 0, 0, 1);
  mModelMatrix.Rotate(mRot[0], 1, 0, 0);
  mModelMatrix.Rotate(mRot[1], 0, 1, 0);
  mModelMatrix.Scale(mScale[0], mScale[1], mScale[2]);
}

bool SceneObject::IntersectRay(const glm::vec3& r, const glm::vec3& C) const
{
  // Transform coordinates from view to model.
  glm::mat4 M_inv = glm::inverse(mModelMatrix.GetGLMatrix());
  glm::vec4 v = M_inv * glm::vec4(r, 0.0);  // Orientation vector.
  glm::vec4 O = M_inv * glm::vec4(C, 1.0);  // Line origin.
  
  O /= O[3];
  v = glm::normalize(v);

  GLfloat* position;
  float min_d = 100000.0f;
  int bestPoint = -1;
  int n = mMesh->GetNumVertices();
  glm::vec3 P;

  // Loop through positions to find the closest one to the ray.
  for (int i = 0; i < n; i++)
  {
    position = mMesh->PositionAt(i);
    P = glm::vec3(position[0], position[1], position[2]);
    float d = glm::length( glm::cross( P-C, r ) );

    if (d < min_d)
    {
      min_d = d;
      bestPoint = i;
    }
  }

  position = mMesh->PositionAt(bestPoint);
  printf("vertex %.4f %.4f %.4f\n", position[0], position[1], position[2]);

  // TODO: improve threshold.
  // Check the minimum distance threshold.
  if (min_d < 5.0f) // Success.
  {
    return true;
  }
  else  // No point selected.
  {
    return false;
  }
}

SceneObject::~SceneObject()
{
  if (mIsMeshOwner)
  {
    delete mMesh;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Material::Bind(GLuint programHandle)
{
  // Set Material.
  GLuint locKa = glGetUniformLocation(programHandle, "material.Ka");
  GLuint locKd = glGetUniformLocation(programHandle, "material.Kd");
  GLuint locKs = glGetUniformLocation(programHandle, "material.Ks");

  glUniform3f(locKa, mKa[0], mKa[1], mKa[2]);
  glUniform3f(locKd, mKd[0], mKd[1], mKd[2]);
  glUniform3f(locKs, mKs[0], mKs[1], mKs[2]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(int width, int height)
{
  if (mBuffer != 0)  // There was a texture previously loaded.
  {
    glDeleteTextures(1, &mBuffer);
  }

  glGenTextures(1, &mBuffer);
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

void Texture::Load(ImageIO* source)
{
  if (mBuffer != 0)  // There was a texture previously loaded.
  {
    glDeleteTextures(1, &mBuffer);
  }

  glGenTextures(1, &mBuffer);
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

bool Texture::Load(const std::string& fileName)
{
  bool successful = false;
  ImageIO* source = new ImageIO();
  if (source->loadJPEG(fileName.c_str()) == ImageIO::OK)
  {
    Texture::Load(source);
    successful = true;
  }
  else
  {
    std::cerr << "WARNING Texture file in " << fileName << " could not be loaded.\n";
  }

  delete source;
  return successful;
}

}  // namespace gloo.

