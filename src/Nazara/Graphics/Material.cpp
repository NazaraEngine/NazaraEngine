// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Material
	* \brief Graphics class that represents a material
	*/

	/*!
	* \brief Constructs a Material object with default states
	*
	* \see Reset
	*/
	Material::Material(std::shared_ptr<const MaterialSettings> settings) :
	m_settings(std::move(settings)),
	m_enabledConditions(0),
	m_pipelineUpdated(false),
	m_shadowCastingEnabled(true)
	{
		m_pipelineInfo.settings = m_settings;

		const auto& shaders = m_settings->GetShaders();
		for (std::size_t i = 0; i < ShaderStageTypeCount; ++i)
			m_pipelineInfo.shaders[i].uberShader = shaders[i];

		m_textures.resize(m_settings->GetTextures().size());

		m_uniformBuffers.reserve(m_settings->GetUniformBlocks().size());
		for (const auto& uniformBufferInfo : m_settings->GetUniformBlocks())
		{
			auto& uniformBuffer = m_uniformBuffers.emplace_back();

			uniformBuffer.buffer = Graphics::Instance()->GetRenderDevice().InstantiateBuffer(Nz::BufferType_Uniform);
			if (!uniformBuffer.buffer->Initialize(uniformBufferInfo.blockSize, BufferUsage_Dynamic))
				throw std::runtime_error("failed to initialize UBO memory");

			assert(uniformBufferInfo.defaultValues.size() <= uniformBufferInfo.blockSize);

			uniformBuffer.buffer->Fill(uniformBufferInfo.defaultValues.data(), 0, uniformBufferInfo.defaultValues.size());
			uniformBuffer.data.resize(uniformBufferInfo.blockSize);
			std::memcpy(uniformBuffer.data.data(), uniformBufferInfo.defaultValues.data(), uniformBufferInfo.defaultValues.size());
		}
	}

	void Material::UpdateShaderBinding(ShaderBinding& shaderBinding) const
	{
		// TODO: Use StackVector
		std::vector<ShaderBinding::Binding> bindings;


		std::size_t bindingIndex = 0;

		for (const auto& textureSlot : m_textures)
		{
			if (!textureSlot.sampler)
			{
				TextureSamplerCache& samplerCache = Graphics::Instance()->GetSamplerCache();
				textureSlot.sampler = samplerCache.Get(textureSlot.samplerInfo);
			}

			//TODO: Use "missing" texture
			if (textureSlot.texture)
			{
				bindings.push_back({
					bindingIndex,
					ShaderBinding::TextureBinding {
						textureSlot.texture.get(), textureSlot.sampler.get()
					}
				});
			}

			bindingIndex++;
		}

		for (const auto& ubo : m_uniformBuffers)
		{
			bindings.push_back({
				bindingIndex++,
				ShaderBinding::UniformBufferBinding {
					ubo.buffer.get(), 0, ubo.buffer->GetSize()
				}
			});
		}

		shaderBinding.Update(bindings.data(), bindings.size());
	}
}
