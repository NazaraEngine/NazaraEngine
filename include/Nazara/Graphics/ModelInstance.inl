// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ModelInstance.hpp>
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
}

#include <Nazara/Graphics/DebugOff.hpp>
