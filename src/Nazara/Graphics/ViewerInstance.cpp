// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/StackVector.hpp>

namespace Nz
{
	ViewerInstance::ViewerInstance() :
	m_invProjectionMatrix(Matrix4f::Identity()),
	m_invViewProjMatrix(Matrix4f::Identity()),
	m_invViewMatrix(Matrix4f::Identity()),
	m_projectionMatrix(Matrix4f::Identity()),
	m_viewProjMatrix(Matrix4f::Identity()),
	m_viewMatrix(Matrix4f::Identity()),
	m_targetSize(Vector2f::Zero()),
	m_eyePosition(Vector3f::Zero()),
	m_dataInvalidated(true),
	m_farPlane(-1.f),
	m_nearPlane(-1.f)
	{
		m_viewerDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, PredefinedViewerOffsets.totalSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
		m_viewerDataBuffer->UpdateDebugName("Viewer data");
	}

	void ViewerInstance::OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder)
	{
		if (!m_dataInvalidated)
			return;

		constexpr auto& viewerDataOffsets = PredefinedViewerOffsets;

		auto& allocation = renderResources.GetUploadPool().Allocate(viewerDataOffsets.totalSize);
		AccessByOffset<Vector3f&>(allocation.mappedPtr, viewerDataOffsets.eyePositionOffset) = m_eyePosition;
		AccessByOffset<Vector2f&>(allocation.mappedPtr, viewerDataOffsets.invTargetSizeOffset) = 1.f / m_targetSize;
		AccessByOffset<Vector2f&>(allocation.mappedPtr, viewerDataOffsets.targetSizeOffset) = m_targetSize;

		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invProjMatrixOffset) = m_invProjectionMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewMatrixOffset) = m_invViewMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.invViewProjMatrixOffset) = m_invViewProjMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.projMatrixOffset) = m_projectionMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewProjMatrixOffset) = m_viewProjMatrix;
		AccessByOffset<Matrix4f&>(allocation.mappedPtr, viewerDataOffsets.viewMatrixOffset) = m_viewMatrix;

		AccessByOffset<float&>(allocation.mappedPtr, viewerDataOffsets.nearPlaneOffset) = m_nearPlane;
		AccessByOffset<float&>(allocation.mappedPtr, viewerDataOffsets.farPlaneOffset) = m_farPlane;

		builder.CopyBuffer(allocation, m_viewerDataBuffer.get());

		m_dataInvalidated = false;
	}
}
