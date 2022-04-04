// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_ENUMS_HPP
#define NAZARA_SHADER_ENUMS_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class ShaderStageType
	{
		Fragment,
		Vertex,

		Max = Vertex
	};

	constexpr std::size_t ShaderStageTypeCount = static_cast<std::size_t>(ShaderStageType::Max) + 1;

	template<>
	struct EnumAsFlags<ShaderStageType>
	{
		static constexpr ShaderStageType max = ShaderStageType::Max;
	};

	using ShaderStageTypeFlags = Flags<ShaderStageType>;

	constexpr ShaderStageTypeFlags ShaderStageType_All = ShaderStageType::Fragment | ShaderStageType::Vertex;

	enum class StructFieldType
	{
		Bool1,
		Bool2,
		Bool3,
		Bool4,
		Float1,
		Float2,
		Float3,
		Float4,
		Double1,
		Double2,
		Double3,
		Double4,
		Int1,
		Int2,
		Int3,
		Int4,
		UInt1,
		UInt2,
		UInt3,
		UInt4,

		Max = UInt4
	};

	enum class StructLayout
	{
		Packed,
		Std140,

		Max = Std140
	};

}

#endif // NAZARA_SHADER_ENUMS_HPP
