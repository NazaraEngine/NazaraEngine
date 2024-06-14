// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/Android/MessageBoxImpl.hpp>

namespace Nz
{
	Result<int, const char*> MessageBoxImpl::Show(const MessageBox& messageBox, const Window* parent)
	{
		return Err("not implemented");
	}
}
