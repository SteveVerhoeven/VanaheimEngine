#pragma once
#include <string>

namespace WindowsUtils
{
	class FileDialogs final
	{
		public:
			FileDialogs() = default;
			~FileDialogs() = default;

			FileDialogs(const FileDialogs&) = delete;
			FileDialogs(FileDialogs&&) noexcept = delete;
			FileDialogs& operator=(const FileDialogs&) = delete;
			FileDialogs& operator=(FileDialogs&&) noexcept = delete;

			static std::string OpenFile(const char* filter);
			static std::string SaveFile(const char* filter);

		protected:
		private:
	};
}
