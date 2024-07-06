#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Render/Renderer.hpp"
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
			
			Renderer::GetInstance()->SetRenderCallback([this](VkCommandBuffer commandBuffer, int index)
			{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ShaderManager::GetInstance()->Get("default")->GetPipeline());

				VkExtent2D swapChainExtent = Renderer::GetInstance()->GetSwapChainExtent();

				VkViewport viewport{};

				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = static_cast<float>(swapChainExtent.width);
				viewport.height = static_cast<float>(swapChainExtent.height);
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

				VkRect2D scissor{};

				scissor.offset = { 0, 0 };
				scissor.extent = swapChainExtent;

				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
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

			ShaderManager::GetInstance()->CleanUp();
			Renderer::GetInstance()->CleanUp();
		}

		static Shared<Engine> GetInstance()
		{
			static Shared<Engine> instance = std::make_shared<Engine>();

			return instance;
		}
	};
}