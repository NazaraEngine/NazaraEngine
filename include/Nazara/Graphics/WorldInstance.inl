// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::shared_ptr<AbstractBuffer>& WorldInstance::GetInstanceBuffer()
	{
		return m_instanceDataBuffer;
	}

	inline const std::shared_ptr<AbstractBuffer>& WorldInstance::GetInstanceBuffer() const
	{
		return m_instanceDataBuffer;
	}

	inline const Matrix4f& WorldInstance::GetInvWorldMatrix() const
	{
		return m_invWorldMatrix;
	}

	inline ShaderBinding& WorldInstance::GetShaderBinding()
	{
		return *m_shaderBinding;
	}

	inline const ShaderBinding& WorldInstance::GetShaderBinding() const
	{
		return *m_shaderBinding;
	}

	inline const Matrix4f& WorldInstance::GetWorldMatrix() const
	{
		return m_worldMatrix;
	}

	inline void WorldInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix)
	{
		m_worldMatrix = worldMatrix;
		if (!m_worldMatrix.GetInverseAffine(&m_invWorldMatrix))
			NazaraError("failed to inverse world matrix");

		m_dataInvalided = true;
	}

	inline void WorldInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix)
	{
		m_worldMatrix = worldMatrix;
		m_invWorldMatrix = invWorldMatrix;
		m_dataInvalided = true;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
