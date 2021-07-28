// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	WorldInstance::WorldInstance() :
	m_invWorldMatrix(Nz::Matrix4f::Identity()),
	m_worldMatrix(Nz::Matrix4f::Identity()),
	m_dataInvalided(true)
	{
		PredefinedInstanceData instanceUboOffsets = PredefinedInstanceData::GetOffsets();

		m_instanceDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
		if (!m_instanceDataBuffer->Initialize(instanceUboOffsets.totalSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
			throw std::runtime_error("failed to initialize viewer data UBO");

		m_shaderBinding = Graphics::Instance()->GetReferencePipelineLayout()->AllocateShaderBinding(Graphics::WorldBindingSet);
		m_shaderBinding->Update({
			{
				0,
				ShaderBinding::UniformBufferBinding {
					m_instanceDataBuffer.get(), 0, m_instanceDataBuffer->GetSize()
				}
			}
		});
	}

	void WorldInstance::UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder)
	{
		if (m_dataInvalided)
		{
			PredefinedInstanceData instanceUboOffsets = PredefinedInstanceData::GetOffsets();

			auto& allocation = uploadPool.Allocate(m_instanceDataBuffer->GetSize());
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.worldMatrixOffset) = m_worldMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.invWorldMatrixOffset) = m_invWorldMatrix;

			builder.CopyBuffer(allocation, m_instanceDataBuffer.get());

			m_dataInvalided = false;
		}
	}
}
