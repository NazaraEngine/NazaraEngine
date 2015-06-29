// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline RenderSystem::RenderSystem(const RenderSystem& renderSystem) :
	System(renderSystem)
	{
	}

	inline const NzBackgroundRef& RenderSystem::GetDefaultBackground() const
	{
		return m_background;
	}

	inline void RenderSystem::SetDefaultBackground(NzBackgroundRef background)
	{
		m_background = std::move(background);
	}
}
