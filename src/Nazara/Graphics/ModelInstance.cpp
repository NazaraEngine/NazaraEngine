// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ModelInstance.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ModelInstance::ModelInstance(const std::shared_ptr<const MaterialSettings>& settings) :
	m_invWorldMatrix(Nz::Matrix4f::Identity()),
	m_worldMatrix(Nz::Matrix4f::Identity()),
	m_dataInvalided(true)
	{
		Nz::PredefinedInstanceData instanceUboOffsets = Nz::PredefinedInstanceData::GetOffsets();

		m_instanceDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
		if (!m_instanceDataBuffer->Initialize(instanceUboOffsets.totalSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
			throw std::runtime_error("failed to initialize viewer data UBO");

		m_shaderBinding = settings->GetRenderPipelineLayout()->AllocateShaderBinding();

		StackVector<ShaderBinding::Binding> bindings = NazaraStackVector(ShaderBinding::Binding, 2);

		if (std::size_t bindingIndex = settings->GetPredefinedBindingIndex(PredefinedShaderBinding::UboInstanceData); bindingIndex != MaterialSettings::InvalidIndex)
		{
			bindings.push_back({
				bindingIndex,
				ShaderBinding::UniformBufferBinding {
					m_instanceDataBuffer.get(), 0, m_instanceDataBuffer->GetSize()
				}
			});
		}

		if (std::size_t bindingIndex = settings->GetPredefinedBindingIndex(PredefinedShaderBinding::UboViewerData); bindingIndex != MaterialSettings::InvalidIndex)
		{
			const std::shared_ptr<AbstractBuffer>& instanceDataUBO = Graphics::Instance()->GetViewerDataUBO();

			bindings.push_back({
				bindingIndex,
				ShaderBinding::UniformBufferBinding {
					instanceDataUBO.get(), 0, instanceDataUBO->GetSize()
				}
			});
		}

		if (!bindings.empty())
			m_shaderBinding->Update(bindings.data(), bindings.size());
	}

	void ModelInstance::UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder)
	{
		if (m_dataInvalided)
		{
			Nz::PredefinedInstanceData instanceUboOffsets = Nz::PredefinedInstanceData::GetOffsets();

			auto& allocation = uploadPool.Allocate(m_instanceDataBuffer->GetSize());
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.worldMatrixOffset) = m_worldMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, instanceUboOffsets.invWorldMatrixOffset) = m_invWorldMatrix;

			builder.CopyBuffer(allocation, m_instanceDataBuffer.get());

			m_dataInvalided = false;
		}
	}
}
