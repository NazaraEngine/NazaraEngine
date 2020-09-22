// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderExpressionType.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline bool IsBasicType(const ShaderExpressionType& type)
	{
		return std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				return true;
			else if constexpr (std::is_same_v<T, std::string>)
				return false;
			else
				static_assert(AlwaysFalse<U>::value, "non-exhaustive visitor");

		}, type);
	}

	inline bool IsMatrixType(const ShaderExpressionType& type)
	{
		using namespace ShaderNodes;

		if (!IsBasicType(type))
			return false;

		switch (std::get<BasicType>(type))
		{
			case BasicType::Mat4x4:
				return true;

			case BasicType::Boolean:
			case BasicType::Float1:
			case BasicType::Float2:
			case BasicType::Float3:
			case BasicType::Float4:
			case BasicType::Int1:
			case BasicType::Int2:
			case BasicType::Int3:
			case BasicType::Int4:
			case BasicType::Sampler2D:
			case BasicType::Void:
			case BasicType::UInt1:
			case BasicType::UInt2:
			case BasicType::UInt3:
			case BasicType::UInt4:
				return false;
		}

		return false;
	}

	inline bool IsSamplerType(const ShaderExpressionType& type)
	{
		using namespace ShaderNodes;

		if (!IsBasicType(type))
			return false;

		switch (std::get<BasicType>(type))
		{
			case BasicType::Sampler2D:
				return true;

			case BasicType::Boolean:
			case BasicType::Float1:
			case BasicType::Float2:
			case BasicType::Float3:
			case BasicType::Float4:
			case BasicType::Int1:
			case BasicType::Int2:
			case BasicType::Int3:
			case BasicType::Int4:
			case BasicType::Mat4x4:
			case BasicType::Void:
			case BasicType::UInt1:
			case BasicType::UInt2:
			case BasicType::UInt3:
			case BasicType::UInt4:
				return false;
		}

		return false;
	}

	inline bool IsStructType(const ShaderExpressionType& type)
	{
		return std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				return false;
			else if constexpr (std::is_same_v<T, std::string>)
				return true;
			else
				static_assert(AlwaysFalse<U>::value, "non-exhaustive visitor");

		}, type);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
