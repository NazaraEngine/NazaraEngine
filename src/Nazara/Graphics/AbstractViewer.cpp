// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::AbstractViewer
	* \brief Graphics class that represents the viewer for our scene
	*
	* \remark This class is abstract
	*/

	AbstractViewer::AbstractViewer() :
	m_viewerDataUpdated(false)
	{
		std::size_t viewerDataSize = PredefinedViewerData::GetOffset().totalSize;
		m_viewerData = UniformBuffer::New(viewerDataSize, DataStorage_Hardware, BufferUsage_Dynamic);
	}

	AbstractViewer::~AbstractViewer() = default;

	Vector3f AbstractViewer::Project(const Nz::Vector3f& worldPosition) const
	{
		Vector4f pos4D(worldPosition, 1.f);
		pos4D = GetViewMatrix() * pos4D;
		pos4D = GetProjectionMatrix() * pos4D;
		pos4D /= pos4D.w;

		Rectf viewport = Rectf(GetViewport());

		Nz::Vector3f screenPosition(pos4D.x * 0.5f + 0.5f, -pos4D.y * 0.5f + 0.5f, pos4D.z * 0.5f + 0.5f);
		screenPosition.x = screenPosition.x * viewport.width + viewport.x;
		screenPosition.y = screenPosition.y * viewport.height + viewport.y;

		return screenPosition;
	}

	float AbstractViewer::ProjectDepth(float depth)
	{
		const Matrix4f& projectionMatrix = GetProjectionMatrix();
		float a = projectionMatrix(2, 2);
		float b = projectionMatrix(2, 3);

		return (0.5f * (-a * depth + b) / depth + 0.5f);
	}

	Vector3f AbstractViewer::Unproject(const Nz::Vector3f& screenPos) const
	{
		Rectf viewport = Rectf(GetViewport());

		Nz::Vector4f normalizedPosition;
		normalizedPosition.x = (screenPos.x - viewport.x) / viewport.width * 2.f - 1.f;
		normalizedPosition.y = (screenPos.y - viewport.y) / viewport.height * 2.f - 1.f;
		normalizedPosition.z = screenPos.z * 2.f - 1.f;
		normalizedPosition.w = 1.f;

		normalizedPosition.y = -normalizedPosition.y;

		Nz::Matrix4f invMatrix = GetViewMatrix() * GetProjectionMatrix();
		invMatrix.Inverse();

		Nz::Vector4f worldPos = invMatrix * normalizedPosition;
		worldPos /= worldPos.w;

		return Vector3f(worldPos.x, worldPos.y, worldPos.z);
	}

	void AbstractViewer::UpdateViewerData() const
	{
		PredefinedViewerData viewerDataOffsets = PredefinedViewerData::GetOffset();

		static_assert(sizeof(Matrix4f) == 16 * sizeof(float), "Unexpected Matrix4f size");
		static_assert(sizeof(Vector2f) == 2 * sizeof(float), "Unexpected Vector2f size");
		static_assert(sizeof(Vector3f) == 3 * sizeof(float), "Unexpected Vector3f size");

		const Matrix4f& projectionMatrix = GetProjectionMatrix();
		const Matrix4f& viewMatrix = GetViewMatrix();

		Matrix4f invProjectionMatrix;
		projectionMatrix.GetInverse(&invProjectionMatrix);

		Matrix4f invViewMatrix;
		viewMatrix.GetInverseAffine(&invViewMatrix);

		BufferMapper<UniformBuffer> mapper(m_viewerData, BufferAccess_WriteOnly);
		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.projMatrixOffset) = projectionMatrix;
		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.invProjMatrixOffset) = invProjectionMatrix;

		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.viewMatrixOffset) = viewMatrix;
		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.invViewMatrixOffset) = invViewMatrix;

		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.viewProjMatrixOffset) = viewMatrix * projectionMatrix;
		*AccessByOffset<Matrix4f>(mapper.GetPointer(), viewerDataOffsets.invViewProjMatrixOffset) = invProjectionMatrix * invViewMatrix;

		Vector2f targetSize = Vector2f(GetTarget()->GetSize());
		Vector2f invTargetSize = 1.f / targetSize;

		*AccessByOffset<Vector2f>(mapper.GetPointer(), viewerDataOffsets.targetSizeOffset) = targetSize;
		*AccessByOffset<Vector2f>(mapper.GetPointer(), viewerDataOffsets.invTargetSizeOffset) = invTargetSize;

		*AccessByOffset<Vector3f>(mapper.GetPointer(), viewerDataOffsets.eyePositionOffset) = GetEyePosition();

		m_viewerDataUpdated = true;
	}
}
