// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

inline NzRenderTexture::NzRenderTexture() :
m_impl(nullptr)
{
}

inline NzRenderTexture::~NzRenderTexture()
{
	Destroy();
}

inline bool NzRenderTexture::IsValid() const
{
	return m_impl != nullptr;
}

inline void NzRenderTexture::SetColorTarget(nzUInt8 target) const
{
	SetColorTargets(&target, 1);
}

inline void NzRenderTexture::Blit(NzRenderTexture* src, NzRenderTexture* dst, nzUInt32 buffers, bool bilinearFilter)
{
	Blit(src, src->GetSize(), dst, dst->GetSize(), buffers, bilinearFilter);
}

inline void NzRenderTexture::InvalidateDrawBuffers() const
{
	m_drawBuffersUpdated = false;
}

inline void NzRenderTexture::InvalidateSize() const
{
	m_sizeUpdated = false;

	OnRenderTargetSizeChange(this);
}

inline void NzRenderTexture::InvalidateTargets() const
{
	m_checked = false;
	m_drawBuffersUpdated = false;
	m_targetsUpdated = false;
}

#include <Nazara/Renderer/DebugOff.hpp>
