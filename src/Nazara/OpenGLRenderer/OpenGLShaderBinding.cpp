// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
		const auto& layoutInfo = m_owner.GetLayoutInfo();

		for (const Binding& binding : bindings)
		{
			assert(binding.bindingIndex < layoutInfo.bindings.size());
			const auto& bindingDesc = layoutInfo.bindings[binding.bindingIndex];

			std::size_t resourceIndex = 0;
			for (std::size_t i = binding.bindingIndex; i > 0; --i)
			{
				// Use i-1 to prevent underflow in for loop
				if (layoutInfo.bindings[i - 1].type == bindingDesc.type)
					resourceIndex++;
			}

			switch (bindingDesc.type)
			{
				case ShaderBindingType::Texture:
				{
					if (!std::holds_alternative<TextureBinding>(binding.content))
						throw std::runtime_error("binding #" + std::to_string(binding.bindingIndex) + " is a texture but another binding type has been supplied");

					const TextureBinding& texBinding = std::get<TextureBinding>(binding.content);

					OpenGLTexture& glTexture = *static_cast<OpenGLTexture*>(texBinding.texture);
					OpenGLTextureSampler& glSampler = *static_cast<OpenGLTextureSampler*>(texBinding.sampler);

					auto& textureDescriptor = m_owner.GetTextureDescriptor(m_poolIndex, m_bindingIndex, resourceIndex);
					textureDescriptor.texture = glTexture.GetTexture().GetObjectId();
					textureDescriptor.sampler = glSampler.GetSampler(glTexture.GetLevelCount() > 1).GetObjectId();
					break;
				}

				case ShaderBindingType::UniformBuffer:
				{
					if (!std::holds_alternative<UniformBufferBinding>(binding.content))
						throw std::runtime_error("binding #" + std::to_string(binding.bindingIndex) + " is an uniform buffer but another binding type has been supplied");

					const UniformBufferBinding& uboBinding = std::get<UniformBufferBinding>(binding.content);

					OpenGLBuffer& glBuffer = *static_cast<OpenGLBuffer*>(uboBinding.buffer);
					if (glBuffer.GetType() != BufferType_Uniform)
						throw std::runtime_error("expected uniform buffer");

					auto& uboDescriptor = m_owner.GetUniformBufferDescriptor(m_poolIndex, m_bindingIndex, resourceIndex);
					uboDescriptor.buffer = glBuffer.GetBuffer().GetObjectId();
					uboDescriptor.offset = uboBinding.offset;
					uboDescriptor.size = uboBinding.range;
					break;
				}
			}
		}
	}

	void OpenGLShaderBinding::Release()
	{
		m_owner.Release(*this);
	}
}
