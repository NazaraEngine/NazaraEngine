// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline std::shared_ptr<RenderBuffer>& WorldInstance::GetInstanceBuffer()
	{
		return m_instanceDataBuffer;
	}

	inline const std::shared_ptr<RenderBuffer>& WorldInstance::GetInstanceBuffer() const
	{
		return m_instanceDataBuffer;
	}

	inline const Matrix4f& WorldInstance::GetInvWorldMatrix() const
	{
		return m_invWorldMatrix;
	}

	inline const Matrix4f& WorldInstance::GetWorldMatrix() const
	{
		return m_worldMatrix;
	}

	inline void WorldInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix)
	{
		m_worldMatrix = worldMatrix;
		if (!m_worldMatrix.GetInverseTransform(&m_invWorldMatrix))
			NazaraError("failed to inverse world matrix");

		InvalidateData();
	}

	inline void WorldInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix)
	{
		m_worldMatrix = worldMatrix;
		m_invWorldMatrix = invWorldMatrix;

		InvalidateData();
	}

	void WorldInstance::InvalidateData()
	{
		m_dataInvalided = true;
		OnTransferRequired(this);
	}
}
