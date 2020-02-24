// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Systems/DebugSystem.hpp>

namespace Ndk
{
	inline bool DebugSystem::IsDepthBufferEnabled() const
	{
		return m_isDepthBufferEnabled;
	}
}
