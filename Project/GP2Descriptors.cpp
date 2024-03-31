#include "GP2Descriptors.h"

// std
#include <cassert>
#include <stdexcept>

namespace GP2 {

    // *************** Descriptor Set Layout Builder *********************

    GP2DescriptorSetLayout::Builder& GP2DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) 
    {
        assert(m_Bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<GP2DescriptorSetLayout> GP2DescriptorSetLayout::Builder::Build() const 
    {
        return std::make_unique<GP2DescriptorSetLayout>(m_GP2Device, m_Bindings);
    }

    // *************** Descriptor Set Layout *********************

    GP2DescriptorSetLayout::GP2DescriptorSetLayout(GP2::GP2Device& GP2Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings):
        m_GP2Device{ GP2Device },
        m_Bindings{ bindings }
        {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) 
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(GP2Device.Device(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    GP2DescriptorSetLayout::~GP2DescriptorSetLayout() 
    {
        vkDestroyDescriptorSetLayout(m_GP2Device.Device(), m_DescriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    GP2DescriptorPool::Builder& GP2DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count) 
    {
        m_PoolSizes.push_back({ descriptorType, count });
        return *this;
    }

    GP2DescriptorPool::Builder& GP2DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags) 
    {
        m_PoolFlags = flags;
        return *this;
    }
    GP2DescriptorPool::Builder& GP2DescriptorPool::Builder::SetMaxSets(uint32_t count) 
    {
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<GP2DescriptorPool> GP2DescriptorPool::Builder::Build() const 
    {
        return std::make_unique<GP2DescriptorPool>(m_GP2Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    // *************** Descriptor Pool *********************

    GP2DescriptorPool::GP2DescriptorPool(
        GP2::GP2Device& GP2Device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes): 
        m_GP2Device{ GP2Device } 
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(GP2Device.Device(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    GP2DescriptorPool::~GP2DescriptorPool() 
    {
        vkDestroyDescriptorPool(m_GP2Device.Device(), m_DescriptorPool, nullptr);
    }

    bool GP2DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_GP2Device.Device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void GP2DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        vkFreeDescriptorSets(
            m_GP2Device.Device(),
            m_DescriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void GP2DescriptorPool::ResetPool() 
    {
        vkResetDescriptorPool(m_GP2Device.Device(), m_DescriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    GP2DescriptorWriter::GP2DescriptorWriter(GP2DescriptorSetLayout& setLayout, GP2DescriptorPool& pool): 
        m_SetLayout{ setLayout },
        m_Pool{ pool } 
    {}

    GP2DescriptorWriter& GP2DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    GP2DescriptorWriter& GP2DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool GP2DescriptorWriter::Build(VkDescriptorSet& set) 
    {
        bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void GP2DescriptorWriter::Overwrite(VkDescriptorSet& set) 
    {
        for (auto& write : m_Writes) 
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_Pool.m_GP2Device.Device(), m_Writes.size(), m_Writes.data(), 0, nullptr);
    }
}