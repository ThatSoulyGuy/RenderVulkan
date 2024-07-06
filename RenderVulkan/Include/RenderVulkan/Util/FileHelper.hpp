#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Core;

namespace RenderVulkan
{
	namespace Util
	{
		class FileHelper
		{

		public:

			FileHelper(const FileHelper&) = delete;
			FileHelper& operator=(const FileHelper&) = delete;

			static String ReadFile(const String& path)
			{
				InputFileStream file(path, std::ios::ate | std::ios::binary);

				if (!file.is_open())
					Logger_ThrowException("Failed to open file: '" + path + "'", true);

				Size fileSize = (Size)file.tellg();
				String buffer(fileSize, ' ');

				file.seekg(0);
				file.read(buffer.data(), fileSize);

				file.close();

				return buffer;
			}

			static Vector<char> ReadFileIntoVector(const String& filename)
			{
				InputFileStream file(filename, std::ios::ate | std::ios::binary);

				if (!file.is_open())
					Logger_ThrowException("Failed to open file: '" + filename + "'", true);
				
				Size fileSize = static_cast<Size>(file.tellg());

				Vector<char> buffer(fileSize);

				file.seekg(0);
				file.read(buffer.data(), fileSize);

				file.close();

				return buffer;
			}

		private:

			FileHelper() = default;

		};
	}
}