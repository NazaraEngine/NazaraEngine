// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline Cursor::Cursor() :
	m_impl(nullptr)
	{
	}

	inline Cursor::~Cursor()
	{
		Destroy();
	}

	inline const Image& Cursor::GetImage() const
	{
		return m_cursorImage;
	}

	inline bool Cursor::IsValid() const
	{
		return m_impl != nullptr;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
