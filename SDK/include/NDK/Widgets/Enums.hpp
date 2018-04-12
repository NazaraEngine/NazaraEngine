// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NAZARA_ENUMS_SDK_HPP
#define NAZARA_ENUMS_SDK_HPP

namespace Ndk
{
	enum CheckboxState
	{
		CheckboxState_Checked,
		CheckboxState_Tristate,
		CheckboxState_Unchecked,

		CheckboxState_Max = CheckboxState_Unchecked
	};

	enum EchoMode
	{
		EchoMode_Normal,
		EchoMode_Password,
		EchoMode_PasswordExceptLast,

		EchoMode_Max = EchoMode_PasswordExceptLast
	};
}

#endif // NAZARA_ENUMS_SDK_HPP
