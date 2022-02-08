// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstCompare.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ArrayType::ArrayType(const ArrayType& array) :
	length(array.length)
	{
		assert(array.containedType);
		containedType = std::make_unique<ContainedType>(*array.containedType);
	}

	ArrayType& ArrayType::operator=(const ArrayType& array)
	{
		assert(array.containedType);

		containedType = std::make_unique<ContainedType>(*array.containedType);
		length = array.length;

		return *this;
	}

	bool ArrayType::operator==(const ArrayType& rhs) const
	{
		assert(containedType);
		assert(rhs.containedType);

		if (containedType->type != rhs.containedType->type)
			return false;

		if (length != rhs.length)
			return false;

		return true;
	}

	
	MethodType::MethodType(const MethodType& methodType) :
	methodIndex(methodType.methodIndex)
	{
		assert(methodType.objectType);
		objectType = std::make_unique<ContainedType>(*methodType.objectType);
	}

	MethodType& MethodType::operator=(const MethodType& methodType)
	{
		assert(methodType.objectType);

		methodIndex = methodType.methodIndex;
		objectType = std::make_unique<ContainedType>(*methodType.objectType);

		return *this;
	}

	bool MethodType::operator==(const MethodType& rhs) const
	{
		assert(objectType);
		assert(rhs.objectType);
		return objectType->type == rhs.objectType->type && methodIndex == rhs.methodIndex;
	}
}
