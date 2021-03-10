// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline std::size_t GetComponentCount(BasicType type)
	{
		switch (type)
		{
			case BasicType::Float2:
			case BasicType::Int2:
				return 2;

			case BasicType::Float3:
			case BasicType::Int3:
				return 3;

			case BasicType::Float4:
			case BasicType::Int4:
				return 4;

			case BasicType::Mat4x4:
				return 4;

			default:
				return 1;
		}
	}

	inline BasicType GetComponentType(BasicType type)
	{
		switch (type)
		{
			case BasicType::Float2:
			case BasicType::Float3:
			case BasicType::Float4:
				return BasicType::Float1;

			case BasicType::Int2:
			case BasicType::Int3:
			case BasicType::Int4:
				return BasicType::Int1;

			case BasicType::Mat4x4:
				return BasicType::Float4;

			default:
				return type;
		}
	}
}

#include <Nazara/Shader/DebugOff.hpp>
