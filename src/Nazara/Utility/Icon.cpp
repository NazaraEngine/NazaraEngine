// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Icon.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/IconImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Utility/Linux/IconImpl.hpp>
#else
	#error Lack of implementation: Icon
#endif

#include <Nazara/Utility/Debug.hpp>

NzIcon::NzIcon() :
m_impl(nullptr)
{
}

NzIcon::~NzIcon()
{
	Destroy();
}

bool NzIcon::Create(const NzImage& icon)
{
	Destroy();

	m_impl = new NzIconImpl;
	if (!m_impl->Create(icon))
	{
		NazaraError("Failed to create icon implementation");
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	return true;
}

void NzIcon::Destroy()
{
	if (m_impl)
	{
		m_impl->Destroy();

		delete m_impl;
		m_impl = nullptr;
	}
}

bool NzIcon::IsValid() const
{
	return m_impl != nullptr;
}
