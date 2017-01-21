// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderTexture::RenderTexture() :
	m_impl(nullptr)
	{
	}

	inline RenderTexture::~RenderTexture()
	{
		Destroy();
	}

	inline bool RenderTexture::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline void RenderTexture::SetColorTarget(UInt8 target) const
	{
		SetColorTargets(&target, 1);
	}

	inline void RenderTexture::Blit(RenderTexture* src, RenderTexture* dst, UInt32 buffers, bool bilinearFilter)
	{
		Blit(src, src->GetSize(), dst, dst->GetSize(), buffers, bilinearFilter);
	}

	inline void RenderTexture::InvalidateDrawBuffers() const
	{
		m_drawBuffersUpdated = false;
	}

	inline void RenderTexture::InvalidateSize() const
	{
		m_sizeUpdated = false;

		OnRenderTargetSizeChange(this);
	}

	inline void RenderTexture::InvalidateTargets() const
	{
		m_checked = false;
		m_drawBuffersUpdated = false;
		m_targetsUpdated = false;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
