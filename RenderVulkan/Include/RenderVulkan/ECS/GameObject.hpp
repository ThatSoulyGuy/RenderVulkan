#pragma once

#include "RenderVulkan/ECS/Component.hpp"
#include "RenderVulkan/Math/Transform.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Math;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace ECS
	{
		class GameObject : public EnableShared<GameObject>
		{

		public:

			template<typename T>
			void AddComponent(Shared<T> component)
			{
				if (!std::is_base_of<Component, T>::value)
					return;

				component->gameObject = std::static_pointer_cast<GameObject>(shared_from_this());
				component->transform = GetComponent<Transform>();
				component->Initialize();

				components[typeid(T)] = component;
			};

			template<typename T>
			Shared<T> GetComponent()
			{
				if (!std::is_base_of<Component, T>::value)
					return nullptr;

				if (components.contains(typeid(T)))
					return std::dynamic_pointer_cast<T>(components[typeid(T)]);
				
				return nullptr;
			}

			template<typename T>
			bool HasComponent()
			{
				return !components.empty();
			}

			template<typename T>
			void RemoveComponent()
			{
				if (!std::is_base_of<Component, T>::value)
					return;

				if (components.contains(typeid(T)))
					components.erase(typeid(T));
			}

			String GetName() const
			{
				return name;
			}

			void Update()
			{
				for (auto& [type, component] : components)
					component->Update();
			}

			void Render(VkCommandBuffer commandBuffer)
			{
				for (auto& [type, component] : components)
					component->Render(commandBuffer);
			}

			void CleanUp()
			{
				for (auto& [type, component] : components)
					component->CleanUp();

				components.clear();
			}

			static Shared<GameObject> Create(const String& name)
			{
				Shared<GameObject> gameObject = std::make_shared<GameObject>();

				gameObject->name = name;
				gameObject->AddComponent<Transform>(Transform::Create());

				return gameObject;
			}

			Shared<Transform> GetTransform()
			{
				return GetComponent<Transform>();
			}

		private:

			String name;
			UnorderedMap<TypeIndex, Shared<Component>> components;

		};
	}
}