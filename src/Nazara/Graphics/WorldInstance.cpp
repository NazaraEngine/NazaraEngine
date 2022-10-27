// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utils/StackVector.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	WorldInstance::WorldInstance() :
	m_invWorldMatrix(Matrix4f::Identity()),
	m_worldMatrix(Matrix4f::Identity()),
	m_dataInvalided(true)
	{
		PredefinedInstanceData instanceUboOffsets = PredefinedInstanceData::GetOffsets();

		m_instanceDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, instanceUboOffsets.totalSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
	}

	void WorldInstance::OnTransfer(RenderFrame& renderFrame, CommandBufferBuilder& builder)
	{
		if (!m_dataInvalided)
			return;

		PredefinedInstanceData instanceUboOffsets = PredefinedInstanceData::GetOffsets();

		auto& allocation = renderFrame.GetUploadPool().Allocate(m_instanceDataBuffer->GetSize());
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.worldMatrixOffset) = m_worldMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.invWorldMatrixOffset) = m_invWorldMatrix;

		builder.CopyBuffer(allocation, m_instanceDataBuffer.get());

		m_dataInvalided = false;
	}
}
