#pragma once

#include "RenderVulkan/Util/Typedefs.hpp"

namespace RenderVulkan
{
	namespace Util
	{
		class DataHelper
		{

		public:

			DataHelper(const DataHelper&) = delete;
			DataHelper& operator=(const DataHelper&) = delete;

			static void CopyMemory(const void* source, void* destination, const Size& size)
			{
				memcpy(destination, source, size);
			}

			static void SetMemory(void* destination, const Size& size, const Byte& value)
			{
				memset(destination, value, size);
			}

			template<typename T, typename U, typename Func>
			static Vector<U> TransformVector(const Vector<T>& inputVector, Func operation)
			{
				Vector<U> outputVector(inputVector.size());

				std::transform(inputVector.begin(), inputVector.end(), outputVector.begin(), operation);

				return outputVector;
			}

			static Vector<const char*> ConvertToCStrVector(const Vector<String>& inputVector)
			{
				Vector<Shared<String>> stringPtrs;
				stringPtrs.reserve(inputVector.size());

				for (const auto& string : inputVector)
					stringPtrs.push_back(std::make_shared<String>(string));
				
				return TransformVector<Shared<String>, const char*>
				(
					stringPtrs, [](const Shared<String>& strPtr) 
					{
						return strPtr->c_str();
					}
				);
			}

		private:

			DataHelper() = default;
		};
	}
}