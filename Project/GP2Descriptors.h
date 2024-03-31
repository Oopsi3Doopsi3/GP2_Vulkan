#pragma once

#include "GP2Device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace GP2
{
    class GP2DescriptorSetLayout {
    public:
        class Builder 
        {
        public:
            Builder(GP2Device& GP2Device): 
                m_GP2Device{ GP2Device }
            {}

            Builder& AddBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<GP2DescriptorSetLayout> Build() const;

        private:
            GP2Device& m_GP2Device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
        };

        GP2DescriptorSetLayout(GP2Device& GP2Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~GP2DescriptorSetLayout();
        GP2DescriptorSetLayout(const GP2DescriptorSetLayout&) = delete;
        GP2DescriptorSetLayout& operator=(const GP2DescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

    private:
        GP2Device& m_GP2Device;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

        friend class GP2DescriptorWriter;
    };

    class GP2DescriptorPool {
    public:
        class Builder {
        public:
            Builder(GP2Device& GP2Device): 
                m_GP2Device{ GP2Device } 
            {}

            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<GP2DescriptorPool> Build() const;

        private:
            GP2Device& m_GP2Device;
            std::vector<VkDescriptorPoolSize> m_PoolSizes{};
            uint32_t m_MaxSets = 1000;
            VkDescriptorPoolCreateFlags m_PoolFlags = 0;
        };

        GP2DescriptorPool(
            GP2Device& GP2Device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~GP2DescriptorPool();
        GP2DescriptorPool(const GP2DescriptorPool&) = delete;
        GP2DescriptorPool& operator=(const GP2DescriptorPool&) = delete;

        bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool();

    private:
        GP2Device& m_GP2Device;
        VkDescriptorPool m_DescriptorPool;

        friend class GP2DescriptorWriter;
    };

    class GP2DescriptorWriter {
    public:
        GP2DescriptorWriter(GP2DescriptorSetLayout& setLayout, GP2DescriptorPool& pool);

        GP2DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        GP2DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set);
        void Overwrite(VkDescriptorSet& set);

    private:
        GP2DescriptorSetLayout& m_SetLayout;
        GP2DescriptorPool& m_Pool;
        std::vector<VkWriteDescriptorSet> m_Writes;
    };
}