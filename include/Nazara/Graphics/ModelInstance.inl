// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ModelInstance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::shared_ptr<AbstractBuffer>& ModelInstance::GetInstanceBuffer()
	{
		return m_instanceDataBuffer;
	}

	inline const std::shared_ptr<AbstractBuffer>& ModelInstance::GetInstanceBuffer() const
	{
		return m_instanceDataBuffer;
	}

	inline ShaderBinding& ModelInstance::GetShaderBinding()
	{
		return *m_shaderBinding;
	}

	inline void ModelInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix)
	{
		m_worldMatrix = worldMatrix;
		if (!m_worldMatrix.GetInverseAffine(&m_invWorldMatrix))
			NazaraError("failed to inverse world matrix");

		m_dataInvalided = true;
	}

	inline void ModelInstance::UpdateWorldMatrix(const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix)
	{
		m_worldMatrix = worldMatrix;
		m_invWorldMatrix = invWorldMatrix;
		m_dataInvalided = true;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
