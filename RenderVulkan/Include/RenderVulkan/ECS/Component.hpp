#pragma once

#include "RenderVulkan/Util/Typedefs.hpp"

namespace RenderVulkan
{
	namespace Math
	{
		class Transform;
	}
}

using namespace RenderVulkan::Math;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace ECS
	{
		class GameObject;

		class Component
		{

		public:

			virtual void Initialize() { }
			virtual void Update() { }
			virtual void Render(VkCommandBuffer commandBuffer) { }
			virtual void CleanUp() { }

			Shared<GameObject> gameObject;
			Shared<Transform> transform;

		};
	}
}