// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

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
