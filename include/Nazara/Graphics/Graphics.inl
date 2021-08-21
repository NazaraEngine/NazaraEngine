// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<RenderPipeline>& Graphics::GetBlitPipeline() const
	{
		return m_blitPipeline;
	}

	inline const std::shared_ptr<RenderPipelineLayout>& Graphics::GetBlitPipelineLayout() const
	{
		return m_blitPipelineLayout;
	}

	inline const std::shared_ptr<AbstractBuffer>& Graphics::GetFullscreenVertexBuffer() const
	{
		return m_fullscreenVertexBuffer;
	}

	inline const std::shared_ptr<VertexDeclaration>& Graphics::GetFullscreenVertexDeclaration() const
	{
		return m_fullscreenVertexDeclaration;
	}

	inline MaterialPassRegistry& Graphics::GetMaterialPassRegistry()
	{
		return m_materialPassRegistry;
	}

	inline const MaterialPassRegistry& Graphics::GetMaterialPassRegistry() const
	{
		return m_materialPassRegistry;
	}

	inline PixelFormat Graphics::GetPreferredDepthStencilFormat() const
	{
		return m_preferredDepthStencilFormat;
	}

	inline const std::shared_ptr<RenderPipelineLayout>& Graphics::GetReferencePipelineLayout() const
	{
		return m_referencePipelineLayout;
	}

	inline const std::shared_ptr<RenderDevice>& Graphics::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline const RenderPassCache& Graphics::GetRenderPassCache() const
	{
		assert(m_renderPassCache);
		return *m_renderPassCache;
	}

	inline TextureSamplerCache& Graphics::GetSamplerCache()
	{
		return *m_samplerCache;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
