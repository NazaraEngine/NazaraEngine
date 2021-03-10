// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVCONSTANTCACHE_HPP
#define NAZARA_SPIRVCONSTANTCACHE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/ShaderConstantValue.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/ShaderAstTypes.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Nz
{
	class SpirvSection;

	class NAZARA_SHADER_API SpirvConstantCache
	{
		public:
			SpirvConstantCache(UInt32& resultId);
			SpirvConstantCache(const SpirvConstantCache& cache) = delete;
			SpirvConstantCache(SpirvConstantCache&& cache) noexcept;
			~SpirvConstantCache();

			struct Constant;
			struct Type;

			using ConstantPtr = std::shared_ptr<Constant>;
			using TypePtr = std::shared_ptr<Type>;

			struct Bool {};

			struct Float
			{
				UInt32 width;
			};

			struct Integer
			{
				UInt32 width;
				bool signedness;
			};

			struct Void {};

			struct Vector
			{
				TypePtr componentType;
				UInt32 componentCount;
			};

			struct Matrix
			{
				TypePtr columnType;
				UInt32 columnCount;
			};

			struct Image
			{
				std::optional<SpirvAccessQualifier> qualifier;
				std::optional<bool> depth;
				std::optional<bool> sampled;
				SpirvDim dim;
				SpirvImageFormat format;
				TypePtr sampledType;
				bool arrayed;
				bool multisampled;
			};

			struct Pointer
			{
				TypePtr type;
				SpirvStorageClass storageClass;
			};

			struct Function
			{
				TypePtr returnType;
				std::vector<TypePtr> parameters;
			};

			struct SampledImage
			{
				TypePtr image;
			};

			struct Structure
			{
				struct Member
				{
					std::string name;
					TypePtr type;
				};

				std::string name;
				std::vector<Member> members;
			};

			using AnyType = std::variant<Bool, Float, Function, Image, Integer, Matrix, Pointer, SampledImage, Structure, Vector, Void>;

			struct ConstantBool
			{
				bool value;
			};

			struct ConstantComposite
			{
				TypePtr type;
				std::vector<ConstantPtr> values;
			};

			struct ConstantScalar
			{
				std::variant<float, double, Nz::Int32, Nz::Int64, Nz::UInt32, Nz::UInt64> value;
			};

			using AnyConstant = std::variant<ConstantBool, ConstantComposite, ConstantScalar>;

			struct Variable
			{
				std::string debugName;
				TypePtr type;
				SpirvStorageClass storageClass;
				std::optional<ConstantPtr> initializer;
			};

			using BaseType = std::variant<Bool, Float, Integer, Vector, Matrix, Image>;
			using CompositeValue = std::variant<ConstantBool, ConstantScalar, ConstantComposite>;
			using PointerOrBaseType = std::variant<BaseType, Pointer>;
			using PrimitiveType = std::variant<Bool, Float, Integer>;
			using ScalarType = std::variant<Float, Integer>;

			struct Constant
			{
				Constant(AnyConstant c) :
				constant(std::move(c))
				{
				}

				AnyConstant constant;
			};

			struct Type
			{
				Type(AnyType c) :
				type(std::move(c))
				{
				}

				AnyType type;
			};

			UInt32 GetId(const Constant& c);
			UInt32 GetId(const Type& t);
			UInt32 GetId(const Variable& v);

			UInt32 Register(Constant c);
			UInt32 Register(Type t);
			UInt32 Register(Variable v);

			void Write(SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos);

			SpirvConstantCache& operator=(const SpirvConstantCache& cache) = delete;
			SpirvConstantCache& operator=(SpirvConstantCache&& cache) noexcept;

			static ConstantPtr BuildConstant(const ShaderConstantValue& value);
			static TypePtr BuildPointerType(const ShaderAst::BasicType& type, SpirvStorageClass storageClass);
			static TypePtr BuildPointerType(const ShaderAst::ShaderExpressionType& type, SpirvStorageClass storageClass);
			static TypePtr BuildType(const ShaderAst::BasicType& type);
			static TypePtr BuildType(const ShaderAst::ShaderExpressionType& type);

		private:
			struct DepRegisterer;
			struct Eq;
			struct Internal;

			void Write(const AnyConstant& constant, UInt32 resultId, SpirvSection& constants);
			void Write(const AnyType& type, UInt32 resultId, SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos);

			void WriteStruct(const Structure& structData, UInt32 resultId, SpirvSection& annotations, SpirvSection& constants, SpirvSection& debugInfos);

			std::unique_ptr<Internal> m_internal;
	};
}

#include <Nazara/Shader/SpirvConstantCache.inl>

#endif
