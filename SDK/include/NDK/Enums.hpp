// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NAZARA_ENUMS_SDK_HPP
#define NAZARA_ENUMS_SDK_HPP

namespace Ndk
{
	enum CheckboxState
	{
		CheckboxState_Unchecked,
		CheckboxState_Checked,
		CheckboxState_Tristate,

		CheckboxState_Max = CheckboxState_Tristate
	};
}

#endif // NAZARA_ENUMS_SDK_HPP
