#include "utilities.h"

#include <glm/gtc/type_ptr.hpp>

namespace gloo
{

namespace tool
{

/* General math/geometry utilities */

float TriangleSurfaceArea(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2)
{
  return std::abs(glm::length(glm::cross(v1-v0, v2-v0)));
}

float TriangleSurfaceArea(const float* v0, const float* v1, const float* v2)
{
  return TriangleSurfaceArea(glm::make_vec3(v0), glm::make_vec3(v1), glm::make_vec3(v2));
}

glm::vec3 TriangleNormal(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2)
{
  return -glm::normalize(glm::cross(v1-v0, v2-v0));
}

glm::vec3 TriangleNormal(const float* v0, const float* v1, const float* v2)
{
  return TriangleNormal(glm::make_vec3(v0), glm::make_vec3(v1), glm::make_vec3(v2));
}

/* General parsing utilities */

std::vector<std::string> SplitString(const std::string & str, const std::string & separator)
{
  std::vector<std::string> tokens;
  int i = 0;

  while (i < str.size())
  {
    size_t next = str.find(separator, i);

    if (next != std::string::npos)  // Found something.
    {
      tokens.push_back(str.substr(i, next-i));
      i = next + separator.size();
    }
    else  // Did not find - break.
    {
      tokens.push_back(str.substr(i));
      break;
    }
  }

  return tokens;
}

std::vector<std::string> SplitString(const std::string & str, const std::vector<std::string> & separators)
{
  std::vector<std::string> tokens;
  int i = 0;

  while (i < str.size())
  {
    size_t next;
    int foundSeparator = -1;

    for (int j = 0; j < separators.size(); j++)
    {
      next = str.find(separators[j], i);

      if (next != std::string::npos)  // The separator was found.
      {
        foundSeparator = j;
        break;
      }
    }

    if (next != std::string::npos)  // Found something.
    {
      const std::string& separator = separators[foundSeparator];
      tokens.push_back(str.substr(i, next-i));
      i = next + separator.size();

    }
    else  // Did not find - break.
    {
      if (str.size() - i - 1 > 0)  // Not empty.
      {
        tokens.push_back(str.substr(i));
      }

      break;
    }
  }

  return tokens;
}

std::string RemoveRepeatedCharacters(const std::string & str, char c)
{
  std::string result = str;

  int i = 0;
  while (result[i] != '\0')
  {
    if (result[i] == c && result[i+1] == c)
    {
      for (int j = i; result[j] != '\0'; j++)
      {
        result[j] = result[j+1];
      }
    }
    else  
    {
      i++;
    }   
  }
  result.resize(i);

  return result;
}

}  // namespace tool.
}  // namespace gloo.