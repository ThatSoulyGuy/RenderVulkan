#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Render/Renderer.hpp"
#include "RenderVulkan/Render/Mesh.hpp"
#include "RenderVulkan/Render/ShaderManager.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Render;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	class Engine
	{

	public:

		void PreInitialize()
		{
			Logger_WriteConsole("Pre-initializing engine...", LogLevel::INFORMATION);

			Settings::GetInstance()->Set<String>("windowTitle", "RenderVulkan* 0.0.3");
			Settings::GetInstance()->Set<Vector2i>("windowDimensions", { 750, 450 });
			Settings::GetInstance()->Set<Function<void(Vector2i)>>("windowResizeCallback", [](Vector2i dimensions)
			{
				if (!Renderer::GetInstance()->IsInitialized())
					return;

				Renderer::GetInstance()->Resize(dimensions);
			});
		}

		void Initialize()
		{
			Logger_WriteConsole("Initializing engine...", LogLevel::INFORMATION);

			Renderer::GetInstance()->Initialize();
			
			ShaderManager::GetInstance()->Register(Shader::Create("Shader/Default", "default"));
			ShaderManager::GetInstance()->CreateShaderGraphicsPipelines(Renderer::GetInstance()->GetRenderPass());
			
			mesh = Mesh::Create("mesh", ShaderManager::GetInstance()->Get("default"), 
			{
				{{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
				{{  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
				{{  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
				{{ -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }}
			}, 
			{
				0, 1, 2, 
				2, 3, 0
			});

			mesh->Generate();

			Renderer::GetInstance()->SetRenderCallback([this](VkCommandBuffer commandBuffer, int index)
			{ 
				mesh->Render(commandBuffer);
			});
		}

		void Update()
		{
			
		}

		void Render()
		{
			Renderer::GetInstance()->Render();
		}

		void CleanUp()
		{
			Logger_WriteConsole("Cleaning up engine...", LogLevel::INFORMATION);

			mesh->CleanUp();
			ShaderManager::GetInstance()->CleanUp();
			Renderer::GetInstance()->CleanUp();
		}

		static Shared<Engine> GetInstance()
		{
			static Shared<Engine> instance = std::make_shared<Engine>();

			return instance;
		}

	private:

		Shared<Mesh> mesh;

	};
}