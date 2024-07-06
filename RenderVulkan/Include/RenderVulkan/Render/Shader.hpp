#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Util/FileHelper.hpp"
#include "RenderVulkan/Util/Formatter.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		class Shader
		{

		public:

			Shader(const Shader&) = delete;
			Shader& operator=(const Shader&) = delete;

            void CreatePipeline(VkRenderPass renderPass, VkExtent2D swapChainExtent)
            {
                VkDevice device = Settings::GetInstance()->Get<VkDevice>("vulkanDevice");

                VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};

                vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                vertexShaderStageInfo.module = vertexShaderModule;
                vertexShaderStageInfo.pName = "main";

                VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};

                fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                fragmentShaderStageInfo.module = fragmentShaderModule;
                fragmentShaderStageInfo.pName = "main";

                VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

                Vector<VkDynamicState> dynamicStates = 
				{ 
					VK_DYNAMIC_STATE_VIEWPORT, 
					VK_DYNAMIC_STATE_SCISSOR 
				};

				VkPipelineDynamicStateCreateInfo dynamicState{};

				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.dynamicStateCount = static_cast<uint>(dynamicStates.size());
				dynamicState.pDynamicStates = dynamicStates.data();

				VkPipelineVertexInputStateCreateInfo vertexInputInformation{};

				vertexInputInformation.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInformation.vertexBindingDescriptionCount = 0;
				vertexInputInformation.pVertexBindingDescriptions = nullptr;
				vertexInputInformation.vertexAttributeDescriptionCount = 0;
				vertexInputInformation.pVertexAttributeDescriptions = nullptr;

				VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

				inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssembly.primitiveRestartEnable = VK_FALSE;

				VkViewport viewport{};

				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)swapChainExtent.width;
				viewport.height = (float)swapChainExtent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				VkRect2D scissor{};

				scissor.offset = { 0, 0 };
				scissor.extent = swapChainExtent;
            }

			String GetName() const
			{
				return name;
			}

			String GetLocalPath() const
			{
				return localPath;
			}

			String GetDomain() const
			{
				return domain;
			}

			Pair<String, String> GetPaths() const
			{
				return { vertexPath, fragmentPath };
			}
			
			Pair<VkShaderModule, VkShaderModule> GetModules()
			{
				return { vertexShaderModule, fragmentShaderModule };
			}

			void CleanUp() const
			{
				if (vertexShaderModule != VK_NULL_HANDLE)
					vkDestroyShaderModule(Settings::GetInstance()->Get<VkDevice>("vulkanDevice"), vertexShaderModule, nullptr);
				
				if (fragmentShaderModule != VK_NULL_HANDLE) 
					vkDestroyShaderModule(Settings::GetInstance()->Get<VkDevice>("vulkanDevice"), fragmentShaderModule, nullptr);

                if (pipeline != VK_NULL_HANDLE)
                    vkDestroyPipeline(Settings::GetInstance()->Get<VkDevice>("vulkanDevice"), pipeline, nullptr);

                if (pipelineLayout != VK_NULL_HANDLE)
                    vkDestroyPipelineLayout(Settings::GetInstance()->Get<VkDevice>("vulkanDevice"), pipelineLayout, nullptr);
			}

			static Shared<Shader> Create(const String& localPath, const String& name, const String& domain = Settings::GetInstance()->Get<String>("defaultDomain"))
			{
				Shared<Shader> shader = std::make_shared<Shader>();

				shader->name = name;
				shader->localPath = localPath;
				shader->domain = domain;
				shader->vertexPath = Formatter::Format("Assets/{}/{}Vertex.spv");
				shader->fragmentPath = Formatter::Format("Assets/{}/{}Fragment.spv");
				shader->vertexData = FileHelper::ReadFileIntoVector(shader->vertexPath);
				shader->fragmentData = FileHelper::ReadFileIntoVector(shader->fragmentPath);

				shader->Generate();

				return shader;
			}

		private:

			Shader() = default;

			void Generate()
			{
				vertexShaderModule = CreateShaderModule(vertexData);
				fragmentShaderModule = CreateShaderModule(fragmentData);
			}

			VkShaderModule CreateShaderModule(const Vector<char>& code)
			{
				VkShaderModuleCreateInfo creationInformation{};

				creationInformation.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				creationInformation.codeSize = code.size();
				creationInformation.pCode = reinterpret_cast<const uint*>(code.data());

				VkShaderModule shaderModule;

				if (vkCreateShaderModule(Settings::GetInstance()->Get<VkDevice>("vulkanDevice"), &creationInformation, nullptr, &shaderModule) != VK_SUCCESS)
					Logger_ThrowException("Failed to create shader module", false);
				
				return shaderModule;
			}

			String name;
			String localPath;
			String domain;

			String vertexPath;
			Vector<char> vertexData;

			String fragmentPath;
			Vector<char> fragmentData;

			VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
			VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

            VkPipeline pipeline = VK_NULL_HANDLE;
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		};
	}
}