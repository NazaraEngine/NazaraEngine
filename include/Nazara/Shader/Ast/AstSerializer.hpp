// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTSERIALIZER_HPP
#define NAZARA_SHADER_AST_ASTSERIALIZER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Module.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstSerializerBase
	{
		public:
			AstSerializerBase() = default;
			AstSerializerBase(const AstSerializerBase&) = delete;
			AstSerializerBase(AstSerializerBase&&) = delete;
			~AstSerializerBase() = default;

			void Serialize(AccessIdentifierExpression& node);
			void Serialize(AccessIndexExpression& node);
			void Serialize(AssignExpression& node);
			void Serialize(BinaryExpression& node);
			void Serialize(CallFunctionExpression& node);
			void Serialize(CallMethodExpression& node);
			void Serialize(CastExpression& node);
			void Serialize(ConstantExpression& node);
			void Serialize(ConditionalExpression& node);
			void Serialize(ConstantValueExpression& node);
			void Serialize(IdentifierExpression& node);
			void Serialize(IntrinsicExpression& node);
			void Serialize(SwizzleExpression& node);
			void Serialize(VariableExpression& node);
			void Serialize(UnaryExpression& node);

			void Serialize(BranchStatement& node);
			void Serialize(ConditionalStatement& node);
			void Serialize(DeclareConstStatement& node);
			void Serialize(DeclareExternalStatement& node);
			void Serialize(DeclareFunctionStatement& node);
			void Serialize(DeclareOptionStatement& node);
			void Serialize(DeclareStructStatement& node);
			void Serialize(DeclareVariableStatement& node);
			void Serialize(DiscardStatement& node);
			void Serialize(ExpressionStatement& node);
			void Serialize(ForStatement& node);
			void Serialize(ForEachStatement& node);
			void Serialize(MultiStatement& node);
			void Serialize(NoOpStatement& node);
			void Serialize(ReturnStatement& node);
			void Serialize(ScopedStatement& node);
			void Serialize(WhileStatement& node);

		protected:
			template<typename T> void Container(T& container);
			template<typename T> void Enum(T& enumVal);
			template<typename T> void ExprValue(ExpressionValue<T>& attribute);
			template<typename T> void OptEnum(std::optional<T>& optVal);
			template<typename T> void OptVal(std::optional<T>& optVal);

			virtual bool IsWriting() const = 0;

			virtual void Node(ExpressionPtr& node) = 0;
			virtual void Node(StatementPtr& node) = 0;

			virtual void Type(ExpressionType& type) = 0;

			virtual void Value(bool& val) = 0;
			virtual void Value(float& val) = 0;
			virtual void Value(std::string& val) = 0;
			virtual void Value(Int32& val) = 0;
			virtual void Value(Vector2f& val) = 0;
			virtual void Value(Vector3f& val) = 0;
			virtual void Value(Vector4f& val) = 0;
			virtual void Value(Vector2i32& val) = 0;
			virtual void Value(Vector3i32& val) = 0;
			virtual void Value(Vector4i32& val) = 0;
			virtual void Value(UInt8& val) = 0;
			virtual void Value(UInt16& val) = 0;
			virtual void Value(UInt32& val) = 0;
			virtual void Value(UInt64& val) = 0;
			inline void SizeT(std::size_t& val);
	};

	class NAZARA_SHADER_API ShaderAstSerializer final : public AstSerializerBase
	{
		public:
			inline ShaderAstSerializer(ByteStream& stream);
			~ShaderAstSerializer() = default;

			void Serialize(Module& shader);

		private:
			using AstSerializerBase::Serialize;

			bool IsWriting() const override;
			void Node(ExpressionPtr& node) override;
			void Node(StatementPtr& node) override;
			void Type(ExpressionType& type) override;
			void Value(bool& val) override;
			void Value(float& val) override;
			void Value(std::string& val) override;
			void Value(Int32& val) override;
			void Value(Vector2f& val) override;
			void Value(Vector3f& val) override;
			void Value(Vector4f& val) override;
			void Value(Vector2i32& val) override;
			void Value(Vector3i32& val) override;
			void Value(Vector4i32& val) override;
			void Value(UInt8& val) override;
			void Value(UInt16& val) override;
			void Value(UInt32& val) override;
			void Value(UInt64& val) override;

			ByteStream& m_stream;
	};

	class NAZARA_SHADER_API ShaderAstUnserializer final : public AstSerializerBase
	{
		public:
			ShaderAstUnserializer(ByteStream& stream);
			~ShaderAstUnserializer() = default;

			ModulePtr Unserialize();

		private:
			using AstSerializerBase::Serialize;

			bool IsWriting() const override;
			void Node(ExpressionPtr& node) override;
			void Node(StatementPtr& node) override;
			void Type(ExpressionType& type) override;
			void Value(bool& val) override;
			void Value(float& val) override;
			void Value(std::string& val) override;
			void Value(Int32& val) override;
			void Value(Vector2f& val) override;
			void Value(Vector3f& val) override;
			void Value(Vector4f& val) override;
			void Value(Vector2i32& val) override;
			void Value(Vector3i32& val) override;
			void Value(Vector4i32& val) override;
			void Value(UInt8& val) override;
			void Value(UInt16& val) override;
			void Value(UInt32& val) override;
			void Value(UInt64& val) override;

			ByteStream& m_stream;
	};
	
	NAZARA_SHADER_API ByteArray SerializeShader(Module& shader);
	inline ModulePtr UnserializeShader(const void* data, std::size_t size);
	NAZARA_SHADER_API ModulePtr UnserializeShader(ByteStream& stream);
}

#include <Nazara/Shader/Ast/AstSerializer.inl>

#endif // NAZARA_SHADER_AST_ASTSERIALIZER_HPP
