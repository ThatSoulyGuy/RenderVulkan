#pragma once

#include "RenderVulkan/Util/Typedefs.hpp"

namespace RenderVulkan
{
	namespace Util
	{
		struct QueueFamilyIndices 
		{
			Optional<uint> graphicsFamily;
			Optional<uint> presentFamily;

			bool IsSet() const
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};
	}
}