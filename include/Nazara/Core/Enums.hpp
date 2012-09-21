// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_CORE_HPP
#define NAZARA_ENUMS_CORE_HPP

enum nzEndianness
{
	nzEndianness_Unknown = -1,

	nzEndianness_BigEndian,
	nzEndianness_LittleEndian,

	nzEndianness_Max = nzEndianness_LittleEndian
};

enum nzErrorType
{
	nzErrorType_AssertFailed,
	nzErrorType_Internal,
	nzErrorType_Normal,
	nzErrorType_Warning,

	nzErrorType_Max = nzErrorType_Warning
};

#endif // NAZARA_ENUMS_CORE_HPP
