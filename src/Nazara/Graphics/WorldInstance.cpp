// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/StackVector.hpp>

namespace Nz
{
	WorldInstance::WorldInstance() :
	m_invWorldMatrix(Matrix4f::Identity()),
	m_worldMatrix(Matrix4f::Identity()),
	m_dataInvalided(true)
	{
		constexpr auto& instanceUboOffsets = PredefinedInstanceOffsets;

		m_instanceDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, instanceUboOffsets.totalSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
		m_instanceDataBuffer->UpdateDebugName("Instance data");
	}

	void WorldInstance::OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder)
	{
		if (!m_dataInvalided)
			return;

		constexpr auto& instanceUboOffsets = PredefinedInstanceOffsets;

		auto& allocation = renderResources.GetUploadPool().Allocate(m_instanceDataBuffer->GetSize());
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.worldMatrixOffset) = m_worldMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.invWorldMatrixOffset) = m_invWorldMatrix;

		builder.CopyBuffer(allocation, m_instanceDataBuffer.get());

		m_dataInvalided = false;
	}
}
