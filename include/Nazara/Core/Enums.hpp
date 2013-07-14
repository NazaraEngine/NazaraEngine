// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_CORE_HPP
#define NAZARA_ENUMS_CORE_HPP

enum nzCoordSys
{
	nzCoordSys_Global,
	nzCoordSys_Local,

	nzCoordSys_Max = nzCoordSys_Local
};

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

enum nzHash
{
	nzHash_CRC32,
	nzHash_Fletcher16,
	nzHash_MD5,
	nzHash_SHA1,
	nzHash_SHA224,
	nzHash_SHA256,
	nzHash_SHA384,
	nzHash_SHA512,
	nzHash_Whirlpool
};

enum nzPlugin
{
	nzPlugin_Assimp,
	nzPlugin_FreeType
};

enum nzPrimitiveType
{
	nzPrimitiveType_Box,
	nzPrimitiveType_Plane,
	nzPrimitiveType_Sphere,

	nzPrimitiveType_Max = nzPrimitiveType_Sphere
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

enum nzSphereType
{
	nzSphereType_Cubic,
	nzSphereType_Ico,
	nzSphereType_UV,

	nzSphereType_Max = nzSphereType_UV
};

enum nzStreamOptionFlags
{
	nzStreamOption_None = 0x0,

	nzStreamOption_Text = 0x1,

	nzStreamOption_Max = nzStreamOption_Text*2-1
};

enum nzTernary
{
	nzTernary_False,
	nzTernary_True,
	nzTernary_Unknown,

	nzTernary_Max = nzTernary_Unknown
};

#endif // NAZARA_ENUMS_CORE_HPP
