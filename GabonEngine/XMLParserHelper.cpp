#include "XMLParserHelper.h"
#include <sstream>
namespace XMLParserHelper
{
	std::vector<std::string> XMLParserHelper::Split(const std::string& str, const std::string& delims /*= ","*/)
	{
		StringList ret;
		if (str.empty())
			return ret;
		ret.reserve(10);
		size_t start, pos;
		start = 0;
		do 
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				start = pos + 1;
			}
			else if (pos == std::string::npos)
			{
				ret.push_back(str.substr(start));
				break;
			}
			else
			{
				ret.push_back(str.substr(start, pos - start));
				start = pos + 1;
			}
			start = str.find_first_not_of(delims, start);			
		} while (pos != std::string::npos);

		return ret;
	}

	Ogre::Vector3 ParseVec3(const std::string& val)
	{
		StringList vec = Split(val);
		if (vec.size() != 3)
		{
			return Vector3::ZERO;
		}
		else
			return Vector3(ParseFloat(vec[0]), ParseFloat(vec[1]), ParseFloat(vec[2]));
	}

	Ogre::Vector2 ParseVec2(const std::string& val)
	{
		StringList vec = Split(val);
		if (vec.size() != 2)
		{
			return Vector2::ZERO;
		}
		else
			return Vector2(ParseFloat(vec[0]), ParseFloat(vec[1]));
	}

	float ParseFloat(const std::string& val)
	{
		std::stringstream str(val);
		float ret = 0.f;
		str >> ret;
		return ret;
	}

}
