// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_CORE_HPP
#define NAZARA_ENUMS_CORE_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class CoordSys
	{
		Global,
		Local,

		Max = Local
	};

	enum class CursorPosition
	{
		AtBegin,   // beginning of the file
		AtCurrent, // Position of the cursor
		AtEnd,     // End of the file

		Max = AtEnd
	};

	enum class Endianness
	{
		Unknown = -1,

		BigEndian,
		LittleEndian,

		Max = LittleEndian
	};

	enum class ErrorMode
	{
		None,

		Silent,
		SilentDisabled,
		ThrowException,
		ThrowExceptionDisabled,

		Max = ThrowExceptionDisabled
	};

	template<>
	struct EnumAsFlags<ErrorMode>
	{
		static constexpr ErrorMode max = ErrorMode::Max;
	};

	using ErrorModeFlags = Flags<ErrorMode>;

	enum class ErrorType
	{
		AssertFailed,
		Internal,
		Normal,
		Warning,

		Max = Warning
	};

	constexpr std::size_t ErrorTypeCount = static_cast<std::size_t>(ErrorType::Max) + 1;

	enum class HashType
	{
		CRC32,
		CRC64,
		Fletcher16,
		MD5,
		SHA1,
		SHA224,
		SHA256,
		SHA384,
		SHA512,
		Whirlpool,

		Max = Whirlpool
	};

	constexpr std::size_t HashTypeCount = static_cast<std::size_t>(HashType::Max) + 1;

	enum class OpenMode
	{
		NotOpen,   // Use the current mod of opening

		Append,    // Disable writing on existing parts and put the cursor at the end
		Lock,      // Disable modifying the file before it is open
		MustExist, // Fail if the file doesn't exists, even if opened in write mode
		ReadOnly,  // Open in read only
		Text,      // Open in text mod
		Truncate,  // Create the file if it doesn't exist and empty it if it exists
		WriteOnly, // Open in write only, create the file if it doesn't exist

		Max = WriteOnly
	};

	template<>
	struct EnumAsFlags<OpenMode>
	{
		static constexpr OpenMode max = OpenMode::Max;
	};

	using OpenModeFlags = Flags<OpenMode>;

	constexpr OpenModeFlags OpenMode_ReadWrite = OpenMode::ReadOnly | OpenMode::WriteOnly;

	enum class ParameterType
	{
		Boolean,
		Color,
		Double,
		Integer,
		None,
		Pointer,
		String,
		Userdata,

		Max = Userdata
	};

	enum class Plugin
	{
		Assimp,

		Max = Assimp
	};

	constexpr std::size_t PluginCount = static_cast<std::size_t>(Plugin::Max) + 1;

	enum class PrimitiveType
	{
		Box,
		Cone,
		Plane,
		Sphere,

		Max = Sphere
	};

	constexpr std::size_t PrimitiveTypeCount = static_cast<std::size_t>(PrimitiveType::Max) + 1;

	enum class ProcessorCap
	{
		x64,
		AVX,
		FMA3,
		FMA4,
		MMX,
		XOP,
		SSE,
		SSE2,
		SSE3,
		SSSE3,
		SSE41,
		SSE42,
		SSE4a,

		Max = SSE4a
	};

	constexpr std::size_t ProcessorCapCount = static_cast<std::size_t>(ProcessorCap::Max) + 1;

	enum class ProcessorVendor
	{
		Unknown = -1,

		AMD,
		Centaur,
		Cyrix,
		Intel,
		KVM,
		HyperV,
		NSC,
		NexGen,
		Rise,
		SIS,
		Transmeta,
		UMC,
		VIA,
		VMware,
		Vortex,
		XenHVM,

		Max = XenHVM
	};

	constexpr std::size_t ProcessorVendorCount = static_cast<std::size_t>(ProcessorVendor::Max) + 1;

	enum class SphereType
	{
		Cubic,
		Ico,
		UV,

		Max = UV
	};

	enum class StreamOption
	{
		None,

		Sequential,
		Text,

		Max = Text
	};

	template<>
	struct EnumAsFlags<StreamOption>
	{
		static constexpr StreamOption max = StreamOption::Max;
	};

	using StreamOptionFlags = Flags<StreamOption>;

	enum class Ternary
	{
		False,
		True,
		Unknown,

		Max = Unknown
	};
}

#endif // NAZARA_ENUMS_CORE_HPP
