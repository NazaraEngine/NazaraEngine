// Copyright (C) 2024 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_ENUMS_HPP
#define NAZARA_WIDGETS_ENUMS_HPP

namespace Nz
{
	enum class BoxLayoutOrientation
	{
		BottomToTop,
		LeftToRight,
		RightToLeft,
		TopToBottom
	};

	enum class CheckboxState
	{
		Checked,
		Tristate,
		Unchecked,

		Max = Unchecked
	};

	enum class EchoMode
	{
		Hidden,
		HiddenExceptLast,
		Normal,

		Max = Normal
	};

	enum class ScrollbarOrientation
	{
		Horizontal,
		Vertical
	};
}

#endif // NAZARA_WIDGETS_ENUMS_HPP
