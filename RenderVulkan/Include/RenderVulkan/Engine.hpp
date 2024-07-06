#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/ECS/GameObjectManager.hpp"
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

			Settings::GetInstance()->Set<String>("windowTitle", "RenderVulkan* 0.1.3");
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

			Unique<DescriptorManager> descriptorManager = DescriptorManager::Create();

			descriptorManager->CreateDescriptorSetLayout
			({
				{ 
					0, 
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
					1, 
					VK_SHADER_STAGE_VERTEX_BIT, 
					nullptr 
				}
			});

			descriptorManager->CreateDescriptorPool
			({
				{ 
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
					1 
				}
			}, 1);
			
			ShaderManager::GetInstance()->Register(Shader::Create("Shader/Default", "default", std::move(descriptorManager)));
			ShaderManager::GetInstance()->CreateShaderGraphicsPipelines(Renderer::GetInstance()->GetRenderPass());
			
			gameObject = GameObject::Create("gameObject");
			GameObjectManager::GetInstance()->Register(gameObject);

			gameObject->AddComponent<Shader>(ShaderManager::GetInstance()->Get("default"));

			gameObject->AddComponent<Mesh>(Mesh::Create("mesh", 
			{
				{{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
				{{  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
				{{  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
				{{ -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }}
			}, 
			{
				0, 1, 2, 
				2, 3, 0
			}));

			gameObject->GetComponent<Mesh>()->Generate();

			Renderer::GetInstance()->SetRenderCallback([this](VkCommandBuffer commandBuffer, int index)
			{ 
				GameObjectManager::GetInstance()->Render(commandBuffer);
			});
		}

		void Update()
		{
			gameObject->GetTransform()->Rotate({0.0f, 0.0f, -0.01f});

			GameObjectManager::GetInstance()->Update();
		}

		void Render()
		{
			Renderer::GetInstance()->Render();
		}

		void CleanUp()
		{
			Logger_WriteConsole("Cleaning up engine...", LogLevel::INFORMATION);

			GameObjectManager::GetInstance()->CleanUp();
			ShaderManager::GetInstance()->CleanUp();
			Renderer::GetInstance()->CleanUp();
		}

		static Shared<Engine> GetInstance()
		{
			static Shared<Engine> instance = std::make_shared<Engine>();

			return instance;
		}

	private:

		Shared<GameObject> gameObject;

	};
}