// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERSERIALIZER_HPP
#define NAZARA_SHADERSERIALIZER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>
#include <Nazara/Renderer/ShaderVariables.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAstSerializerBase
	{
		public:
			ShaderAstSerializerBase() = default;
			ShaderAstSerializerBase(const ShaderAstSerializerBase&) = delete;
			ShaderAstSerializerBase(ShaderAstSerializerBase&&) = delete;
			~ShaderAstSerializerBase() = default;

			void Serialize(ShaderNodes::AccessMember& node);
			void Serialize(ShaderNodes::AssignOp& node);
			void Serialize(ShaderNodes::BinaryOp& node);
			void Serialize(ShaderNodes::BuiltinVariable& var);
			void Serialize(ShaderNodes::Branch& node);
			void Serialize(ShaderNodes::Cast& node);
			void Serialize(ShaderNodes::Constant& node);
			void Serialize(ShaderNodes::DeclareVariable& node);
			void Serialize(ShaderNodes::ExpressionStatement& node);
			void Serialize(ShaderNodes::Identifier& node);
			void Serialize(ShaderNodes::IntrinsicCall& node);
			void Serialize(ShaderNodes::NamedVariable& var);
			void Serialize(ShaderNodes::Sample2D& node);
			void Serialize(ShaderNodes::StatementBlock& node);
			void Serialize(ShaderNodes::SwizzleOp& node);

		protected:
			template<typename T> void Container(T& container);
			template<typename T> void Enum(T& enumVal);
			template<typename T> void OptEnum(std::optional<T>& optVal);
			template<typename T> void OptVal(std::optional<T>& optVal);

			virtual bool IsWriting() const = 0;

			virtual void Node(ShaderNodes::NodePtr& node) = 0;
			template<typename T> void Node(std::shared_ptr<T>& node);

			virtual void Type(ShaderExpressionType& type) = 0;

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
			inline void Value(std::size_t& val);

			virtual void Variable(ShaderNodes::VariablePtr& var) = 0;
			template<typename T> void Variable(std::shared_ptr<T>& var);
	};

	class NAZARA_RENDERER_API ShaderAstSerializer final : public ShaderAstSerializerBase
	{
		public:
			inline ShaderAstSerializer(ByteStream& stream);
			~ShaderAstSerializer() = default;

			void Serialize(const ShaderAst& shader);

		private:
			bool IsWriting() const override;
			void Node(const ShaderNodes::NodePtr& node);
			void Node(ShaderNodes::NodePtr& node) override;
			void Type(ShaderExpressionType& type) override;
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
			void Variable(ShaderNodes::VariablePtr& var) override;

			ByteStream& m_stream;
	};

	class NAZARA_RENDERER_API ShaderAstUnserializer final : public ShaderAstSerializerBase
	{
		public:
			ShaderAstUnserializer(ByteStream& stream);
			~ShaderAstUnserializer() = default;

			ShaderAst Unserialize();

		private:
			bool IsWriting() const override;
			void Node(ShaderNodes::NodePtr& node) override;
			void Type(ShaderExpressionType& type) override;
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
			void Variable(ShaderNodes::VariablePtr& var) override;

			ByteStream& m_stream;
	};
	
	NAZARA_RENDERER_API ByteArray SerializeShader(const ShaderAst& shader);
	NAZARA_RENDERER_API ShaderAst UnserializeShader(ByteStream& stream);
}

#include <Nazara/Renderer/ShaderAstSerializer.inl>

#endif
