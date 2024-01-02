// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Icon.hpp>
#include <Nazara/Platform/SDL2/IconImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	Icon::Icon() = default;

	Icon::Icon(const Image& icon)
	{
		ErrorFlags flags(ErrorMode::ThrowException);
		Create(icon);
	}

	Icon::Icon(Icon&&) noexcept = default;
	Icon::~Icon() = default;

	bool Icon::Create(const Image& icon)
	{
		Destroy();

		try
		{
			m_impl = std::make_unique<IconImpl>(icon);
		}
		catch (const std::exception& e)
		{
			NazaraError(e.what());
			return false;
		}

		return true;
	}

	void Icon::Destroy()
	{
		m_impl.reset();
	}

	Icon& Icon::operator=(Icon&&) noexcept = default;
}
