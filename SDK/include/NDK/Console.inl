// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include "Console.hpp"

namespace Ndk
{
	inline const EntityHandle& Console::GetBackground() const
	{
		return m_background;
	}

	inline unsigned int Console::GetCharacterSize() const
	{
		return m_characterSize;
	}

	inline const EntityHandle& Console::GetHistory() const
	{
		return m_history;
	}

	inline const EntityHandle& Console::GetInput() const
	{
		return m_input;
	}

	inline const Nz::Vector2f& Console::GetSize() const
	{
		return m_size;
	}

	inline const Nz::FontRef& Console::GetTextFont() const
	{
		return m_defaultFont;
	}

	inline bool Console::IsVisible() const
	{
		return m_opened;
	}
}
