// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzColorBackground::NzColorBackground(const NzColor& color) :
m_color(color)
{
}

void NzColorBackground::Draw() const
{
	NzRenderer::SetClearColor(m_color);
	NzRenderer::Clear(nzRendererClear_Color);
}

nzBackgroundType NzColorBackground::GetBackgroundType() const
{
	return nzBackgroundType_Color;
}

NzColor NzColorBackground::GetColor() const
{
	return m_color;
}

void NzColorBackground::SetColor(const NzColor& color)
{
	m_color = color;
}
