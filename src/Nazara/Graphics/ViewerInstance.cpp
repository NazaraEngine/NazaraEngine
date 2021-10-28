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
	m_invProjectionMatrix(Matrix4f::Identity()),
	m_invViewProjMatrix(Matrix4f::Identity()),
	m_invViewMatrix(Matrix4f::Identity()),
	m_projectionMatrix(Matrix4f::Identity()),
	m_viewProjMatrix(Matrix4f::Identity()),
	m_viewMatrix(Matrix4f::Identity()),
	m_targetSize(Vector2f(0.f, 0.f)),
	m_dataInvalided(true)
	{
		PredefinedViewerData viewerUboOffsets = PredefinedViewerData::GetOffsets();

		m_viewerDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
		if (!m_viewerDataBuffer->Initialize(viewerUboOffsets.totalSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic))
			throw std::runtime_error("failed to initialize viewer data UBO");
	}

	void ViewerInstance::UpdateBuffers(UploadPool& uploadPool, CommandBufferBuilder& builder)
	{
		if (m_dataInvalided)
		{
			PredefinedViewerData viewerDataOffsets = PredefinedViewerData::GetOffsets();

			auto& allocation = uploadPool.Allocate(viewerDataOffsets.totalSize);
			AccessByOffset<Vector3f&>(allocation.mappedPtr, viewerDataOffsets.eyePositionOffset) = m_viewMatrix.GetTranslation();
			AccessByOffset<Vector2f&>(allocation.mappedPtr, viewerDataOffsets.invTargetSizeOffset) = 1.f / m_targetSize;
			AccessByOffset<Vector2f&>(allocation.mappedPtr, viewerDataOffsets.targetSizeOffset) = m_targetSize;

			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invProjMatrixOffset) = m_invProjectionMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewMatrixOffset) = m_invViewMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewProjMatrixOffset) = m_invViewProjMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.projMatrixOffset) = m_projectionMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewProjMatrixOffset) = m_viewProjMatrix;
			AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewMatrixOffset) = m_viewMatrix;

			builder.CopyBuffer(allocation, m_viewerDataBuffer.get());

			m_dataInvalided = false;
		}
	}
}
