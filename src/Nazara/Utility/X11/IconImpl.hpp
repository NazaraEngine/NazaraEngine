// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICONIMPL_HPP
#define NAZARA_ICONIMPL_HPP

#include <Nazara/Utility/X11/ScopedXCB.hpp>

class NzImage;

class NzIconImpl
{
	public:
		NzIconImpl();

		bool Create(const NzImage& image);
		void Destroy();

		xcb_pixmap_t GetIcon();
		xcb_pixmap_t GetMask();

	private:
		NzXCBPixmap m_iconPixmap;
		NzXCBPixmap m_maskPixmap;
};

#endif // NAZARA_ICONIMPL_HPP
