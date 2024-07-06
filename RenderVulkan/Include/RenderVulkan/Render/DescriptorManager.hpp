#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Core/Logger.hpp"
#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::Core;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
    namespace Render
    {
        class DescriptorManager
        {

        public:

            void CreateDescriptorSetLayout(const Vector<VkDescriptorSetLayoutBinding>& bindings)
            {
                VkDescriptorSetLayoutCreateInfo layoutInformation{};

                layoutInformation.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInformation.bindingCount = static_cast<uint32_t>(bindings.size());
                layoutInformation.pBindings = bindings.data();

                VkResult result = vkCreateDescriptorSetLayout(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), &layoutInformation, nullptr, &descriptorSetLayout);
                Logger_ThrowIfFailed(result, "Failed to create descriptor set layout", false);
            }

            void CreateDescriptorPool(const Vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
            {
                VkDescriptorPoolCreateInfo poolInformation{};

                poolInformation.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolInformation.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
                poolInformation.pPoolSizes = poolSizes.data();
                poolInformation.maxSets = maxSets;

                VkResult result = vkCreateDescriptorPool(Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice"), &poolInformation, nullptr, &descriptorPool);
                Logger_ThrowIfFailed(result, "Failed to create descriptor pool", false);
            }

            void CreateDescriptorSets(const Vector<VkBuffer>& buffers, VkDeviceSize bufferSize)
            {
                VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

                Vector<VkDescriptorSetLayout> layouts(maxSets, descriptorSetLayout);

                VkDescriptorSetAllocateInfo allocInfo{};

                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = descriptorPool;
                allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
                allocInfo.pSetLayouts = layouts.data();

                descriptorSets.resize(layouts.size());

                VkResult result = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());

                Logger_ThrowIfFailed(result, "Failed to allocate descriptor sets", false);

                for (Size b = 0; b < buffers.size(); b++)
                {
                    VkDescriptorBufferInfo bufferInfo{};

                    bufferInfo.buffer = buffers[b];
                    bufferInfo.offset = 0;
                    bufferInfo.range = bufferSize;

                    VkWriteDescriptorSet descriptorWrite{};

                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = descriptorSets[b];
                    descriptorWrite.dstBinding = 0;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfo;

                    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
                }
            }

            VkDescriptorSetLayout GetDescriptorSetLayout() const 
            { 
                return descriptorSetLayout; 
            }

            VkDescriptorPool GetDescriptorPool() const 
            { 
                return descriptorPool;
            }

            const Vector<VkDescriptorSet>& GetDescriptorSets() const 
            { 
                return descriptorSets; 
            }

            void CleanUp()
            {
                VkDevice device = Settings::GetInstance()->GetPointer<VkDevice>("logicalDevice");

                if (descriptorPool != VK_NULL_HANDLE)
                    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
                
                if (descriptorSetLayout != VK_NULL_HANDLE)
                    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

                descriptorSets.clear();
            }

            static Unique<DescriptorManager> Create()
			{
                class EnabledDescriptorManager : public DescriptorManager { };
				return std::make_unique<EnabledDescriptorManager>();
			}

        private:

            DescriptorManager() = default;

            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            Vector<VkDescriptorSet> descriptorSets;

            uint maxSets = 1;

        };
    }
}