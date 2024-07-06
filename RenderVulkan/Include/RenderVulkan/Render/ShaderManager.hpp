#pragma once

#include "RenderVulkan/Render/Shader.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		class ShaderManager
		{

		public:

			ShaderManager(const ShaderManager&) = delete;
			ShaderManager& operator=(const ShaderManager&) = delete;

			void Register(Shared<Shader> shader)
			{
				registeredShaders[shader->GetName()] = shader;
			}

			void Unregister(const String& name)
			{
				registeredShaders.erase(name);
			}

			Shared<Shader> Get(const String& name)
			{
				return registeredShaders[name];
			}

			void CleanUp()
			{
				for (auto& [name, shader] : registeredShaders)
					shader->CleanUp();
			}

			static Shared<ShaderManager> GetInstance()
			{
				class EnabledShaderManager : public ShaderManager { };
				static Shared<ShaderManager> instance = std::make_shared<EnabledShaderManager>();

				return instance;
			}

		private:

			ShaderManager() = default;

			UnorderedMap<String, Shared<Shader>> registeredShaders;

		};
	}
}