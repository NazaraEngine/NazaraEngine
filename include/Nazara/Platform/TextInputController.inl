// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void TextInputController::StartTextInput()
	{
		OnTextInputStarted(this);
	}

	inline void Nz::TextInputController::StopTextInput()
	{
		OnTextInputStopped(this);
	}
}
