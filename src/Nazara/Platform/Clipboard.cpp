// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Clipboard.hpp>
#include <Nazara/Platform/SDL3/InputImpl.hpp>

namespace Nz
{
	ClipboardContentType Clipboard::GetContentType()
	{
		return InputImpl::GetClipboardContentType();
	}

	std::string Clipboard::GetString()
	{
		return InputImpl::GetClipboardString();
	}

	void Clipboard::SetString(const std::string& str)
	{
		return InputImpl::SetClipboardString(str);
	}
}
