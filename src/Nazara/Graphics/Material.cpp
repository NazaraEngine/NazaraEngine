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
	m_pipelineUpdated(false),
	m_shadowCastingEnabled(true)
	{
		m_pipelineInfo.settings = m_settings;
		m_pipelineInfo.shaders = m_settings->GetDefaultShaders();

		m_textures.resize(m_settings->GetTextures().size());

		m_uniformBuffers.reserve(m_settings->GetUniformBlocks().size());
		for (const auto& uniformBufferInfo : m_settings->GetUniformBlocks())
		{
			//TODO: Use pools
			UniformBufferRef ubo = UniformBuffer::New(static_cast<UInt32>(uniformBufferInfo.blockSize), DataStorage_Hardware, BufferUsage_Dynamic);
			ubo->Fill(uniformBufferInfo.defaultValues.data(), 0, uniformBufferInfo.defaultValues.size());

			m_uniformBuffers.emplace_back(std::move(ubo));
		}
	}
}
