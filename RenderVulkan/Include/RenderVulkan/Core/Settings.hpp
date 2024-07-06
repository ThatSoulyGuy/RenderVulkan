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
				if (registeredSettings.contains(key))
					return;

				registeredSettings[key] = value;
			}

			template<typename T>
			T Get(const String& key)
			{
				if (!registeredSettings.contains(key))
					return T();

				return std::any_cast<T>(registeredSettings[key]);
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