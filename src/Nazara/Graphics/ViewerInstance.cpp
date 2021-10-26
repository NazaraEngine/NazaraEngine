// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ViewerInstance::ViewerInstance() :
	m_invProjectionMatrix(Nz::Matrix4f::Identity()),
	m_invViewProjMatrix(Nz::Matrix4f::Identity()),
	m_invViewMatrix(Nz::Matrix4f::Identity()),
	m_projectionMatrix(Nz::Matrix4f::Identity()),
	m_viewProjMatrix(Nz::Matrix4f::Identity()),
	m_viewMatrix(Nz::Matrix4f::Identity()),
	m_targetSize(Nz::Vector2f(0.f, 0.f)),
	m_dataInvalided(true)
	{
		Nz::PredefinedViewerData viewerUboOffsets = Nz::PredefinedViewerData::GetOffsets();

		m_viewerDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
		if (!m_viewerDataBuffer->Initialize(viewerUboOffsets.totalSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
			throw std::runtime_error("failed to initialize viewer data UBO");

		m_shaderBinding = Graphics::Instance()->GetReferencePipelineLayout()->AllocateShaderBinding(Graphics::ViewerBindingSet);
		m_shaderBinding->Update({
			{
				0,
				ShaderBinding::UniformBufferBinding {
					m_viewerDataBuffer.get(), 0, m_viewerDataBuffer->GetSize()
				}
			}
		});
	}

	void ViewerInstance::UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder)
	{
		if (m_dataInvalided)
		{
			Nz::PredefinedViewerData viewerDataOffsets = Nz::PredefinedViewerData::GetOffsets();

			auto& allocation = uploadPool.Allocate(viewerDataOffsets.totalSize);
			Nz::AccessByOffset<Nz::Vector3f&>(allocation.mappedPtr, viewerDataOffsets.eyePositionOffset) = m_viewMatrix.GetTranslation();
			Nz::AccessByOffset<Nz::Vector2f&>(allocation.mappedPtr, viewerDataOffsets.invTargetSizeOffset) = 1.f / m_targetSize;
			Nz::AccessByOffset<Nz::Vector2f&>(allocation.mappedPtr, viewerDataOffsets.targetSizeOffset) = m_targetSize;

			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invProjMatrixOffset) = m_invProjectionMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewMatrixOffset) = m_invViewMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewProjMatrixOffset) = m_invViewProjMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.projMatrixOffset) = m_projectionMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewProjMatrixOffset) = m_viewProjMatrix;
			Nz::AccessByOffset<Nz::Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewMatrixOffset) = m_viewMatrix;

			builder.CopyBuffer(allocation, m_viewerDataBuffer.get());

			m_dataInvalided = false;
		}
	}
}
