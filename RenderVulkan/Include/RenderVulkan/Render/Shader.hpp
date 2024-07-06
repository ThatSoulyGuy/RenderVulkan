#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Render/DescriptorManager.hpp"
#include "RenderVulkan/Render/Vertex.hpp"
#include "RenderVulkan/Util/FileHelper.hpp"
#include "RenderVulkan/Util/Formatter.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"
#include "RenderVulkan/Util/VulkanHelper.hpp"

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

            void CreatePipeline(VkRenderPass renderPass)
            {
                VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

                VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};

                vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                vertexShaderStageInfo.module = vertexShaderModule;
                vertexShaderStageInfo.pName = "Main";

                VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};

                fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                fragmentShaderStageInfo.module = fragmentShaderModule;
                fragmentShaderStageInfo.pName = "Main";

                VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

                Vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

                VkPipelineDynamicStateCreateInfo dynamicState{};

                dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
                dynamicState.pDynamicStates = dynamicStates.data();

                VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
                Array<VkVertexInputAttributeDescription, 4> attributeDescriptions = Vertex::GetAttributeDescriptions();

                VkPipelineVertexInputStateCreateInfo vertexInputInformation{};

                vertexInputInformation.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInformation.vertexBindingDescriptionCount = 1;
                vertexInputInformation.pVertexBindingDescriptions = &bindingDescription;
                vertexInputInformation.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
                vertexInputInformation.pVertexAttributeDescriptions = attributeDescriptions.data();

                VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

                inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                inputAssembly.primitiveRestartEnable = VK_FALSE;

                VkExtent2D swapChainExtent = Settings::GetInstance()->Get<VkExtent2D>("swapChainExtent");

                VkViewport viewport{};

                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(swapChainExtent.width);
                viewport.height = static_cast<float>(swapChainExtent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor{};

                scissor.offset = { 0, 0 };
                scissor.extent = swapChainExtent;

                VkPipelineViewportStateCreateInfo viewportState{};

                viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportState.viewportCount = 1;
                viewportState.pViewports = &viewport;
                viewportState.scissorCount = 1;
                viewportState.pScissors = &scissor;

                VkPipelineRasterizationStateCreateInfo rasterizer{};

                rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizer.depthClampEnable = VK_FALSE;
                rasterizer.rasterizerDiscardEnable = VK_FALSE;
                rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
                rasterizer.lineWidth = 1.0f;
                rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
                rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
                rasterizer.depthBiasEnable = VK_FALSE;
                rasterizer.depthBiasConstantFactor = 0.0f;
                rasterizer.depthBiasClamp = 0.0f;
                rasterizer.depthBiasSlopeFactor = 0.0f;

                VkPipelineMultisampleStateCreateInfo multisampling{};

                multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisampling.sampleShadingEnable = VK_FALSE;
                multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
                multisampling.minSampleShading = 1.0f;
                multisampling.pSampleMask = nullptr;
                multisampling.alphaToCoverageEnable = VK_FALSE;
                multisampling.alphaToOneEnable = VK_FALSE;

                VkPipelineColorBlendAttachmentState colorBlendAttachment{};

                colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

                VkPipelineColorBlendStateCreateInfo colorBlending{};

                colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlending.logicOpEnable = VK_FALSE;
                colorBlending.logicOp = VK_LOGIC_OP_COPY;
                colorBlending.attachmentCount = 1;
                colorBlending.pAttachments = &colorBlendAttachment;
                colorBlending.blendConstants[0] = 0.0f;
                colorBlending.blendConstants[1] = 0.0f;
                colorBlending.blendConstants[2] = 0.0f;
                colorBlending.blendConstants[3] = 0.0f;

                VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

                VkDescriptorSetLayout layout = descriptorManager->GetDescriptorSetLayout();

                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount = 1;
                pipelineLayoutInfo.pSetLayouts = &layout;
                pipelineLayoutInfo.pushConstantRangeCount = 0;
                pipelineLayoutInfo.pPushConstantRanges = nullptr;

                VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
                Logger_ThrowIfFailed(result, "Failed to create pipeline layout", false);

                VkGraphicsPipelineCreateInfo pipelineInfo{};

                pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount = 2;
                pipelineInfo.pStages = shaderStages;
                pipelineInfo.pVertexInputState = &vertexInputInformation;
                pipelineInfo.pInputAssemblyState = &inputAssembly;
                pipelineInfo.pViewportState = &viewportState;
                pipelineInfo.pRasterizationState = &rasterizer;
                pipelineInfo.pMultisampleState = &multisampling;
                pipelineInfo.pDepthStencilState = nullptr;
                pipelineInfo.pColorBlendState = &colorBlending;
                pipelineInfo.pDynamicState = &dynamicState;
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = renderPass;
                pipelineInfo.subpass = 0;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
                pipelineInfo.basePipelineIndex = -1;

                result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
                Logger_ThrowIfFailed(result, "Failed to create graphics pipeline", false);
            }

            void Bind(VkCommandBuffer commandBuffer)
			{
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
                const auto& descriptorSets = descriptorManager->GetDescriptorSets();
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			}

            template<typename T>
            void CreateConstantBuffer()
            {
                VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

                VkDeviceSize bufferSize = sizeof(T);

                VkBufferCreateInfo bufferInformation{};

                bufferInformation.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInformation.size = bufferSize;
                bufferInformation.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferInformation.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                VkResult result = vkCreateBuffer(device, &bufferInformation, nullptr, &constantBuffer);
                Logger_ThrowIfFailed(result, "Failed to create constant buffer!", false);

                VkMemoryRequirements memoryRequirements;
                vkGetBufferMemoryRequirements(device, constantBuffer, &memoryRequirements);

                VkMemoryAllocateInfo allocationInformation{};

                allocationInformation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocationInformation.allocationSize = memoryRequirements.size;
                allocationInformation.memoryTypeIndex = VulkanHelper::FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                result = vkAllocateMemory(device, &allocationInformation, nullptr, &constantBufferMemory);
                Logger_ThrowIfFailed(result, "Failed to allocate constant buffer memory!", false);

                vkBindBufferMemory(device, constantBuffer, constantBufferMemory, 0);

                Vector<VkBuffer> buffers = { constantBuffer };
                descriptorManager->CreateDescriptorSets(buffers, bufferSize);
            }

            template<typename T>
            void UpdateConstantBuffer(const T& data)
            {
                void* mappedData;

                vkMapMemory(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), constantBufferMemory, 0, sizeof(T), 0, &mappedData);

                memcpy(mappedData, &data, sizeof(T));

                vkUnmapMemory(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), constantBufferMemory);
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

            Unique<DescriptorManager>& GetDescriptorManager()
			{
				return descriptorManager;
			}

			Pair<String, String> GetPaths() const
			{
				return { vertexPath, fragmentPath };
			}
			
			Pair<VkShaderModule, VkShaderModule> GetModules()
			{
				return { vertexShaderModule, fragmentShaderModule };
			}

			VkPipeline GetPipeline() const
			{
				return pipeline;
			}

			VkPipelineLayout GetPipelineLayout() const
			{
				return pipelineLayout;
			}

			void CleanUp() const
			{
				vkDeviceWaitIdle(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"));

				if (vertexShaderModule != VK_NULL_HANDLE)
					vkDestroyShaderModule(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), vertexShaderModule, nullptr);
				
				if (fragmentShaderModule != VK_NULL_HANDLE) 
					vkDestroyShaderModule(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), fragmentShaderModule, nullptr);

                if (pipeline != VK_NULL_HANDLE)
                    vkDestroyPipeline(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), pipeline, nullptr);

                if (pipelineLayout != VK_NULL_HANDLE)
                    vkDestroyPipelineLayout(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), pipelineLayout, nullptr);

                if (constantBuffer != VK_NULL_HANDLE)
					vkDestroyBuffer(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), constantBuffer, nullptr);

                if (constantBufferMemory != VK_NULL_HANDLE)
                    vkFreeMemory(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), constantBufferMemory, nullptr);

                if (descriptorManager)
                    descriptorManager->CleanUp();
			}

			static Shared<Shader> Create(const String& localPath, const String& name, Unique<DescriptorManager> descriptorManager, const String& domain = Settings::GetInstance()->Get<String>("defaultDomain"))
			{
				class EnabledShader : public Shader { };
				Shared<Shader> shader = std::make_shared<EnabledShader>();

				shader->name = name;
				shader->localPath = localPath;
				shader->domain = domain;
				shader->vertexPath = Formatter::Format("Assets/{}/{}Vertex.spv", domain, localPath);
				shader->fragmentPath = Formatter::Format("Assets/{}/{}Fragment.spv", domain, localPath);
				shader->vertexData = FileHelper::ReadFileIntoVector(shader->vertexPath);
				shader->fragmentData = FileHelper::ReadFileIntoVector(shader->fragmentPath);
                shader->descriptorManager = std::move(descriptorManager);

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
				
				if (vkCreateShaderModule(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), &creationInformation, nullptr, &shaderModule) != VK_SUCCESS)
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

            Unique<DescriptorManager> descriptorManager = DescriptorManager::Create();

            VkBuffer constantBuffer = VK_NULL_HANDLE;
            VkDeviceMemory constantBufferMemory = VK_NULL_HANDLE;
		};
	}
}