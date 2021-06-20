// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERSERIALIZER_HPP
#define NAZARA_SHADERSERIALIZER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>

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
			void Serialize(ConditionalExpression& node);
			void Serialize(ConstantExpression& node);
			void Serialize(IdentifierExpression& node);
			void Serialize(IntrinsicExpression& node);
			void Serialize(SelectOptionExpression& node);
			void Serialize(SwizzleExpression& node);
			void Serialize(VariableExpression& node);
			void Serialize(UnaryExpression& node);

			void Serialize(BranchStatement& node);
			void Serialize(ConditionalStatement& node);
			void Serialize(DeclareExternalStatement& node);
			void Serialize(DeclareFunctionStatement& node);
			void Serialize(DeclareOptionStatement& node);
			void Serialize(DeclareStructStatement& node);
			void Serialize(DeclareVariableStatement& node);
			void Serialize(DiscardStatement& node);
			void Serialize(ExpressionStatement& node);
			void Serialize(MultiStatement& node);
			void Serialize(NoOpStatement& node);
			void Serialize(ReturnStatement& node);

		protected:
			template<typename T> void Container(T& container);
			template<typename T> void Enum(T& enumVal);
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

			void Serialize(StatementPtr& shader);

		private:
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

			StatementPtr Unserialize();

		private:
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
	
	NAZARA_SHADER_API ByteArray SerializeShader(StatementPtr& shader);
	inline StatementPtr UnserializeShader(const void* data, std::size_t size);
	NAZARA_SHADER_API StatementPtr UnserializeShader(ByteStream& stream);
}

#include <Nazara/Shader/Ast/AstSerializer.inl>

#endif
