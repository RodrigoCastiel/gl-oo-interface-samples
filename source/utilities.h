/******************************************+
*                                          *
*  CSCI420 - Computer Graphics USC         *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

namespace gloo
{

namespace tool
{

/* General math/geometry utilities */

float TriangleSurfaceArea(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2);
float TriangleSurfaceArea(const float* v0, const float* v1, const float* v2);

glm::vec3 TriangleNormal(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2);
glm::vec3 TriangleNormal(const float* v0, const float* v1, const float* v2);

/* General parsing utilities */

// TODO: Description.
std::vector<std::string> SplitString(const std::string & str, const std::string & separator);

// TODO: Description.
std::vector<std::string> SplitString(const std::string & str, const std::vector<std::string> & separators);

// TODO: Description.
std::string RemoveRepeatedCharacters(const std::string & str, char c);

}  // namespace tool.
}  // namespace gloo.