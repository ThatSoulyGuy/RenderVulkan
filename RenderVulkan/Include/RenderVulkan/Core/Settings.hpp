#pragma once

#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Core
	{
		class Settings
		{

		public:
			
			template<typename T>
			void Set(const String& key, const T& value)
			{
				registeredSettings[key] = std::make_any<T>(value);
			}

			template<typename T>
			T Get(const String& key)
			{
				if (!registeredSettings.contains(key))
					return T();

				return std::any_cast<T>(registeredSettings[key]);
			}

			template<typename T>
			void SetPointer(const String& key, T handle)
			{
				registeredSettings[key] = std::make_any<uint64_t>(reinterpret_cast<uint64_t>(handle));
			}

			template<typename T>
			T GetPointer(const std::string& key)
			{
				if (!registeredSettings.contains(key))
					return nullptr;

				return reinterpret_cast<T>(std::any_cast<uint64_t>(registeredSettings[key]));
			}

			static Shared<Settings> GetInstance()
			{
				class EnabledSettings : public Settings { };

				static Shared<Settings> instance = std::make_shared<EnabledSettings>();

				return instance;
			}

		private:

			Settings() = default;

			UnorderedMap<String, Any> registeredSettings;

		};
	}
}