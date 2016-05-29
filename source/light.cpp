#include "light.h"

namespace gloo
{

void Light::Position(OpenGLMatrix& viewMatrix, int id) 
{
  // In order to compute specular component, the light position
  // and fragment position are in camera coordinates.
  glm::vec4 p(mPos, 1.0);
  glm::vec4 n(mDir, 0.0);
  glm::mat4& V = viewMatrix.GetGLMatrix();

  // Transform to camera coordinates before update light uniform to shader.
  p = V * p;
  n = V * n;
  p = p / p[3];  // Homogenous coordinates - make w == 1 in (x, y, z, w)'.

  // Set uniforms for fragment shader.
  GLuint location;

  // Postion.
  location = glGetUniformLocation(mProgramHandle, std::string("light[" + std::to_string(id) + "].position").c_str());
  glUniform3f(location, p[0], p[1], p[2]);

  // Normal (orientation).
  location = glGetUniformLocation(mProgramHandle, std::string("light[" + std::to_string(id) + "].normal").c_str());
  glUniform3f(location, n[0], n[1], n[2]);

  // Ambient component.
  location = glGetUniformLocation(mProgramHandle, std::string("light[" + std::to_string(id) + "].La").c_str());
  glUniform1f(location, mLa);

  // Specular component.
  location = glGetUniformLocation(mProgramHandle, std::string("light[" + std::to_string(id) + "].Ld").c_str());
  glUniform1f(location, mLd);

  // Specular component.
  location = glGetUniformLocation(mProgramHandle, std::string("light[" + std::to_string(id) + "].Ls").c_str());
  glUniform1f(location, mLs);
}

void Light::Animate()
{
  // static GLfloat t = 0.0f;
  // mPos[0] = 4.0f*sin(2*t);
  // mPos[2] = 4.0f*cos(2*t);
  // t -= 0.02f;
}

}  // namespace gloo.