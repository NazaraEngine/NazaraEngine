// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	void OpenGLShaderBinding::Update(std::initializer_list<Binding> bindings)
	{
		StackVector<VkDescriptorBufferInfo> bufferBinding = NazaraStackVector(VkDescriptorBufferInfo, bindings.size());
		StackVector<VkDescriptorImageInfo> imageBinding = NazaraStackVector(VkDescriptorImageInfo, bindings.size());
		StackVector<VkWriteDescriptorSet> writeOps = NazaraStackVector(VkWriteDescriptorSet, bindings.size());

		for (const Binding& binding : bindings)
		{
			VkWriteDescriptorSet& writeOp = writeOps.emplace_back();
			writeOp.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeOp.dstSet = m_descriptorSet;
			writeOp.dstBinding = UInt32(binding.bindingIndex);

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, TextureBinding>)
				{
					OpenGLTexture& vkTexture = *static_cast<OpenGLTexture*>(arg.texture);
					OpenGLTextureSampler& vkSampler = *static_cast<OpenGLTextureSampler*>(arg.sampler);

					VkDescriptorImageInfo& imageInfo = imageBinding.emplace_back();
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = vkTexture.GetImageView();
					imageInfo.sampler = vkSampler.GetSampler();

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

					writeOp.pImageInfo = &imageInfo;
				}
				else if constexpr (std::is_same_v<T, UniformBufferBinding>)
				{
					OpenGLBuffer& vkBuffer = *static_cast<OpenGLBuffer*>(arg.buffer);

					VkDescriptorBufferInfo& bufferInfo = bufferBinding.emplace_back();
					bufferInfo.buffer = vkBuffer.GetBuffer();
					bufferInfo.offset = arg.offset;
					bufferInfo.range = arg.range;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

					writeOp.pBufferInfo = &bufferInfo;
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

			}, binding.content);
		}

		m_owner.GetDevice()->vkUpdateDescriptorSets(*m_owner.GetDevice(), UInt32(writeOps.size()), writeOps.data(), 0U, nullptr);
	}

	void OpenGLShaderBinding::Release()
	{
		m_owner.Release(*this);
	}
}

#endif
