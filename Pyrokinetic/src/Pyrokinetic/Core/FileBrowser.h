#pragma once

#include <string>

namespace pk
{
	namespace util
	{
		class FileBrowser
		{
		public:
			static std::string Open(const char* filter);
			static std::string Save(const char* filter);
		};
	}
}