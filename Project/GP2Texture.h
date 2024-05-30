#pragma once
#include "GP2Device.h"

//std
#include <string>

namespace GP2
{
	class GP2Texture
	{
	public:
		GP2Texture(GP2Device& device, const std::string& filepath);
		~GP2Texture();

		GP2Texture(const GP2Texture&) = delete;
		GP2Texture& operator=(const GP2Texture&) = delete;
		GP2Texture(GP2Texture&&) = delete;
		GP2Texture& operator=(GP2Texture&&) = delete;

		VkSampler GetSampler() const { return m_Sampler; }
		VkImageView GetImageView() const { return m_ImageView; }
		VkImageLayout GetImageLayout() const { return m_ImageLayout; }

	private:
		void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		void GenerateMipmaps();

		int m_Width, m_Height, m_MipLevels;
		GP2Device& m_GP2Device;
		VkImage m_Image;
		VkDeviceMemory m_ImageMemory;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
		VkFormat m_ImageFormat;
		VkImageLayout m_ImageLayout;
	};
}