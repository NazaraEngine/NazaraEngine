// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		constexpr UInt32 s_magicNumber = 0x4E534852;
		constexpr UInt32 s_currentVersion = 1;

		class ShaderSerializerVisitor : public AstExpressionVisitor, public AstStatementVisitor
		{
			public:
				ShaderSerializerVisitor(AstSerializerBase& serializer) :
				m_serializer(serializer)
				{
				}

#define NAZARA_SHADERAST_NODE(Node) void Visit(Node& node) override \
				{ \
					m_serializer.Serialize(node); \
				}
#include <Nazara/Shader/Ast/AstNodeList.hpp>

			private:
				AstSerializerBase& m_serializer;
		};
	}

	void AstSerializerBase::Serialize(AccessIdentifierExpression& node)
	{
		Node(node.expr);

		Container(node.identifiers);
		for (std::string& identifier : node.identifiers)
			Value(identifier);
	}

	void AstSerializerBase::Serialize(AccessIndexExpression& node)
	{
		Node(node.expr);

		Container(node.indices);
		for (auto& identifier : node.indices)
			Node(identifier);
	}

	void AstSerializerBase::Serialize(AssignExpression& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void AstSerializerBase::Serialize(BinaryExpression& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void AstSerializerBase::Serialize(CallFunctionExpression& node)
	{
		Node(node.targetFunction);

		Container(node.parameters);
		for (auto& param : node.parameters)
			Node(param);
	}

	void AstSerializerBase::Serialize(CallMethodExpression& node)
	{
		Node(node.object);
		Value(node.methodName);

		Container(node.parameters);
		for (auto& param : node.parameters)
			Node(param);
	}

	void AstSerializerBase::Serialize(CastExpression& node)
	{
		ExprValue(node.targetType);
		for (auto& expr : node.expressions)
			Node(expr);
	}

	void AstSerializerBase::Serialize(ConstantExpression& node)
	{
		SizeT(node.constantId);
	}

	void AstSerializerBase::Serialize(ConditionalExpression& node)
	{
		Node(node.condition);
		Node(node.truePath);
		Node(node.falsePath);
	}
	
	void AstSerializerBase::Serialize(ConstantValueExpression& node)
	{
		UInt32 typeIndex;
		if (IsWriting())
			typeIndex = UInt32(node.value.index());

		Value(typeIndex);

		// Waiting for template lambda in C++20
		auto SerializeValue = [&](auto dummyType)
		{
			using T = std::decay_t<decltype(dummyType)>;

			auto& value = (IsWriting()) ? std::get<T>(node.value) : node.value.emplace<T>();
			Value(value);
		};

		static_assert(std::variant_size_v<decltype(node.value)> == 12);
		switch (typeIndex)
		{
			case 0:  break;
			case 1:  SerializeValue(bool()); break;
			case 2:  SerializeValue(float()); break;
			case 3:  SerializeValue(Int32()); break;
			case 4:  SerializeValue(UInt32()); break;
			case 5:  SerializeValue(Vector2f()); break;
			case 6:  SerializeValue(Vector3f()); break;
			case 7:  SerializeValue(Vector4f()); break;
			case 8:  SerializeValue(Vector2i32()); break;
			case 9:  SerializeValue(Vector3i32()); break;
			case 10: SerializeValue(Vector4i32()); break;
			case 11: SerializeValue(std::string()); break;
			default: throw std::runtime_error("unexpected data type");
		}
	}

	void AstSerializerBase::Serialize(IdentifierExpression& node)
	{
		Value(node.identifier);
	}

	void AstSerializerBase::Serialize(IntrinsicExpression& node)
	{
		Enum(node.intrinsic);
		Container(node.parameters);
		for (auto& param : node.parameters)
			Node(param);
	}

	void AstSerializerBase::Serialize(SwizzleExpression& node)
	{
		SizeT(node.componentCount);
		Node(node.expression);

		for (std::size_t i = 0; i < node.componentCount; ++i)
			Enum(node.components[i]);
	}

	void AstSerializerBase::Serialize(VariableExpression& node)
	{
		SizeT(node.variableId);
	}

	void AstSerializerBase::Serialize(UnaryExpression& node)
	{
		Enum(node.op);
		Node(node.expression);
	}


	void AstSerializerBase::Serialize(BranchStatement& node)
	{
		Container(node.condStatements);
		for (auto& condStatement : node.condStatements)
		{
			Node(condStatement.condition);
			Node(condStatement.statement);
		}

		Node(node.elseStatement);
		Value(node.isConst);
	}

	void AstSerializerBase::Serialize(ConditionalStatement& node)
	{
		Node(node.condition);
		Node(node.statement);
	}

	void AstSerializerBase::Serialize(DeclareExternalStatement& node)
	{
		ExprValue(node.bindingSet);

		Container(node.externalVars);
		for (auto& extVar : node.externalVars)
		{
			Value(extVar.name);
			OptVal(extVar.varIndex);
			ExprValue(extVar.type);
			ExprValue(extVar.bindingIndex);
			ExprValue(extVar.bindingSet);
		}
	}

	void AstSerializerBase::Serialize(DeclareConstStatement& node)
	{
		OptVal(node.constIndex);
		Value(node.name);
		ExprValue(node.type);
		Node(node.expression);
	}

	void AstSerializerBase::Serialize(DeclareFunctionStatement& node)
	{
		Value(node.name);
		ExprValue(node.returnType);
		ExprValue(node.depthWrite);
		ExprValue(node.earlyFragmentTests);
		ExprValue(node.entryStage);
		OptVal(node.funcIndex);
		OptVal(node.varIndex);

		Container(node.parameters);
		for (auto& parameter : node.parameters)
		{
			Value(parameter.name);
			ExprValue(parameter.type);
		}

		Container(node.statements);
		for (auto& statement : node.statements)
			Node(statement);
	}

	void AstSerializerBase::Serialize(DeclareOptionStatement& node)
	{
		OptVal(node.optIndex);
		Value(node.optName);
		ExprValue(node.optType);
		Node(node.defaultValue);
	}

	void AstSerializerBase::Serialize(DeclareStructStatement& node)
	{
		OptVal(node.structIndex);

		Value(node.description.name);
		ExprValue(node.description.layout);

		Container(node.description.members);
		for (auto& member : node.description.members)
		{
			Value(member.name);
			ExprValue(member.type);
			ExprValue(member.builtin);
			ExprValue(member.cond);
			ExprValue(member.locationIndex);
		}
	}
	
	void AstSerializerBase::Serialize(DeclareVariableStatement& node)
	{
		OptVal(node.varIndex);
		Value(node.varName);
		ExprValue(node.varType);
		Node(node.initialExpression);
	}

	void AstSerializerBase::Serialize(DiscardStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstSerializerBase::Serialize(ExpressionStatement& node)
	{
		Node(node.expression);
	}

	void AstSerializerBase::Serialize(ForStatement& node)
	{
		ExprValue(node.unroll);
		Value(node.varName);
		Node(node.fromExpr);
		Node(node.toExpr);
		Node(node.stepExpr);
		Node(node.statement);
	}

	void AstSerializerBase::Serialize(ForEachStatement& node)
	{
		ExprValue(node.unroll);
		Value(node.varName);
		Node(node.expression);
		Node(node.statement);
	}

	void AstSerializerBase::Serialize(MultiStatement& node)
	{
		Container(node.statements);
		for (auto& statement : node.statements)
			Node(statement);
	}

	void AstSerializerBase::Serialize(NoOpStatement& /*node*/)
	{
		/* Nothing to do */
	}

	void AstSerializerBase::Serialize(ReturnStatement& node)
	{
		Node(node.returnExpr);
	}

	void AstSerializerBase::Serialize(ScopedStatement& node)
	{
		Node(node.statement);
	}

	void AstSerializerBase::Serialize(WhileStatement& node)
	{
		ExprValue(node.unroll);
		Node(node.condition);
		Node(node.body);
	}

	void ShaderAstSerializer::Serialize(Module& module)
	{
		m_stream << s_magicNumber << s_currentVersion;

		m_stream << module.metadata->shaderLangVersion;
		Serialize(*module.rootNode);

		m_stream.FlushBits();
	}
	
	bool ShaderAstSerializer::IsWriting() const
	{
		return true;
	}

	void ShaderAstSerializer::Node(ExpressionPtr& node)
	{
		NodeType nodeType = (node) ? node->GetType() : NodeType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstSerializer::Node(StatementPtr& node)
	{
		NodeType nodeType = (node) ? node->GetType() : NodeType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstSerializer::Type(ExpressionType& type)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, NoType>)
				m_stream << UInt8(0);
			else if constexpr (std::is_same_v<T, PrimitiveType>)
			{
				m_stream << UInt8(1);
				Enum(arg);
			}
			else if constexpr (std::is_same_v<T, IdentifierType>)
			{
				m_stream << UInt8(2);
				m_stream << arg.name;
			}
			else if constexpr (std::is_same_v<T, MatrixType>)
			{
				m_stream << UInt8(3);
				SizeT(arg.columnCount);
				SizeT(arg.rowCount);
				Enum(arg.type);
			}
			else if constexpr (std::is_same_v<T, SamplerType>)
			{
				m_stream << UInt8(4);
				Enum(arg.dim);
				Enum(arg.sampledType);
			}
			else if constexpr (std::is_same_v<T, StructType>)
			{
				m_stream << UInt8(5);
				SizeT(arg.structIndex);
			}
			else if constexpr (std::is_same_v<T, UniformType>)
			{
				m_stream << UInt8(6);
				SizeT(arg.containedType.structIndex);
			}
			else if constexpr (std::is_same_v<T, VectorType>)
			{
				m_stream << UInt8(7);
				SizeT(arg.componentCount);
				Enum(arg.type);
			}
			else if constexpr (std::is_same_v<T, ArrayType>)
			{
				m_stream << UInt8(8);
				Value(arg.length);
				Type(arg.containedType->type);
			}
			else if constexpr (std::is_same_v<T, ShaderAst::Type>)
			{
				m_stream << UInt8(9);
				SizeT(arg.typeIndex);
			}
			else if constexpr (std::is_same_v<T, ShaderAst::FunctionType>)
			{
				m_stream << UInt8(10);
				SizeT(arg.funcIndex);
			}
			else if constexpr (std::is_same_v<T, ShaderAst::IntrinsicFunctionType>)
			{
				m_stream << UInt8(11);
				Enum(arg.intrinsic);
			}
			else if constexpr (std::is_same_v<T, ShaderAst::MethodType>)
			{
				m_stream << UInt8(12);
				Type(arg.objectType->type);
				SizeT(arg.methodIndex);
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, type);
	}

	void ShaderAstSerializer::Value(bool& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(float& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(std::string& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Int32& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector2f& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector3f& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector4f& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector2i32& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector3i32& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(Vector4i32& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(UInt8& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(UInt16& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(UInt32& val)
	{
		m_stream << val;
	}

	void ShaderAstSerializer::Value(UInt64& val)
	{
		m_stream << val;
	}

	ModulePtr ShaderAstUnserializer::Unserialize()
	{
		UInt32 magicNumber;
		UInt32 version;
		m_stream >> magicNumber;
		if (magicNumber != s_magicNumber)
			throw std::runtime_error("invalid shader file");

		m_stream >> version;
		if (version > s_currentVersion)
			throw std::runtime_error("unsupported version");

		ModulePtr module = std::make_shared<Module>();

		std::shared_ptr<Module::Metadata> metadata = std::make_shared<Module::Metadata>();
		m_stream >> metadata->shaderLangVersion;

		module->metadata = std::move(metadata);

		module->rootNode = ShaderBuilder::MultiStatement();
		ShaderSerializerVisitor visitor(*this);
		module->rootNode->Visit(visitor);

		return module;
	}

	bool ShaderAstUnserializer::IsWriting() const
	{
		return false;
	}

	void ShaderAstUnserializer::Node(ExpressionPtr& node)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		if (nodeTypeInt < static_cast<Int32>(NodeType::None) || nodeTypeInt > static_cast<Int32>(NodeType::Max))
			throw std::runtime_error("invalid node type");

		NodeType nodeType = static_cast<NodeType>(nodeTypeInt);
		switch (nodeType)
		{
			case NodeType::None: break;

#define NAZARA_SHADERAST_EXPRESSION(Node) case NodeType:: Node : node = std::make_unique<Node>(); break;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

			default: throw std::runtime_error("unexpected node type");
		}

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstUnserializer::Node(StatementPtr& node)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		if (nodeTypeInt < static_cast<Int32>(NodeType::None) || nodeTypeInt > static_cast<Int32>(NodeType::Max))
			throw std::runtime_error("invalid node type");

		NodeType nodeType = static_cast<NodeType>(nodeTypeInt);
		switch (nodeType)
		{
			case NodeType::None: break;

#define NAZARA_SHADERAST_STATEMENT(Node) case NodeType:: Node : node = std::make_unique<Node>(); break;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

			default: throw std::runtime_error("unexpected node type");
		}

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstUnserializer::Type(ExpressionType& type)
	{
		UInt8 typeIndex;
		Value(typeIndex);

		switch (typeIndex)
		{
			case 0: //< NoType
				type = NoType{};
				break;

			case 1: //< PrimitiveType
			{
				PrimitiveType primitiveType;
				Enum(primitiveType);

				type = primitiveType;
				break;
			}

			case 2: //< Identifier
			{
				std::string identifier;
				Value(identifier);

				type = IdentifierType{ std::move(identifier) };
				break;
			}

			case 3: //< MatrixType
			{
				std::size_t columnCount, rowCount;
				PrimitiveType primitiveType;
				SizeT(columnCount);
				SizeT(rowCount);
				Enum(primitiveType);

				type = MatrixType {
					columnCount,
					rowCount,
					primitiveType
				};
				break;
			}

			case 4: //< SamplerType
			{
				ImageType dim;
				PrimitiveType sampledType;
				Enum(dim);
				Enum(sampledType);

				type = SamplerType {
					dim,
					sampledType
				};
				break;
			}

			case 5: //< StructType
			{
				UInt32 structIndex;
				Value(structIndex);

				type = StructType{
					structIndex
				};
				break;
			}

			case 6: //< UniformType
			{
				std::size_t structIndex;
				SizeT(structIndex);

				type = UniformType {
					StructType {
						structIndex
					}
				};
				break;
			}

			case 7: //< VectorType
			{
				std::size_t componentCount;
				PrimitiveType componentType;
				SizeT(componentCount);
				Enum(componentType);

				type = VectorType{
					componentCount,
					componentType
				};
				break;
			}

			case 8: //< ArrayType
			{
				UInt32 length;
				ExpressionType containedType;
				Value(length);
				Type(containedType);

				ArrayType arrayType;
				arrayType.length = length;
				arrayType.containedType = std::make_unique<ContainedType>();
				arrayType.containedType->type = std::move(containedType);

				type = std::move(arrayType);
				break;
			}

			case 9: //< Type
			{
				std::size_t containedTypeIndex;
				SizeT(containedTypeIndex);

				type = ShaderAst::Type{
					containedTypeIndex
				};
			}

			case 10: //< FunctionType
			{
				std::size_t funcIndex;
				SizeT(funcIndex);

				type = FunctionType {
					funcIndex
				};
			}

			case 11: //< IntrinsicFunctionType
			{
				IntrinsicType intrinsicType;
				Enum(intrinsicType);

				type = IntrinsicFunctionType {
					intrinsicType
				};
			}

			case 12: //< MethodType
			{
				ExpressionType objectType;
				Type(objectType);

				std::size_t methodIndex;
				SizeT(methodIndex);

				MethodType methodType;
				methodType.objectType = std::make_unique<ContainedType>();
				methodType.objectType->type = std::move(objectType);
				methodType.methodIndex = methodIndex;

				type = std::move(methodType);
			}

			default:
				break;
		}
	}

	void ShaderAstUnserializer::Value(bool& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(float& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(std::string& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Int32& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector2f& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector3f& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector4f& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector2i32& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector3i32& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(Vector4i32& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(UInt8& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(UInt16& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(UInt32& val)
	{
		m_stream >> val;
	}

	void ShaderAstUnserializer::Value(UInt64& val)
	{
		m_stream >> val;
	}
	

	ByteArray SerializeShader(Module& module)
	{
		ByteArray byteArray;
		ByteStream stream(&byteArray, OpenModeFlags(OpenMode::WriteOnly));

		ShaderAstSerializer serializer(stream);
		serializer.Serialize(module);

		return byteArray;
	}

	ModulePtr UnserializeShader(ByteStream& stream)
	{
		ShaderAstUnserializer unserializer(stream);
		return unserializer.Unserialize();
	}
}
