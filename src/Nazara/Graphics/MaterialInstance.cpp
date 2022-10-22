// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	MaterialInstance::MaterialInstance(std::shared_ptr<Material> parent) :
	m_parent(std::move(parent)),
	m_materialSettings(m_parent->GetSettings())
	{
		const auto& settings = m_parent->GetSettings();
		m_textureOverride.resize(settings.GetTexturePropertyCount());
		m_valueOverride.resize(settings.GetTexturePropertyCount());

		m_uniformBuffers.resize(m_parent->GetUniformBlockCount());
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uniformBlockData = m_parent->GetUniformBlockData(i);

			auto& uniformBuffer = m_uniformBuffers[i];
			uniformBuffer.bufferView = uniformBlockData.bufferPool->Allocate(uniformBuffer.bufferIndex);
			uniformBuffer.values.resize(uniformBlockData.bufferPool->GetBufferSize());
		}
	}

	MaterialInstance::~MaterialInstance()
	{
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			auto& uniformBuffer = m_uniformBuffers[i];
			m_parent->GetUniformBlockData(i).bufferPool->Free(uniformBuffer.bufferIndex);
		}
	}

	const std::shared_ptr<MaterialPass>& MaterialInstance::GetPass(std::size_t passIndex) const
	{
		return m_parent->GetPass(passIndex);
	}

	void MaterialInstance::OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder)
	{
		UploadPool& uploadPool = renderFrame.GetUploadPool();
		for (UniformBuffer& uniformBuffer : m_uniformBuffers)
		{
			if (!uniformBuffer.dataInvalidated)
				continue;

			auto& allocation = uploadPool.Allocate(uniformBuffer.values.size());
			std::memcpy(allocation.mappedPtr, uniformBuffer.values.data(), uniformBuffer.values.size());

			builder.CopyBuffer(allocation, uniformBuffer.bufferView);

			uniformBuffer.dataInvalidated = false;
		}
	}
}
