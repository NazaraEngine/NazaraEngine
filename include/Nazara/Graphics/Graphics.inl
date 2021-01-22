// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderDevice& Graphics::GetRenderDevice()
	{
		return *m_renderDevice;
	}

	inline const std::shared_ptr<AbstractBuffer>& Graphics::GetViewerDataUBO()
	{
		return m_viewerDataUBO;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
