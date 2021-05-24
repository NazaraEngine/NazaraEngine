// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ModelInstance.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ModelInstance::ModelInstance(const std::shared_ptr<const MaterialSettings>& settings)
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
}
