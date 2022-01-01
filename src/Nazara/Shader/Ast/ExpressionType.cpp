// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstCompare.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ArrayType::ArrayType(const ArrayType& array)
	{
		assert(array.containedType);
		containedType = std::make_unique<ContainedType>(*array.containedType);
		length = Clone(length);
	}

	ArrayType& ArrayType::operator=(const ArrayType& array)
	{
		assert(array.containedType);

		containedType = std::make_unique<ContainedType>(*array.containedType);
		length = Clone(length);

		return *this;
	}

	bool ArrayType::operator==(const ArrayType& rhs) const
	{
		assert(containedType);
		assert(rhs.containedType);

		if (containedType->type != rhs.containedType->type)
			return false;

		if (!Compare(length, rhs.length))
			return false;

		return true;
	}
}
