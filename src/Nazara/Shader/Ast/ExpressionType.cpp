// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstCompare.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	AliasType::AliasType(const AliasType& alias) :
	aliasIndex(alias.aliasIndex)
	{
		assert(alias.targetType);
		targetType = std::make_unique<ContainedType>(*alias.targetType);
	}

	AliasType& AliasType::operator=(const AliasType& alias)
	{
		aliasIndex = alias.aliasIndex;

		assert(alias.targetType);
		targetType = std::make_unique<ContainedType>(*alias.targetType);

		return *this;
	}

	bool AliasType::operator==(const AliasType& rhs) const
	{
		assert(targetType);
		assert(rhs.targetType);

		if (aliasIndex != rhs.aliasIndex)
			return false;

		if (targetType->type != rhs.targetType->type)
			return false;

		return true;
	}
	
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

		if (length != rhs.length)
			return false;

		if (containedType->type != rhs.containedType->type)
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

	std::string ToString(const AliasType& type, const Stringifier& stringifier)
	{
		std::string str = "alias ";
		if (stringifier.aliasStringifier)
			str += stringifier.aliasStringifier(type.aliasIndex);
		else
		{
			str += "#";
			str += std::to_string(type.aliasIndex);
		}

		str += " -> ";
		str += ToString(type.targetType->type);

		return str;
	}

	std::string ToString(const ArrayType& type, const Stringifier& stringifier)
	{
		std::string str = "array[";
		str += ToString(type.containedType->type, stringifier);
		str += ", ";
		str += std::to_string(type.length);
		str += "]";

		return str;
	}

	std::string ShaderAst::ToString(const ExpressionType& type, const Stringifier& stringifier)
	{
		return std::visit([&](auto&& arg)
		{
			return ToString(arg, stringifier);
		}, type);
	}

	std::string ToString(const FunctionType& /*type*/, const Stringifier& /*stringifier*/)
	{
		return "<function type>";
	}

	std::string ToString(const IntrinsicFunctionType& /*type*/, const Stringifier& /*stringifier*/)
	{
		return "<intrinsic function type>";
	}

	std::string ToString(const MatrixType& type, const Stringifier& /*stringifier*/)
	{
		std::string str = "mat";
		if (type.columnCount == type.rowCount)
			str += std::to_string(type.columnCount);
		else
		{
			str += std::to_string(type.columnCount);
			str += "x";
			str += std::to_string(type.rowCount);
		}

		str += "[";
		str += ToString(type.type);
		str += "]";

		return str;
	}

	std::string ToString(const MethodType& type, const Stringifier& /*stringifier*/)
	{
		return "<method of object " + ToString(type.objectType->type) + " type>";
	}

	std::string ToString(NoType /*type*/, const Stringifier& /*stringifier*/)
	{
		return "()";
	}

	std::string ToString(PrimitiveType type, const Stringifier& /*stringifier*/)
	{
		switch (type)
		{
			case ShaderAst::PrimitiveType::Boolean: return "bool";
			case ShaderAst::PrimitiveType::Float32: return "f32";
			case ShaderAst::PrimitiveType::Int32:   return "i32";
			case ShaderAst::PrimitiveType::UInt32:  return "u32";
			case ShaderAst::PrimitiveType::String:  return "string";
		}

		return "<unhandled primitive type>";
	}

	std::string ToString(const SamplerType& type, const Stringifier& /*stringifier*/)
	{
		std::string str = "sampler";

		switch (type.dim)
		{
			case ImageType::E1D:       str += "1D";      break;
			case ImageType::E1D_Array: str += "1DArray"; break;
			case ImageType::E2D:       str += "2D";      break;
			case ImageType::E2D_Array: str += "2DArray"; break;
			case ImageType::E3D:       str += "3D";      break;
			case ImageType::Cubemap:   str += "Cube";    break;
		}

		str += "[";
		str += ToString(type.sampledType);
		str += "]";

		return str;
	}

	std::string ToString(const StructType& type, const Stringifier& stringifier)
	{
		if (stringifier.structStringifier)
			return "struct " + stringifier.structStringifier(type.structIndex);
		else
			return "struct #" + std::to_string(type.structIndex);
	}

	std::string ToString(const Type& type, const Stringifier& stringifier)
	{
		if (stringifier.typeStringifier)
			return "type " + stringifier.typeStringifier(type.typeIndex);
		else
			return "type #" + std::to_string(type.typeIndex);
	}

	std::string ToString(const UniformType& type, const Stringifier& stringifier)
	{
		std::string str = "uniform[";
		str += ToString(type.containedType, stringifier);
		str += "]";

		return str;
	}

	std::string ToString(const VectorType& type, const Stringifier& /*stringifier*/)
	{
		std::string str = "vec";
		str += std::to_string(type.componentCount);
		str += "[";
		str += ToString(type.type);
		str += "]";

		return str;
	}
}
