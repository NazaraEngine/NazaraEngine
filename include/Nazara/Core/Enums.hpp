// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_CORE_HPP
#define NAZARA_ENUMS_CORE_HPP

namespace Nz
{
	enum CoordSys
	{
		CoordSys_Global,
		CoordSys_Local,

		CoordSys_Max = CoordSys_Local
	};

	enum CursorPosition
	{
		CursorPosition_AtBegin,   // beginning of the file
		CursorPosition_AtCurrent, // Position of the cursor
		CursorPosition_AtEnd,     // End of the file

		CursorPosition_Max = CursorPosition_AtEnd
	};

	enum Endianness
	{
		Endianness_Unknown = -1,

		Endianness_BigEndian,
		Endianness_LittleEndian,

		Endianness_Max = Endianness_LittleEndian
	};

	enum ErrorFlag
	{
		ErrorFlag_None = 0,

		ErrorFlag_Silent                 = 0x1,
		ErrorFlag_SilentDisabled         = 0x2,
		ErrorFlag_ThrowException         = 0x4,
		ErrorFlag_ThrowExceptionDisabled = 0x8,

		ErrorFlag_Max = ErrorFlag_ThrowExceptionDisabled * 2 - 1
	};

	enum ErrorType
	{
		ErrorType_AssertFailed,
		ErrorType_Internal,
		ErrorType_Normal,
		ErrorType_Warning,

		ErrorType_Max = ErrorType_Warning
	};

	enum HashType
	{
		HashType_CRC32,
		HashType_Fletcher16,
		HashType_MD5,
		HashType_SHA1,
		HashType_SHA224,
		HashType_SHA256,
		HashType_SHA384,
		HashType_SHA512,
		HashType_Whirlpool,

		HashType_Max = HashType_Whirlpool
	};

	enum OpenModeFlags
	{
		OpenMode_NotOpen   = 0x00, // Use the current mod of opening

		OpenMode_Append    = 0x01, // Disable writing on existing parts and put the cursor at the end
		OpenMode_Lock      = 0x02, // Disable modifying the file before it is open
		OpenMode_MustExit  = 0x04, // Fail if the file doesn't exists, even if opened in write mode
		OpenMode_ReadOnly  = 0x08, // Open in read only
		OpenMode_Text      = 0x10, // Open in text mod
		OpenMode_Truncate  = 0x20, // Create the file if it doesn't exist and empty it if it exists
		OpenMode_WriteOnly = 0x40, // Open in write only, create the file if it doesn't exist

		OpenMode_ReadWrite = OpenMode_ReadOnly | OpenMode_WriteOnly, // Open in read and write

		OpenMode_Max = OpenMode_WriteOnly * 2 - 1
	};

	enum ParameterType
	{
		ParameterType_Boolean,
		ParameterType_Color,
		ParameterType_Float,
		ParameterType_Integer,
		ParameterType_None,
		ParameterType_Pointer,
		ParameterType_String,
		ParameterType_Userdata,

		ParameterType_Max = ParameterType_Userdata
	};

	enum Plugin
	{
		Plugin_Assimp,

		Plugin_Count
	};

	enum PrimitiveType
	{
		PrimitiveType_Box,
		PrimitiveType_Cone,
		PrimitiveType_Plane,
		PrimitiveType_Sphere,

		PrimitiveType_Max = PrimitiveType_Sphere
	};

	enum ProcessorCap
	{
		ProcessorCap_x64,
		ProcessorCap_AVX,
		ProcessorCap_FMA3,
		ProcessorCap_FMA4,
		ProcessorCap_MMX,
		ProcessorCap_XOP,
		ProcessorCap_SSE,
		ProcessorCap_SSE2,
		ProcessorCap_SSE3,
		ProcessorCap_SSSE3,
		ProcessorCap_SSE41,
		ProcessorCap_SSE42,
		ProcessorCap_SSE4a,

		ProcessorCap_Max = ProcessorCap_SSE4a
	};

	enum ProcessorVendor
	{
		ProcessorVendor_Unknown = -1,

		ProcessorVendor_AMD,
		ProcessorVendor_Centaur,
		ProcessorVendor_Cyrix,
		ProcessorVendor_Intel,
		ProcessorVendor_KVM,
		ProcessorVendor_HyperV,
		ProcessorVendor_NSC,
		ProcessorVendor_NexGen,
		ProcessorVendor_Rise,
		ProcessorVendor_SIS,
		ProcessorVendor_Transmeta,
		ProcessorVendor_UMC,
		ProcessorVendor_VIA,
		ProcessorVendor_VMware,
		ProcessorVendor_Vortex,
		ProcessorVendor_XenHVM,

		ProcessorVendor_Max = ProcessorVendor_XenHVM
	};

	enum SphereType
	{
		SphereType_Cubic,
		SphereType_Ico,
		SphereType_UV,

		SphereType_Max = SphereType_UV
	};

	enum StreamOptionFlags
	{
		StreamOption_None = 0,

		StreamOption_Sequential = 0x1,
		StreamOption_Text       = 0x2,

		StreamOption_Max = StreamOption_Text * 2 - 1
	};

	enum Ternary
	{
		Ternary_False,
		Ternary_True,
		Ternary_Unknown,

		Ternary_Max = Ternary_Unknown
	};
}

#endif // NAZARA_ENUMS_CORE_HPP
