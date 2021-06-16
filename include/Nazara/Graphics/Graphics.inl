// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<RenderPipelineLayout>& Graphics::GetReferencePipelineLayout() const
	{
		return m_referencePipelineLayout;
	}

	inline const std::shared_ptr<RenderDevice>& Graphics::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline TextureSamplerCache& Graphics::GetSamplerCache()
	{
		return *m_samplerCache;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
