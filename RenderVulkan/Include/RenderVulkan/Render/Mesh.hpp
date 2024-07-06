#pragma once

#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Render/Shader.hpp"
#include "RenderVulkan/Render/Vertex.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"
#include "RenderVulkan/Util/VulkanHelper.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Render
	{
		class Mesh
		{

		public:

			void Generate()
			{
				VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

				VkBufferCreateInfo bufferInformation{};

				bufferInformation.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInformation.size = sizeof(vertices[0]) * vertices.size();
				bufferInformation.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				bufferInformation.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				VkResult result = vkCreateBuffer(device, &bufferInformation, nullptr, &vertexBuffer);
				
				Logger_ThrowIfFailed(result, "Failed to create vertex buffer!", false);
				
				VkMemoryRequirements memoryRequirements;
				vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

				VkMemoryAllocateInfo allocationInformation{};

				allocationInformation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocationInformation.allocationSize = memoryRequirements.size;
				allocationInformation.memoryTypeIndex = VulkanHelper::FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				result = vkAllocateMemory(device, &allocationInformation, nullptr, &vertexBufferMemory);
				
				Logger_ThrowIfFailed(result, "Failed to allocate vertex buffer memory!", false);
			

				vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

				void* data;

				vkMapMemory(device, vertexBufferMemory, 0, bufferInformation.size, 0, &data);

				memcpy(data, vertices.data(), (size_t)bufferInformation.size);

				vkUnmapMemory(device, vertexBufferMemory);


				bufferInformation.size = sizeof(indices[0]) * indices.size();
				bufferInformation.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

				result = vkCreateBuffer(device, &bufferInformation, nullptr, &indexBuffer);
				
				Logger_ThrowIfFailed(result, "Failed to create index buffer!", false);
				
				vkGetBufferMemoryRequirements(device, indexBuffer, &memoryRequirements);


				allocationInformation.allocationSize = memoryRequirements.size;
				allocationInformation.memoryTypeIndex = VulkanHelper::FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				result = vkAllocateMemory(device, &allocationInformation, nullptr, &indexBufferMemory);
				
				Logger_ThrowIfFailed(result, "Failed to allocate index buffer memory!", false);

				vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);

				vkMapMemory(device, indexBufferMemory, 0, bufferInformation.size, 0, &data);

				memcpy(data, indices.data(), (size_t)bufferInformation.size);

				vkUnmapMemory(device, indexBufferMemory);
			}

			void Render(VkCommandBuffer commandBuffer)
			{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());

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
				mesh->vertices = vertices;
				mesh->indices = indices;

				return mesh;
			}

		private:

			Mesh() = default;

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