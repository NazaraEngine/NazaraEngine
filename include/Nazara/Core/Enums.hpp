// Copyright (C) 2013 Jérôme Leclercq
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

enum nzPlugin
{
	nzPlugin_Assimp,
	nzPlugin_FreeType
};

enum nzProcessorCap
{
	nzProcessorCap_x64,
	nzProcessorCap_AVX,
	nzProcessorCap_FMA3,
	nzProcessorCap_FMA4,
	nzProcessorCap_MMX,
	nzProcessorCap_XOP,
	nzProcessorCap_SSE,
	nzProcessorCap_SSE2,
	nzProcessorCap_SSE3,
	nzProcessorCap_SSSE3,
	nzProcessorCap_SSE41,
	nzProcessorCap_SSE42,
	nzProcessorCap_SSE4a,

	nzProcessorCap_Max = nzProcessorCap_SSE4a
};

enum nzProcessorVendor
{
	nzProcessorVendor_Unknown = -1,

	nzProcessorVendor_AMD,
	nzProcessorVendor_Centaur,
	nzProcessorVendor_Cyrix,
	nzProcessorVendor_Intel,
	nzProcessorVendor_Transmeta,
	nzProcessorVendor_NSC,
	nzProcessorVendor_NexGen,
	nzProcessorVendor_Rise,
	nzProcessorVendor_SIS,
	nzProcessorVendor_UMC,
	nzProcessorVendor_VIA,
	nzProcessorVendor_Vortex,

	nzProcessorVendor_Max = nzProcessorVendor_Vortex
};

enum nzStreamOptionFlags
{
	nzStreamOption_None = 0x0,

	nzStreamOption_Text = 0x1
};

enum nzTernary
{
	nzTernary_False,
	nzTernary_True,
	nzTernary_Unknown,

	nzTernary_Max = nzTernary_Unknown
};

#endif // NAZARA_ENUMS_CORE_HPP
