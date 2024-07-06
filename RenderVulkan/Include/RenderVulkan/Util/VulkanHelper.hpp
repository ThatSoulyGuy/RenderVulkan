#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Window.hpp"
#include "RenderVulkan/Render/QueueFamilyIndices.hpp"
#include "RenderVulkan/Render/SwapChainSupportDetails.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Render;

namespace RenderVulkan
{
	namespace Util
	{
		class VulkanHelper
		{

		public:

			VulkanHelper(const VulkanHelper&) = delete;
			VulkanHelper& operator=(const VulkanHelper&) = delete;

			static Vector<const char*> GetRequiredExtensions(bool enableValidationLayers, Vector<const char*> validationLayers)
			{
				if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers))
					Logger_ThrowException("Validation layers requested, but not available", true);

				uint glfwExtensionCount = 0;
				const char** glfwExtensions;

				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				Vector<const char*> requiredExtensions;

				for (uint e = 0; e < glfwExtensionCount; e++)
					requiredExtensions.emplace_back(glfwExtensions[e]);

				requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

				if (enableValidationLayers) 
					requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				
				return requiredExtensions;
			}

			static bool CheckValidationLayerSupport(Vector<const char*> validationLayers)
			{
				uint layerCount;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

				Vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				for (const char* layerName : validationLayers)
				{
					bool layerFound = false;

					for (const auto& layerProperties : availableLayers)
					{
						if (strcmp(layerName, layerProperties.layerName) == 0)
						{
							layerFound = true;
							break;
						}
					}

					if (!layerFound)
						return false;
				}

				return true;
			}

			static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInformation, PFN_vkDebugUtilsMessengerCallbackEXT callback)
			{
				createInformation = {};
				createInformation.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInformation.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInformation.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInformation.pfnUserCallback = callback;
			}

			static SwapChainSupportDetails GetSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
			{
				SwapChainSupportDetails details;

				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

				uint formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

				if (formatCount != 0)
				{
					details.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
				}

				uint presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

				if (presentModeCount != 0)
				{
					details.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
				}

				return details;
			}

			static bool IsDeviceCompatable(VkPhysicalDevice device, Vector<const char*> deviceExtensions, VkSurfaceKHR surface) 
			{
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(device, &deviceProperties);

				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

				bool extensionsSupported = CheckDeviceExtensionSupport(device, deviceExtensions);

				bool swapChainAdequate = false;

				if (extensionsSupported) 
				{
					SwapChainSupportDetails swapChainSupport = GetSwapChainSupport(device, surface);
					swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
				}

				return (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && deviceFeatures.tessellationShader) && FindQueueFamilies(device, surface).IsSet() && extensionsSupported && swapChainAdequate;
			}

			static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) 
			{
				QueueFamilyIndices indices = {};
				
				uint queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

				Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

				int i = 0;

				for (const auto& queueFamily : queueFamilies) 
				{
					if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
						indices.graphicsFamily = i;

					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

					if (presentSupport) 
						indices.presentFamily = i;
					
					if (indices.IsSet()) 
						break;
					
					i++;
				}

				return indices;
			}

			static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, Vector<const char*> deviceExtensions)
			{
				uint extensionCount;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

				Vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

				Set<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

				for (const auto& extension : availableExtensions) 
					requiredExtensions.erase(extension.extensionName);
				
				return requiredExtensions.empty();
			}

			static VkSurfaceFormatKHR GetSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& availableFormats) 
			{
				for (const auto& availableFormat : availableFormats) 
				{
					if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
						return availableFormat;
				}

				return availableFormats[0];
			}

			static VkPresentModeKHR GetSwapPresentMode(const Vector<VkPresentModeKHR>& availablePresentModes)
			{
				for (const auto& availablePresentMode : availablePresentModes) 
				{
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
						return availablePresentMode;
				}

				return VK_PRESENT_MODE_FIFO_KHR;
			}

			static VkExtent2D GetSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
			{
				if (capabilities.currentExtent.width != NumericLimits<uint>::max()) 
					return capabilities.currentExtent;
				else 
				{
					int width, height;
					glfwGetFramebufferSize(Window::GetInstance()->GetHandle(), &width, &height);

					VkExtent2D actualExtent = 
					{
						static_cast<uint>(width),
						static_cast<uint>(height)
					};

					actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
					actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

					return actualExtent;
				}
			}

			VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice) 
			{
				VkPhysicalDeviceProperties physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

				VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

				if (counts & VK_SAMPLE_COUNT_64_BIT) 
					return VK_SAMPLE_COUNT_64_BIT; 

				if (counts & VK_SAMPLE_COUNT_32_BIT) 
					return VK_SAMPLE_COUNT_32_BIT; 

				if (counts & VK_SAMPLE_COUNT_16_BIT) 
					return VK_SAMPLE_COUNT_16_BIT; 

				if (counts & VK_SAMPLE_COUNT_8_BIT) 
					return VK_SAMPLE_COUNT_8_BIT; 

				if (counts & VK_SAMPLE_COUNT_4_BIT) 
					return VK_SAMPLE_COUNT_4_BIT; 

				if (counts & VK_SAMPLE_COUNT_2_BIT) 
					return VK_SAMPLE_COUNT_2_BIT; 

				return VK_SAMPLE_COUNT_1_BIT;
			}

		private:

			VulkanHelper() = default;

		};
	}
}