#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Window.hpp"
#include "RenderVulkan/Util/DataHelper.hpp"
#include "RenderVulkan/Util/VulkanHelper.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		class Renderer
		{

		public:

			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;

			void Initialize()
			{
				uint extensionsCount = 0;
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

				Logger_WriteConsole("Number of extensions supported: '" + std::to_string(extensionsCount) + "'", LogLevel::INFORMATION);

				CreateInstance();
				SetupDebugMessenger();
				CreateWindowSurface();
				CreatePhysicalDevice();
				CreateLogicalDevice();
				CreateCommandPool();
				CreateSwapChain();
				CreateImageViews();
				CreateRenderPass();
				CreateFramebuffers();
				CreateCommandBuffers();
				CreateSyncObjects();

				Settings::GetInstance()->SetPointer<VkInstance>("vulkanInstance", instance);
				Settings::GetInstance()->SetPointer<VkDevice>("logicalDevice", device);
				Settings::GetInstance()->SetPointer<VkPhysicalDevice>("physicalDevice", physicalDevice);
				Settings::GetInstance()->SetPointer<VkQueue>("graphicsQueue", graphicsQueue);
				Settings::GetInstance()->SetPointer<VkQueue>("presentQueue", presentQueue);
				Settings::GetInstance()->SetPointer<VkSurfaceKHR>("surface", surface);
				Settings::GetInstance()->Set<VkExtent2D>("swapChainExtent", swapChainExtent);

				isInitalized = true;
			}

			void Render()
			{
				if (swapChainExtent.width <= 0 || swapChainExtent.height <= 0)
					return;

				vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
				vkResetFences(device, 1, &inFlightFences[currentFrame]);

				uint imageIndex;

				VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
				Logger_ThrowIfFailed(result, "Failed to acquire swap chain image", true);

				vkResetCommandBuffer(commandBuffers[imageIndex], 0);
				RecordCommandBuffer(commandBuffers[imageIndex], imageIndex);

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };

				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = waitSemaphores;
				submitInfo.pWaitDstStageMask = waitStages;

				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

				VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = signalSemaphores;

				result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
				Logger_ThrowIfFailed(result, "Failed to submit draw command buffer", true);

				VkPresentInfoKHR presentInfo = {};

				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

				presentInfo.waitSemaphoreCount = 1;
				presentInfo.pWaitSemaphores = signalSemaphores;

				VkSwapchainKHR swapChains[] = { swapChain };

				presentInfo.swapchainCount = 1;
				presentInfo.pSwapchains = swapChains;
				presentInfo.pImageIndices = &imageIndex;

				result = vkQueuePresentKHR(presentQueue, &presentInfo);
				Logger_ThrowIfFailed(result, "Failed to present swap chain image", true);

				currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			}

			void Resize(const Vector2i& dimensions)
			{
				vkDeviceWaitIdle(device);

				CleanUpSwapChain();

				CreateSwapChain();
				CreateImageViews();
				CreateFramebuffers();
				CreateCommandBuffers();
			}

			bool IsInitialized() const
			{
				return isInitalized;
			}

			void AddValidationLayer(const String& layer)
			{
				validationLayers.push_back(layer.c_str());
			}

			void AddDeviceExtension(const String& extension)
			{
				deviceExtensions.push_back(extension.c_str());
			}

			void SetRenderCallback(const Function<void(VkCommandBuffer, int)>& callback)
			{
				renderCallback = callback;
			}

			VkInstance GetVulkanInstance() const
			{
				return instance;
			}

			VkDevice GetLogicalDevice() const
			{
				return device;
			}

			VkPhysicalDevice GetPhysicalDevice() const
			{
				return physicalDevice;
			}

			VkQueue GetGraphicsQueue() const
			{
				return graphicsQueue;
			}

			VkQueue GetPresentQueue() const
			{
				return presentQueue;
			}

			VkSurfaceKHR GetSurface() const
			{
				return surface;
			}

			VkSwapchainKHR GetSwapChain() const
			{
				return swapChain;
			}

			VkCommandBuffer GetCommandBuffer(Size index) const
			{
				return commandBuffers[index];
			}

			VkCommandBuffer GetCommandBuffer() const
			{
				return commandBuffers[GetCurrentFrame()];
			}

			VkRenderPass GetRenderPass() const
			{
				return renderPass;
			}

			VkExtent2D GetSwapChainExtent() const
			{
				return swapChainExtent;
			}

			VkSemaphore GetImageAvailableSemaphore(Size index) const
			{
				return imageAvailableSemaphores[index];
			}

			VkSemaphore GetRenderFinishedSemaphore(Size index) const
			{
				return renderFinishedSemaphores[index];
			}

			VkFence GetInFlightFence(Size index) const
			{
				return inFlightFences[index];
			}

			void SetCurrentFrame(Size index)
			{
				currentFrame = index;
			}

			Size GetCurrentFrame() const
			{
				return currentFrame;
			}

			void CleanUp()
			{
				vkDeviceWaitIdle(device);
				
				for (Size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
				{
					vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
					vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
					vkDestroyFence(device, inFlightFences[i], nullptr);
				}

				for (auto framebuffer : swapChainFramebuffers) 
					vkDestroyFramebuffer(device, framebuffer, nullptr);

				vkDestroyRenderPass(device, renderPass, nullptr);

				for (auto imageView : swapChainImageViews) 
					vkDestroyImageView(device, imageView, nullptr);
				
				vkDestroySwapchainKHR(device, swapChain, nullptr);

				vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

				vkDestroyCommandPool(device, commandPool, nullptr);

				vkDestroySurfaceKHR(instance, surface, nullptr);

				vkDestroyDevice(device, nullptr);

				if (enableValidationLayers) 
				{
					auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

					if (function != nullptr) 
						function(instance, debugMessenger, nullptr);
				}

				vkDestroyInstance(instance, nullptr);

				Logger_WriteConsole("Vulkan instance destroyed", LogLevel::INFORMATION);
			}

			static Shared<Renderer> GetInstance()
			{
				class EnabledRenderer : public Renderer { };

				static Shared<Renderer> instance = std::make_shared<EnabledRenderer>();

				return instance;
			}

		private:

			Renderer() = default; 

			void CreateInstance()
			{
				Vector<const char*> requiredExtensions = VulkanHelper::GetRequiredExtensions(enableValidationLayers, validationLayers);

				VkApplicationInfo applicationInformation{};

				applicationInformation.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				applicationInformation.pApplicationName = "RenderVulkan* Engine";
				applicationInformation.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
				applicationInformation.pEngineName = "RenderVulkan";
				applicationInformation.engineVersion = VK_MAKE_VERSION(0, 0, 1);
				applicationInformation.apiVersion = VK_API_VERSION_1_0;

				VkInstanceCreateInfo creationInformation{};

				creationInformation.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				creationInformation.pApplicationInfo = &applicationInformation;

				creationInformation.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

				creationInformation.enabledExtensionCount = requiredExtensions.size();
				creationInformation.ppEnabledExtensionNames = requiredExtensions.data();

				VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

				if (enableValidationLayers) 
				{
					creationInformation.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
					creationInformation.ppEnabledLayerNames = validationLayers.data();

					VulkanHelper::PopulateDebugMessengerCreateInfo(debugCreateInfo, DebugCallback);
					creationInformation.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
				}
				else 
				{
					creationInformation.enabledLayerCount = 0;

					creationInformation.pNext = nullptr;
				}

				VkResult result = vkCreateInstance(&creationInformation, nullptr, &instance);

				Logger_ThrowIfFailed(result, "Failed to create Vulkan instance", true);
				Logger_WriteConsole("Vulkan instance created", LogLevel::INFORMATION);
			}

			void SetupDebugMessenger()
			{
				if (!enableValidationLayers) 
					return;

				VkDebugUtilsMessengerCreateInfoEXT creationInformation{};
				VulkanHelper::PopulateDebugMessengerCreateInfo(creationInformation, DebugCallback);

				auto function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

				if (function != nullptr)
				{
					VkResult result = function(instance, &creationInformation, nullptr, &debugMessenger);

					Logger_ThrowIfFailed(result, "Failed to set up debug messenger", false);
				}
				else
					Logger_WriteConsole("Failed to set up debug messenger", LogLevel::ERROR);
			}

			void CreatePhysicalDevice()
			{
				uint deviceCount = 0;
				vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

				if (deviceCount <= 0)
					Logger_ThrowException("Failed to find GPU(s) with Vulkan support", true);

				Vector<VkPhysicalDevice> devices(deviceCount);
				vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

				for (const auto& device : devices) 
				{
					if (VulkanHelper::IsDeviceCompatable(device, deviceExtensions, surface)) 
					{
						physicalDevice = device;
						break;
					}
				}

				if (physicalDevice == VK_NULL_HANDLE) 
					Logger_ThrowException("failed to find a suitable GPU!", true);
				
				Logger_WriteConsole("Physical device selected", LogLevel::INFORMATION);
			}

			void CreateLogicalDevice()
			{
				QueueFamilyIndices indices = VulkanHelper::FindQueueFamilies(physicalDevice, surface);

				Vector<VkDeviceQueueCreateInfo> queueCreationInformations{};
				Set<uint> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

				float queuePriority = 1.0f;

				for (uint queueFamily : uniqueQueueFamilies) 
				{
					VkDeviceQueueCreateInfo queueCreationInformation{};

					queueCreationInformation.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreationInformation.queueFamilyIndex = queueFamily;
					queueCreationInformation.queueCount = 1;
					queueCreationInformation.pQueuePriorities = &queuePriority;

					queueCreationInformations.push_back(queueCreationInformation);
				}

				VkPhysicalDeviceFeatures deviceFeatures{};
				deviceFeatures.samplerAnisotropy = VK_TRUE;
				deviceFeatures.sampleRateShading = VK_TRUE;

				VkDeviceCreateInfo creationInformation{};

				creationInformation.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				creationInformation.queueCreateInfoCount = static_cast<uint>(queueCreationInformations.size());
				creationInformation.pQueueCreateInfos = queueCreationInformations.data();
				
				creationInformation.pEnabledFeatures = &deviceFeatures;

				creationInformation.enabledExtensionCount = static_cast<uint>(deviceExtensions.size());
				creationInformation.ppEnabledExtensionNames = deviceExtensions.data();

				if (enableValidationLayers) 
				{
					creationInformation.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
					creationInformation.ppEnabledLayerNames = validationLayers.data();
				}
				else 
					creationInformation.enabledLayerCount = 0;

				VkResult result = vkCreateDevice(physicalDevice, &creationInformation, nullptr, &device);

				Logger_ThrowIfFailed(result, "Failed to create logical device", true);

				vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
				vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

				Logger_WriteConsole("Logical device created", LogLevel::INFORMATION);
			}

			void CreateCommandPool() 
			{
				QueueFamilyIndices queueFamilyIndices = VulkanHelper::FindQueueFamilies(physicalDevice, surface);

				VkCommandPoolCreateInfo poolInformation = {};

				poolInformation.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInformation.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
				poolInformation.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

				VkResult result = vkCreateCommandPool(device, &poolInformation, nullptr, &commandPool);
				Logger_ThrowIfFailed(result, "Failed to create command pool", true);

				Logger_WriteConsole("Command pool created", LogLevel::INFORMATION);
			}

			void CreateWindowSurface()
			{
				if (glfwVulkanSupported() != GLFW_TRUE)
					Logger_ThrowException("Vulkan is not supported", true);

				GLFWwindow* window = Window::GetInstance()->GetHandle();

				VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

				Logger_ThrowIfFailed(result, "Failed to create window surface", true);

				Logger_WriteConsole("Window surface created", LogLevel::INFORMATION);
			}

			void CreateSwapChain()
			{
				SwapChainSupportDetails swapChainSupport = VulkanHelper::GetSwapChainSupport(physicalDevice, surface);

				VkSurfaceFormatKHR surfaceFormat = VulkanHelper::GetSwapSurfaceFormat(swapChainSupport.formats);
				VkPresentModeKHR presentMode = VulkanHelper::GetSwapPresentMode(swapChainSupport.presentModes);
				VkExtent2D extent = VulkanHelper::GetSwapExtent(swapChainSupport.capabilities);

				uint imageCount = swapChainSupport.capabilities.minImageCount + 1;

				if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
					imageCount = swapChainSupport.capabilities.maxImageCount;

				VkSwapchainCreateInfoKHR creationInformation{};

				creationInformation.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				creationInformation.surface = surface;
				creationInformation.minImageCount = imageCount;
				creationInformation.imageFormat = surfaceFormat.format;
				creationInformation.imageColorSpace = surfaceFormat.colorSpace;
				creationInformation.imageExtent = extent;
				creationInformation.imageArrayLayers = 1;
				creationInformation.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				QueueFamilyIndices indices = VulkanHelper::FindQueueFamilies(physicalDevice, surface);
				uint queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

				if (indices.graphicsFamily != indices.presentFamily) 
				{
					creationInformation.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					creationInformation.queueFamilyIndexCount = 2;
					creationInformation.pQueueFamilyIndices = queueFamilyIndices;
				}
				else 
				{
					creationInformation.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					creationInformation.queueFamilyIndexCount = 0;
					creationInformation.pQueueFamilyIndices = nullptr;
				}

				creationInformation.preTransform = swapChainSupport.capabilities.currentTransform;
				creationInformation.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				creationInformation.presentMode = presentMode;
				creationInformation.clipped = VK_TRUE;

				VkResult result = vkCreateSwapchainKHR(device, &creationInformation, nullptr, &swapChain);

				Logger_ThrowIfFailed(result, "Failed to create swap chain", true);

				vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);

				swapChainImages.resize(imageCount);

				vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

				swapChainImageFormat = surfaceFormat.format;

				swapChainExtent = extent;

				Settings::GetInstance()->Set<VkExtent2D>("swapChainExtent", swapChainExtent);

				Logger_WriteConsole("Swap chain created", LogLevel::INFORMATION);
			}

			void CreateImageViews()
			{
				swapChainImageViews.resize(swapChainImages.size());

				for (Size i = 0; i < swapChainImages.size(); i++)
				{
					VkImageViewCreateInfo creationInformation{};

					creationInformation.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					creationInformation.image = swapChainImages[i];
					creationInformation.viewType = VK_IMAGE_VIEW_TYPE_2D;
					creationInformation.format = swapChainImageFormat;

					creationInformation.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
					creationInformation.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
					creationInformation.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
					creationInformation.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

					creationInformation.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					creationInformation.subresourceRange.baseMipLevel = 0;
					creationInformation.subresourceRange.levelCount = 1;
					creationInformation.subresourceRange.baseArrayLayer = 0;
					creationInformation.subresourceRange.layerCount = 1;

					VkResult result = vkCreateImageView(device, &creationInformation, nullptr, &swapChainImageViews[i]);

					Logger_ThrowIfFailed(result, "Failed to create image views", true);
				}

				Logger_WriteConsole("Image views created", LogLevel::INFORMATION);
			}

			void CreateRenderPass() 
			{
				VkAttachmentDescription colorAttachment = {};

				colorAttachment.format = swapChainImageFormat;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference colorAttachmentRef = {};

				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};

				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentRef;

				VkSubpassDependency dependency = {};

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				VkRenderPassCreateInfo renderPassInformation = {};

				renderPassInformation.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInformation.attachmentCount = 1;
				renderPassInformation.pAttachments = &colorAttachment;
				renderPassInformation.subpassCount = 1;
				renderPassInformation.pSubpasses = &subpass;
				renderPassInformation.dependencyCount = 1;
				renderPassInformation.pDependencies = &dependency;

				VkResult result = vkCreateRenderPass(device, &renderPassInformation, nullptr, &renderPass);
				Logger_ThrowIfFailed(result, "Failed to create render pass", true);

				Logger_WriteConsole("Render pass created", LogLevel::INFORMATION);
			}

			void CreateFramebuffers() 
			{
				swapChainFramebuffers.resize(swapChainImageViews.size());

				for (size_t i = 0; i < swapChainImageViews.size(); i++) 
				{
					VkImageView attachments[] = { swapChainImageViews[i] };

					VkFramebufferCreateInfo framebufferInformation = {};

					framebufferInformation.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					framebufferInformation.renderPass = renderPass;
					framebufferInformation.attachmentCount = 1;
					framebufferInformation.pAttachments = attachments;
					framebufferInformation.width = swapChainExtent.width;
					framebufferInformation.height = swapChainExtent.height;
					framebufferInformation.layers = 1;

					VkResult result = vkCreateFramebuffer(device, &framebufferInformation, nullptr, &swapChainFramebuffers[i]);

					Logger_ThrowIfFailed(result, "Failed to create framebuffer", true);
				}

				Logger_WriteConsole("Framebuffers created", LogLevel::INFORMATION);
			}

			void CreateCommandBuffers() 
			{
				commandBuffers.resize(swapChainFramebuffers.size());

				VkCommandBufferAllocateInfo allocationInformation = {};

				allocationInformation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocationInformation.commandPool = commandPool;
				allocationInformation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocationInformation.commandBufferCount = (uint32_t)commandBuffers.size();

				VkResult result = vkAllocateCommandBuffers(device, &allocationInformation, commandBuffers.data());
				Logger_ThrowIfFailed(result, "Failed to allocate command buffers", true);

				Logger_WriteConsole("Command buffers allocated", LogLevel::INFORMATION);
			}

			void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint imageIndex) 
			{
				VkCommandBufferBeginInfo beginInfo = {};

				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

				VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
				Logger_ThrowIfFailed(result, "Failed to begin recording command buffer", true);

				VkRenderPassBeginInfo renderPassInfo = {};

				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChainExtent;

				VkClearValue clearColor = { 0.0f, 0.45f, 0.75f, 1.0f };

				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				renderCallback(commandBuffer, imageIndex);

				vkCmdEndRenderPass(commandBuffer);

				result = vkEndCommandBuffer(commandBuffer);
				Logger_ThrowIfFailed(result, "Failed to record command buffer", true);
			}

			void CreateSyncObjects() 
			{
				imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
				renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
				inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

				VkSemaphoreCreateInfo semaphoreInformation = {};

				semaphoreInformation.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				VkFenceCreateInfo fenceInformation = {};

				fenceInformation.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInformation.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
				{
					VkResult result = vkCreateSemaphore(device, &semaphoreInformation, nullptr, &imageAvailableSemaphores[i]);
					Logger_ThrowIfFailed(result, "Failed to create image available semaphore", true);

					result = vkCreateSemaphore(device, &semaphoreInformation, nullptr, &renderFinishedSemaphores[i]);
					Logger_ThrowIfFailed(result, "Failed to create render finished semaphore", true);

					result = vkCreateFence(device, &fenceInformation, nullptr, &inFlightFences[i]);
					Logger_ThrowIfFailed(result, "Failed to create in flight fence", true);
				}

				Logger_WriteConsole("Sync objects created", LogLevel::INFORMATION);
			}

			void CleanUpSwapChain() 
			{
				for (auto framebuffer : swapChainFramebuffers) 
					vkDestroyFramebuffer(device, framebuffer, nullptr);
				
				vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

				for (auto imageView : swapChainImageViews) 
					vkDestroyImageView(device, imageView, nullptr);
				
				vkDestroySwapchainKHR(device, swapChain, nullptr);
			}

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
			{
				if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
					Logger_WriteConsole("\n" + String(pCallbackData->pMessage), LogLevel::ERROR);
				else
					Logger_WriteConsole("\n" + String(pCallbackData->pMessage), LogLevel::DEBUGGING);

				return VK_FALSE;
			}

			VkInstance instance = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE; 
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkQueue graphicsQueue = VK_NULL_HANDLE;
			VkQueue presentQueue = VK_NULL_HANDLE;

			VkSwapchainKHR swapChain = VK_NULL_HANDLE;
			Vector<VkImage> swapChainImages = {};
			VkFormat swapChainImageFormat = VkFormat::VK_FORMAT_UNDEFINED;
			VkExtent2D swapChainExtent = {};

			VkRenderPass renderPass = VK_NULL_HANDLE;
			Vector<VkFramebuffer> swapChainFramebuffers = {};
			Vector<VkCommandBuffer> commandBuffers = {};
			
			Vector<VkSemaphore> imageAvailableSemaphores;
			Vector<VkSemaphore> renderFinishedSemaphores;
			Vector<VkFence> inFlightFences;

			VkCommandPool commandPool = VK_NULL_HANDLE;

			Size currentFrame = 0;
			const Size MAX_FRAMES_IN_FLIGHT = 2;

			Vector<VkImageView> swapChainImageViews = {};

			VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
			
			VkSurfaceKHR surface = VK_NULL_HANDLE;

			Function<void(VkCommandBuffer, int)> renderCallback;

			Vector<const char*> validationLayers = 
			{
				"VK_LAYER_KHRONOS_validation"
			};

			Vector<const char*> deviceExtensions = 
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			bool isInitalized = false;

#ifdef _DEBUG
			const bool enableValidationLayers = true;
#else
			const bool enableValidationLayers = false;
#endif
		};
	}
}