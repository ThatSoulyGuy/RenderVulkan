#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			Vector<VkSurfaceFormatKHR> formats;
			Vector<VkPresentModeKHR> presentModes;
		};
	}
}