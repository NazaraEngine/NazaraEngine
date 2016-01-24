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
		CursorPosition_AtBegin,   // Début du fichier
		CursorPosition_AtCurrent, // Position du pointeur
		CursorPosition_AtEnd,     // Fin du fichier

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

		ErrorFlag_Max = ErrorFlag_ThrowExceptionDisabled*2-1
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
		OpenMode_NotOpen   = 0x00, // Utilise le mode d'ouverture actuel

		OpenMode_Append    = 0x01, // Empêche l'écriture sur la partie déjà existante et met le curseur à la fin
		OpenMode_Lock      = 0x02, // Empêche le fichier d'être modifié tant qu'il est ouvert
		OpenMode_ReadOnly  = 0x04, // Ouvre uniquement en lecture
		OpenMode_Text      = 0x10, // Ouvre en mode texte
		OpenMode_Truncate  = 0x20, // Créé le fichier s'il n'existe pas et le vide s'il existe
		OpenMode_WriteOnly = 0x40, // Ouvre uniquement en écriture, créé le fichier s'il n'existe pas

		OpenMode_ReadWrite = OpenMode_ReadOnly | OpenMode_WriteOnly, // Ouvre en lecture/écriture

		OpenMode_Max = OpenMode_WriteOnly*2 - 1
	};

	enum ParameterType
	{
		ParameterType_Boolean,
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
		Plugin_FreeType
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

		StreamOption_Max = StreamOption_Text*2 - 1
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
