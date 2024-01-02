// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	void OpenGLShaderBinding::Apply(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 setIndex, const GL::Context& context) const
	{
		//TODO: Check layout compaitiblity
		const auto& bindingMapping = pipelineLayout.GetBindingMapping();
		const auto& layoutInfo = pipelineLayout.GetLayoutInfo();

		m_owner.ForEachDescriptor(m_poolIndex, m_bindingIndex, [&](UInt32 bindingIndex, auto&& descriptor)
		{
			using DescriptorType = std::decay_t<decltype(descriptor)>;

			auto bindingIt = std::find_if(layoutInfo.bindings.begin(), layoutInfo.bindings.end(), [&](const auto& binding)
			{
				if (binding.setIndex != setIndex)
					return false;

				assert(binding.arraySize > 0);
				if (bindingIndex < binding.bindingIndex || bindingIndex >= binding.bindingIndex + binding.arraySize)
					return false;

				return true;
			});

			if (bindingIt == layoutInfo.bindings.end())
				throw std::runtime_error("invalid binding index");

			const auto& bindingInfo = *bindingIt;

			auto bindingMappingIt = bindingMapping.find(UInt64(setIndex) << 32 | bindingIndex);
			assert(bindingMappingIt != bindingMapping.end());

			UInt32 bindingPoint = bindingMappingIt->second;

			if constexpr (std::is_same_v<DescriptorType, OpenGLRenderPipelineLayout::SampledTextureDescriptor>)
			{
				if (bindingInfo.type != ShaderBindingType::Sampler)
					throw std::runtime_error("descriptor (set=" + std::to_string(setIndex) + ", binding=" + std::to_string(bindingIndex) + ") is not a sampler");

				context.BindSampler(bindingPoint, descriptor.sampler);
				context.BindTexture(bindingPoint, descriptor.textureTarget, descriptor.texture);
			}
			else if constexpr (std::is_same_v<DescriptorType, OpenGLRenderPipelineLayout::StorageBufferDescriptor>)
			{
				if (bindingInfo.type != ShaderBindingType::StorageBuffer)
					throw std::runtime_error("descriptor (set=" + std::to_string(setIndex) + ", binding=" + std::to_string(bindingIndex) + ") is not a storage buffer");

				context.BindStorageBuffer(bindingPoint, descriptor.buffer, descriptor.offset, descriptor.size);
			}
			else if constexpr (std::is_same_v<DescriptorType, OpenGLRenderPipelineLayout::TextureDescriptor>)
			{
				if (bindingInfo.type != ShaderBindingType::Texture)
					throw std::runtime_error("descriptor (set=" + std::to_string(setIndex) + ", binding=" + std::to_string(bindingIndex) + ") is not a texture");

				context.BindImageTexture(bindingPoint, descriptor.texture, descriptor.level, descriptor.layered, descriptor.layer, descriptor.access, descriptor.format);
			}
			else if constexpr (std::is_same_v<DescriptorType, OpenGLRenderPipelineLayout::UniformBufferDescriptor>)
			{
				if (bindingInfo.type != ShaderBindingType::UniformBuffer)
					throw std::runtime_error("descriptor (set=" + std::to_string(setIndex) + ", binding=" + std::to_string(bindingIndex) + ") is not an uniform buffer");

				context.BindUniformBuffer(bindingPoint, descriptor.buffer, descriptor.offset, descriptor.size);
			}
			else
				static_assert(AlwaysFalse<DescriptorType>::value, "non-exhaustive visitor");
		});
	}

	void OpenGLShaderBinding::Update(const Binding* bindings, std::size_t bindingCount)
	{
		for (std::size_t i = 0; i < bindingCount; ++i)
		{
			const Binding& binding = bindings[i];
			
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				
				if constexpr (std::is_same_v<T, SampledTextureBinding>)
					HandleTextureBinding(binding.bindingIndex, arg);
				else if constexpr (std::is_same_v<T, SampledTextureBindings>)
				{
					for (UInt32 i = 0; i < arg.arraySize; ++i)
						HandleTextureBinding(binding.bindingIndex + i, arg.textureBindings[i]);
				}
				else if constexpr (std::is_same_v<T, StorageBufferBinding>)
				{
					auto& storageDescriptor = m_owner.GetStorageBufferDescriptor(m_poolIndex, m_bindingIndex, binding.bindingIndex);
					storageDescriptor.offset = arg.offset;
					storageDescriptor.size = arg.range;

					if (OpenGLBuffer* glBuffer = static_cast<OpenGLBuffer*>(arg.buffer))
					{
						if (glBuffer->GetType() != BufferType::Storage)
							throw std::runtime_error("expected storage buffer");

						storageDescriptor.buffer = glBuffer->GetBuffer().GetObjectId();
					}
					else
						storageDescriptor.buffer = 0;
				}
				else if constexpr (std::is_same_v<T, TextureBinding>)
				{
					auto& textureDescriptor = m_owner.GetTextureDescriptor(m_poolIndex, m_bindingIndex, binding.bindingIndex);
					if (const OpenGLTexture* glTexture = static_cast<const OpenGLTexture*>(arg.texture))
					{
						std::optional<GLTextureFormat> format = DescribeTextureFormat(glTexture->GetFormat());
						if (!format)
							throw std::runtime_error("unexpected texture format");

						textureDescriptor.access = ToOpenGL(arg.access);
						textureDescriptor.format = format->internalFormat;

						// Don't bother emulating texture views as their support is virtually guaranteed when compute shaders are available
						if (Nz::ImageType imageType = glTexture->GetType(); imageType == ImageType::Cubemap || imageType == ImageType::E1D_Array || imageType == ImageType::E2D_Array)
							textureDescriptor.layered = true;
						else
							textureDescriptor.layered = false;

						textureDescriptor.level = 0;
						textureDescriptor.layer = 0;

						textureDescriptor.texture = glTexture->GetTexture().GetObjectId();
					}
					else
					{
						textureDescriptor.access = GL_READ_ONLY;
						textureDescriptor.format = GL_RGBA8;
						textureDescriptor.layer = 0;
						textureDescriptor.layered = GL_FALSE;
						textureDescriptor.level = 0;
						textureDescriptor.texture = 0;
					}
				}
				else if constexpr (std::is_same_v<T, UniformBufferBinding>)
				{
					auto& uboDescriptor = m_owner.GetUniformBufferDescriptor(m_poolIndex, m_bindingIndex, binding.bindingIndex);
					uboDescriptor.offset = arg.offset;
					uboDescriptor.size = arg.range;

					if (OpenGLBuffer* glBuffer = static_cast<OpenGLBuffer*>(arg.buffer))
					{
						if (glBuffer->GetType() != BufferType::Uniform)
							throw std::runtime_error("expected uniform buffer");

						uboDescriptor.buffer = glBuffer->GetBuffer().GetObjectId();
					}
					else
						uboDescriptor.buffer = 0;
				}
				else
					static_assert(AlwaysFalse<T>(), "non-exhaustive visitor");

			}, binding.content);
		}
	}

	void OpenGLShaderBinding::UpdateDebugName(std::string_view /*name*/)
	{
		// No OpenGL object to name
	}

	void OpenGLShaderBinding::HandleTextureBinding(UInt32 bindingIndex, const SampledTextureBinding& textureBinding)
	{
		auto& textureDescriptor = m_owner.GetSampledTextureDescriptor(m_poolIndex, m_bindingIndex, bindingIndex);

		if (const OpenGLTexture* glTexture = static_cast<const OpenGLTexture*>(textureBinding.texture))
		{
			// TODO: Add texture view emulation

			textureDescriptor.texture = glTexture->GetTexture().GetObjectId();

			if (const OpenGLTextureSampler* glSampler = static_cast<const OpenGLTextureSampler*>(textureBinding.sampler))
				textureDescriptor.sampler = glSampler->GetSampler(glTexture->GetLevelCount() > 1).GetObjectId();
			else
				textureDescriptor.sampler = 0;

			textureDescriptor.textureTarget = OpenGLTexture::ToTextureTarget(glTexture->GetType());
		}
		else
		{
			textureDescriptor.sampler = 0;
			textureDescriptor.texture = 0;
			textureDescriptor.textureTarget = GL::TextureTarget::Target2D;
		}
	}

	void OpenGLShaderBinding::Release()
	{
		m_owner.Release(*this);
	}
}
