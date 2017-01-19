// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Icon.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/IconImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Utility/X11/IconImpl.hpp>
#else
	#error Lack of implementation: Icon
#endif

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool Icon::Create(const Image& icon)
	{
		Destroy();

		std::unique_ptr<IconImpl> impl(new IconImpl);
		if (!impl->Create(icon))
		{
			NazaraError("Failed to create icon implementation");
			return false;
		}

		m_impl = impl.release();

		return true;
	}

	void Icon::Destroy()
	{
		if (m_impl)
		{
			m_impl->Destroy();

			delete m_impl;
			m_impl = nullptr;
		}
	}
}
