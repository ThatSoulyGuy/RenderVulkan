#pragma once

#include "RenderVulkan/ECS/GameObject.hpp"

namespace RenderVulkan
{
	namespace ECS
	{
		class GameObjectManager
		{

		public:

			GameObjectManager(const GameObjectManager&) = delete;
			GameObjectManager& operator=(const GameObjectManager&) = delete;

			void Register(Shared<GameObject> gameObject)
			{
				registeredGameObjects[gameObject->GetName()] = gameObject;
			}

			void Unregister(const String& name)
			{
				registeredGameObjects.erase(name);
			}

			void Update()
			{
				for (auto& [name, gameObject] : registeredGameObjects)
					gameObject->Update();
			}

			void Render(VkCommandBuffer commandBuffer)
			{
				for (auto& [name, gameObject] : registeredGameObjects)
					gameObject->Render(commandBuffer);
			}

			void CleanUp()
			{
				for (auto& [name, gameObject] : registeredGameObjects)
					gameObject->CleanUp();

				registeredGameObjects.clear();
			}

			static Shared<GameObjectManager> GetInstance()
			{
				class EnabledGameObjectManager : public GameObjectManager { };
				static Shared<GameObjectManager> instance = std::make_shared<EnabledGameObjectManager>();

				return instance;
			}

		private:

			GameObjectManager() = default;

			UnorderedMap<String, Shared<GameObject>> registeredGameObjects;

		};
	}
}