// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICONIMPL_HPP
#define NAZARA_ICONIMPL_HPP

#include <windows.h>

class NzImage;

class NzIconImpl
{
	public:
		bool Create(const NzImage& image);
		void Destroy();

		HICON GetIcon();

	private:
		HICON m_icon = nullptr;
};

#endif // NAZARA_ICONIMPL_HPP
