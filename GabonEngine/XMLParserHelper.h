#include <string>
#include <vector>
#include "MathHeader.h"
namespace XMLParserHelper
{
	typedef std::vector<std::string> StringList;
	StringList Split(const std::string& str, const std::string& delims = ",");
	Vector3 ParseVec3(const std::string& val);
	Vector2 ParseVec2(const std::string& val);
	float ParseFloat(const std::string& val);
}