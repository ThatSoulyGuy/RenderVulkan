#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Math/Transform.hpp"
#include "RenderVulkan/Render/Shader.hpp"
#include "RenderVulkan/Render/Vertex.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"
#include "RenderVulkan/Util/VulkanHelper.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Math;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		struct DefaultMatrixBuffer
		{
			Matrix4x4f worldMatrix;
		};

		class Mesh
		{

		public:

			void Generate()
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

				VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
				VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;

				CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
				void* data;
				vkMapMemory(device, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
				memcpy(data, vertices.data(), (size_t)vertexBufferSize);
				vkUnmapMemory(device, stagingBufferMemory);

				CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
				CopyBuffer(stagingBuffer, vertexBuffer, vertexBufferSize);

				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
				vkMapMemory(device, stagingBufferMemory, 0, indexBufferSize, 0, &data);
				memcpy(data, indices.data(), (size_t)indexBufferSize);
				vkUnmapMemory(device, stagingBufferMemory);

				CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
				CopyBuffer(stagingBuffer, indexBuffer, indexBufferSize);

				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				shader->CreateConstantBuffer<DefaultMatrixBuffer>();
			}

			String GetName() const
			{
				return name;
			}

			Pair<Vector<Vertex>, Vector<uint>> GetData() const
			{
				return { vertices, indices };
			}

			void Render(VkCommandBuffer commandBuffer)
			{
				shader->Bind(commandBuffer);

				VkExtent2D swapChainExtent = Settings::GetInstance()->Get<VkExtent2D>("swapChainExtent");

				VkViewport viewport{};

				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = static_cast<float>(swapChainExtent.width);
				viewport.height = static_cast<float>(swapChainExtent.height);
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

				VkRect2D scissor{};

				scissor.offset = { 0, 0 };
				scissor.extent = swapChainExtent;

				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

				VkBuffer vertexBuffers[] = { vertexBuffer };
				VkDeviceSize offsets[] = { 0 };

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

				shader->UpdateConstantBuffer(DefaultMatrixBuffer
				{ 
					glm::transpose(transform->GetWorldMatrix()) 
				});

				const auto& descriptorSets = shader->GetDescriptorManager()->GetDescriptorSets();
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, static_cast<uint>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, static_cast<uint>(indices.size()), 1, 0, 0, 0);
			}

			void CleanUp()
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

				vkDeviceWaitIdle(device);

				if (indexBuffer != VK_NULL_HANDLE)
				{
					vkDestroyBuffer(device, indexBuffer, nullptr);
					indexBuffer = VK_NULL_HANDLE;
				}

				if (indexBufferMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(device, indexBufferMemory, nullptr);
					indexBufferMemory = VK_NULL_HANDLE;
				}

				if (vertexBuffer != VK_NULL_HANDLE)
				{
					vkDestroyBuffer(device, vertexBuffer, nullptr);
					vertexBuffer = VK_NULL_HANDLE;
				}

				if (vertexBufferMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(device, vertexBufferMemory, nullptr);
					vertexBufferMemory = VK_NULL_HANDLE;
				}
			}

			static Shared<Mesh> Create(String name, Shared<Shader> shader, Vector<Vertex> vertices, Vector<uint> indices)
			{
				class EnabledMesh : public Mesh { };
				
				Shared<Mesh> mesh = std::make_shared<EnabledMesh>();

				mesh->name = name;
				mesh->shader = shader;
				mesh->transform = Transform::Create();
				mesh->vertices = vertices;
				mesh->indices = indices;

				return mesh;
			}

			Shared<Transform> transform;

		private:

			Mesh() = default;

			void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

				VkBufferCreateInfo bufferInfo{};

				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = usage;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
				Logger_ThrowIfFailed(result, "Failed to create buffer!", false);

				VkMemoryRequirements memRequirements;

				vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

				VkMemoryAllocateInfo allocInfo{};

				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = VulkanHelper::FindMemoryType(memRequirements.memoryTypeBits, properties);

				result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
				Logger_ThrowIfFailed(result, "Failed to allocate buffer memory!", false);

				vkBindBufferMemory(device, buffer, bufferMemory, 0);
			}

			void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
			{
				VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

				VkBufferCopy copyRegion{};

				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;
				copyRegion.size = size;

				vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

				EndSingleTimeCommands(commandBuffer);
			}

			VkCommandBuffer BeginSingleTimeCommands()
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");
				VkCommandPool commandPool = Settings::GetInstance()->GetPointer<VkCommandPool>("commandPool");

				VkCommandBufferAllocateInfo allocationInformation{};

				allocationInformation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocationInformation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocationInformation.commandPool = commandPool;
				allocationInformation.commandBufferCount = 1;

				VkCommandBuffer commandBuffer;

				vkAllocateCommandBuffers(device, &allocationInformation, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(commandBuffer, &beginInfo);

				return commandBuffer;
			}

			void EndSingleTimeCommands(VkCommandBuffer commandBuffer)
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");
				VkQueue graphicsQueue = Settings::GetInstance()->GetPointer<VkQueue>("graphicsQueue");
				VkCommandPool commandPool = Settings::GetInstance()->GetPointer<VkCommandPool>("commandPool");

				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(graphicsQueue);

				vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
			}

			String name;

			Shared<Shader> shader;
			
			Vector<Vertex> vertices;
			Vector<uint> indices;

			VkBuffer vertexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
			VkBuffer indexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
		};
	}
}