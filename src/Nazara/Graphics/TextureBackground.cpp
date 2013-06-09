// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextureBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzTextureBackground::NzTextureBackground(NzTexture* texture) :
m_texture(texture)
{
}

void NzTextureBackground::Draw(const NzScene* scene) const
{
	NazaraUnused(scene);

	const NzRenderTarget* target = NzRenderer::GetTarget();
	NzRectui viewport = NzRenderer::GetViewport();

	// Sous forme de flottants pour la division flottante
	float width = static_cast<float>(target->GetWidth());
	float height = static_cast<float>(target->GetHeight());

	NzVector2f uv0(viewport.x/width, viewport.y/height);
	NzVector2f uv1((viewport.x+viewport.width)/width, (viewport.y+viewport.height)/height);

	NzRenderer::SetTexture(0, m_texture);
	NzRenderer::DrawTexture(0, NzRectf(0.f, 0.f, width, height), uv0, uv1, 1.f);
}

nzBackgroundType NzTextureBackground::GetBackgroundType() const
{
	return nzBackgroundType_Texture;
}

NzTexture* NzTextureBackground::GetTexture() const
{
	return m_texture;
}

void NzTextureBackground::SetTexture(NzTexture* texture)
{
	m_texture = texture;
}
