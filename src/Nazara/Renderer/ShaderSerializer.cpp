// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		class ShaderSerializerVisitor : public ShaderVisitor
		{
			public:
				ShaderSerializerVisitor(ShaderSerializerBase& serializer) :
				m_serializer(serializer)
				{
				}

				void Visit(const AssignOp& node) override
				{
					Serialize(node);
				}

				void Visit(const BinaryFunc& node) override
				{
					Serialize(node);
				}

				void Visit(const BinaryOp& node) override
				{
					Serialize(node);
				}

				void Visit(const Branch& node) override
				{
					Serialize(node);
				}

				void Visit(const BuiltinVariable& node) override
				{
					Serialize(node);
				}

				void Visit(const Cast& node) override
				{
					Serialize(node);
				}

				void Visit(const Constant& node) override
				{
					Serialize(node);
				}

				void Visit(const DeclareVariable& node) override
				{
					Serialize(node);
				}

				void Visit(const ExpressionStatement& node) override
				{
					Serialize(node);
				}

				void Visit(const NamedVariable& node) override
				{
					Serialize(node);
				}

				void Visit(const Sample2D& node) override
				{
					Serialize(node);
				}

				void Visit(const StatementBlock& node) override
				{
					Serialize(node);
				}

				void Visit(const SwizzleOp& node) override
				{
					Serialize(node);
				}

			private:
				template<typename T>
				void Serialize(const T& node)
				{
					// I know const_cast is evil but I don't have a better solution here (it's not used to write)
					m_serializer.Serialize(const_cast<T&>(node));
				}

				ShaderSerializerBase& m_serializer;
		};
	}

	void ShaderSerializerBase::Serialize(AssignOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderSerializerBase::Serialize(BinaryFunc& node)
	{
		Enum(node.intrinsic);
		Node(node.left);
		Node(node.right);
	}

	void ShaderSerializerBase::Serialize(BinaryOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderSerializerBase::Serialize(Branch& node)
	{
		Container(node.condStatements);
		for (auto& condStatement : node.condStatements)
		{
			Node(condStatement.condition);
			Node(condStatement.statement);
		}

		Node(node.elseStatement);
	}

	void ShaderSerializerBase::Serialize(BuiltinVariable& node)
	{
		Enum(node.var);
		Enum(node.type);
	}

	void ShaderSerializerBase::Serialize(Cast& node)
	{
		Enum(node.exprType);
		for (auto& expr : node.expressions)
			Node(expr);
	}

	void ShaderSerializerBase::Serialize(Constant& node)
	{
		Enum(node.exprType);

		switch (node.exprType)
		{
			case ExpressionType::Boolean:
				Value(node.values.bool1);
				break;

			case ExpressionType::Float1:
				Value(node.values.vec1);
				break;

			case ExpressionType::Float2:
				Value(node.values.vec2);
				break;

			case ExpressionType::Float3:
				Value(node.values.vec3);
				break;

			case ExpressionType::Float4:
				Value(node.values.vec4);
				break;
		}
	}

	void ShaderSerializerBase::Serialize(DeclareVariable& node)
	{
		Node(node.variable);
		Node(node.expression);
	}

	void ShaderSerializerBase::Serialize(ExpressionStatement& node)
	{
		Node(node.expression);
	}

	void ShaderSerializerBase::Serialize(NamedVariable& node)
	{
		Value(node.name);
		Enum(node.kind);
		Enum(node.type);
	}

	void ShaderSerializerBase::Serialize(Sample2D& node)
	{
		Node(node.sampler);
		Node(node.coordinates);
	}

	void ShaderSerializerBase::Serialize(StatementBlock& node)
	{
		Container(node.statements);
		for (auto& statement : node.statements)
			Node(statement);
	}

	void ShaderSerializerBase::Serialize(SwizzleOp& node)
	{
		Value(node.componentCount);
		Node(node.expression);

		for (std::size_t i = 0; i < node.componentCount; ++i)
			Enum(node.components[i]);
	}


	void ShaderSerializer::Serialize(const StatementPtr& shader)
	{
		assert(shader);
		m_stream << static_cast<Int32>(shader->GetType());

		ShaderSerializerVisitor visitor(*this);
		shader->Visit(visitor);

		m_stream.FlushBits();
	}

	bool ShaderSerializer::IsWriting() const
	{
		return true;
	}

	void ShaderSerializer::Node(NodePtr& node)
	{
		NodeType nodeType = (node) ? node->GetType() : NodeType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderSerializer::Value(bool& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(float& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(std::string& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(Vector2f& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(Vector3f& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(Vector4f& val)
	{
		m_stream << val;
	}

	void ShaderSerializer::Value(UInt32& val)
	{
		m_stream << val;
	}

	ByteArray Serialize(const StatementPtr& shader)
	{
		ByteArray byteArray;
		ShaderSerializer serializer(byteArray);
		serializer.Serialize(shader);

		return byteArray;
	}

	StatementPtr Unserialize(const ByteArray& data)
	{
		ShaderUnserializer unserializer(data);
		return unserializer.Unserialize();
	}

	StatementPtr ShaderUnserializer::Unserialize()
	{
		NodePtr statement;
		Node(statement);
		if (!statement || statement->GetType() != NodeType::StatementBlock)
			throw std::runtime_error("Invalid shader");

		return std::static_pointer_cast<Statement>(statement);
	}

	bool ShaderUnserializer::IsWriting() const
	{
		return false;
	}

	void ShaderUnserializer::Node(NodePtr& node)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		NodeType nodeType = static_cast<NodeType>(nodeTypeInt);

#define HandleNodeType(Type) case NodeType:: Type : node = std::make_shared<Type>(); break
		switch (nodeType)
		{
			case NodeType::None: break;

			HandleNodeType(AssignOp);
			HandleNodeType(BinaryFunc);
			HandleNodeType(BinaryOp);
			HandleNodeType(Branch);
			HandleNodeType(BuiltinVariable);
			HandleNodeType(Cast);
			HandleNodeType(Constant);
			HandleNodeType(ConditionalStatement);
			HandleNodeType(DeclareVariable);
			HandleNodeType(ExpressionStatement);
			HandleNodeType(NamedVariable);
			HandleNodeType(Sample2D);
			HandleNodeType(SwizzleOp);
			HandleNodeType(StatementBlock);
		}
#undef HandleNodeType

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderUnserializer::Value(bool& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(float& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(std::string& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(Vector2f& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(Vector3f& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(Vector4f& val)
	{
		m_stream >> val;
	}

	void ShaderUnserializer::Value(UInt32& val)
	{
		m_stream >> val;
	}
}

