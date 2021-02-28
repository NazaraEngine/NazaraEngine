// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr UInt32 s_magicNumber = 0x4E534852;
		constexpr UInt32 s_currentVersion = 1;

		class ShaderSerializerVisitor : public ShaderAstVisitor, public ShaderVarVisitor
		{
			public:
				ShaderSerializerVisitor(ShaderAstSerializerBase& serializer) :
				m_serializer(serializer)
				{
				}

				void Visit(ShaderNodes::AccessMember& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::AssignOp& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::BinaryOp& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Branch& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Cast& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::ConditionalExpression& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::ConditionalStatement& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Constant& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::DeclareVariable& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Discard& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::ExpressionStatement& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Identifier& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::IntrinsicCall& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::NoOp& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::ReturnStatement& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::Sample2D& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::StatementBlock& node) override
				{
					Serialize(node);
				}

				void Visit(ShaderNodes::SwizzleOp& node) override
				{
					Serialize(node);
				}


				void Visit(ShaderNodes::BuiltinVariable& var) override
				{
					Serialize(var);
				}

				void Visit(ShaderNodes::InputVariable& var) override
				{
					Serialize(var);
				}

				void Visit(ShaderNodes::LocalVariable& var) override
				{
					Serialize(var);
				}

				void Visit(ShaderNodes::OutputVariable& var) override
				{
					Serialize(var);
				}

				void Visit(ShaderNodes::ParameterVariable& var) override
				{
					Serialize(var);
				}

				void Visit(ShaderNodes::UniformVariable& var) override
				{
					Serialize(var);
				}

			private:
				template<typename T>
				void Serialize(const T& node)
				{
					// I know const_cast is evil but I don't have a better solution here (it's not used to write)
					m_serializer.Serialize(const_cast<T&>(node));
				}

				ShaderAstSerializerBase& m_serializer;
		};
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::AccessMember& node)
	{
		Node(node.structExpr);
		Type(node.exprType);

		Container(node.memberIndices);
		for (std::size_t& index : node.memberIndices)
			SizeT(index);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::AssignOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::BinaryOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Branch& node)
	{
		Container(node.condStatements);
		for (auto& condStatement : node.condStatements)
		{
			Node(condStatement.condition);
			Node(condStatement.statement);
		}

		Node(node.elseStatement);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::BuiltinVariable& node)
	{
		Enum(node.entry);
		Type(node.type);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Cast& node)
	{
		Enum(node.exprType);
		for (auto& expr : node.expressions)
			Node(expr);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::ConditionalExpression& node)
	{
		Value(node.conditionName);
		Node(node.truePath);
		Node(node.falsePath);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::ConditionalStatement& node)
	{
		Value(node.conditionName);
		Node(node.statement);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Constant& node)
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

		static_assert(std::variant_size_v<decltype(node.value)> == 10);
		switch (typeIndex)
		{
			case 0: SerializeValue(bool()); break;
			case 1: SerializeValue(float()); break;
			case 2: SerializeValue(Int32()); break;
			case 3: SerializeValue(UInt32()); break;
			case 4: SerializeValue(Vector2f()); break;
			case 5: SerializeValue(Vector3f()); break;
			case 6: SerializeValue(Vector4f()); break;
			case 7: SerializeValue(Vector2i32()); break;
			case 8: SerializeValue(Vector3i32()); break;
			case 9: SerializeValue(Vector4i32()); break;
			default: throw std::runtime_error("unexpected data type");
		}
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::DeclareVariable& node)
	{
		Variable(node.variable);
		Node(node.expression);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Discard& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::ExpressionStatement& node)
	{
		Node(node.expression);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Identifier& node)
	{
		Variable(node.var);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::IntrinsicCall& node)
	{
		Enum(node.intrinsic);
		Container(node.parameters);
		for (auto& param : node.parameters)
			Node(param);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::NamedVariable& node)
	{
		Value(node.name);
		Type(node.type);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::NoOp& /*node*/)
	{
		/* Nothing to do */
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::ReturnStatement& node)
	{
		Node(node.returnExpr);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::Sample2D& node)
	{
		Node(node.sampler);
		Node(node.coordinates);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::StatementBlock& node)
	{
		Container(node.statements);
		for (auto& statement : node.statements)
			Node(statement);
	}

	void ShaderAstSerializerBase::Serialize(ShaderNodes::SwizzleOp& node)
	{
		SizeT(node.componentCount);
		Node(node.expression);

		for (std::size_t i = 0; i < node.componentCount; ++i)
			Enum(node.components[i]);
	}


	void ShaderAstSerializer::Serialize(const ShaderAst& shader)
	{
		m_stream << s_magicNumber << s_currentVersion;

		m_stream << UInt32(shader.GetStage());

		auto SerializeType = [&](const ShaderExpressionType& type)
		{
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				{
					m_stream << UInt8(0);
					m_stream << UInt32(arg);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					m_stream << UInt8(1);
					m_stream << arg;
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, type);
		};

		auto SerializeInputOutput = [&](auto& inout)
		{
			m_stream << UInt32(inout.size());
			for (const auto& data : inout)
			{
				m_stream << data.name;
				SerializeType(data.type);

				m_stream << data.locationIndex.has_value();
				if (data.locationIndex)
					m_stream << UInt32(data.locationIndex.value());
			}
		};

		// Conditions
		m_stream << UInt32(shader.GetConditionCount());
		for (const auto& cond : shader.GetConditions())
			m_stream << cond.name;

		// Structs
		m_stream << UInt32(shader.GetStructCount());
		for (const auto& s : shader.GetStructs())
		{
			m_stream << s.name;
			m_stream << UInt32(s.members.size());
			for (const auto& member : s.members)
			{
				m_stream << member.name;
				SerializeType(member.type);
			}
		}

		// Inputs / Outputs
		SerializeInputOutput(shader.GetInputs());
		SerializeInputOutput(shader.GetOutputs());

		// Uniforms
		m_stream << UInt32(shader.GetUniformCount());
		for (const auto& uniform : shader.GetUniforms())
		{
			m_stream << uniform.name;
			SerializeType(uniform.type);

			m_stream << uniform.bindingIndex.has_value();
			if (uniform.bindingIndex)
				m_stream << UInt32(uniform.bindingIndex.value());

			m_stream << uniform.memoryLayout.has_value();
			if (uniform.memoryLayout)
				m_stream << UInt32(uniform.memoryLayout.value());
		}

		// Functions
		m_stream << UInt32(shader.GetFunctionCount());
		for (const auto& func : shader.GetFunctions())
		{
			m_stream << func.name;
			SerializeType(func.returnType);

			m_stream << UInt32(func.parameters.size());
			for (const auto& param : func.parameters)
			{
				m_stream << param.name;
				SerializeType(param.type);
			}

			Node(func.statement);
		}

		m_stream.FlushBits();
	}

	bool ShaderAstSerializer::IsWriting() const
	{
		return true;
	}

	void ShaderAstSerializer::Node(ShaderNodes::NodePtr& node)
	{
		ShaderNodes::NodeType nodeType = (node) ? node->GetType() : ShaderNodes::NodeType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstSerializer::Type(ShaderExpressionType& type)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
			{
				m_stream << UInt8(0);
				m_stream << UInt32(arg);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				m_stream << UInt8(1);
				m_stream << arg;
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, type);
	}

	void ShaderAstSerializer::Node(const ShaderNodes::NodePtr& node)
	{
		Node(const_cast<ShaderNodes::NodePtr&>(node)); //< Yes const_cast is ugly but it won't be used for writing
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

	void ShaderAstSerializer::Variable(ShaderNodes::VariablePtr& var)
	{
		ShaderNodes::VariableType nodeType = (var) ? var->GetType() : ShaderNodes::VariableType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (var)
		{
			ShaderSerializerVisitor visitor(*this);
			var->Visit(visitor);
		}
	}

	ShaderAst ShaderAstUnserializer::Unserialize()
	{
		UInt32 magicNumber;
		UInt32 version;
		m_stream >> magicNumber;
		if (magicNumber != s_magicNumber)
			throw std::runtime_error("invalid shader file");

		m_stream >> version;
		if (version > s_currentVersion)
			throw std::runtime_error("unsupported version");

		UInt32 shaderStage;
		m_stream >> shaderStage;

		ShaderAst shader(static_cast<ShaderStageType>(shaderStage));

		// Conditions
		UInt32 conditionCount;
		m_stream >> conditionCount;
		for (UInt32 i = 0; i < conditionCount; ++i)
		{
			std::string conditionName;
			Value(conditionName);

			shader.AddCondition(std::move(conditionName));
		}

		// Structs
		UInt32 structCount;
		m_stream >> structCount;
		for (UInt32 i = 0; i < structCount; ++i)
		{
			std::string structName;
			std::vector<ShaderAst::StructMember> members;

			Value(structName);
			Container(members);

			for (auto& member : members)
			{
				Value(member.name);
				Type(member.type);
			}

			shader.AddStruct(std::move(structName), std::move(members));
		}

		// Inputs
		UInt32 inputCount;
		m_stream >> inputCount;
		for (UInt32 i = 0; i < inputCount; ++i)
		{
			std::string inputName;
			ShaderExpressionType inputType;
			std::optional<std::size_t> location;

			Value(inputName);
			Type(inputType);
			OptVal(location);

			shader.AddInput(std::move(inputName), std::move(inputType), location);
		}

		// Outputs
		UInt32 outputCount;
		m_stream >> outputCount;
		for (UInt32 i = 0; i < outputCount; ++i)
		{
			std::string outputName;
			ShaderExpressionType outputType;
			std::optional<std::size_t> location;

			Value(outputName);
			Type(outputType);
			OptVal(location);

			shader.AddOutput(std::move(outputName), std::move(outputType), location);
		}

		// Uniforms
		UInt32 uniformCount;
		m_stream >> uniformCount;
		for (UInt32 i = 0; i < uniformCount; ++i)
		{
			std::string name;
			ShaderExpressionType type;
			std::optional<std::size_t> binding;
			std::optional<ShaderNodes::MemoryLayout> memLayout;

			Value(name);
			Type(type);
			OptVal(binding);
			OptEnum(memLayout);

			shader.AddUniform(std::move(name), std::move(type), std::move(binding), std::move(memLayout));
		}

		// Functions
		UInt32 funcCount;
		m_stream >> funcCount;
		for (UInt32 i = 0; i < funcCount; ++i)
		{
			std::string name;
			ShaderExpressionType retType;
			std::vector<ShaderAst::FunctionParameter> parameters;

			Value(name);
			Type(retType);

			Container(parameters);
			for (auto& param : parameters)
			{
				Value(param.name);
				Type(param.type);
			}

			ShaderNodes::NodePtr node;
			Node(node);
			if (!node || !node->IsStatement())
				throw std::runtime_error("functions can only have statements");

			ShaderNodes::StatementPtr statement = std::static_pointer_cast<ShaderNodes::Statement>(node);

			shader.AddFunction(std::move(name), std::move(statement), std::move(parameters), std::move(retType));
		}

		return shader;
	}

	bool ShaderAstUnserializer::IsWriting() const
	{
		return false;
	}

	void ShaderAstUnserializer::Node(ShaderNodes::NodePtr& node)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		if (nodeTypeInt < static_cast<Int32>(ShaderNodes::NodeType::None) || nodeTypeInt > static_cast<Int32>(ShaderNodes::NodeType::Max))
			throw std::runtime_error("invalid node type");

		ShaderNodes::NodeType nodeType = static_cast<ShaderNodes::NodeType>(nodeTypeInt);

#define HandleType(Type) case ShaderNodes::NodeType:: Type : node = std::make_shared<ShaderNodes:: Type>(); break
		switch (nodeType)
		{
			case ShaderNodes::NodeType::None: break;

			HandleType(AccessMember);
			HandleType(AssignOp);
			HandleType(BinaryOp);
			HandleType(Branch);
			HandleType(Cast);
			HandleType(Constant);
			HandleType(ConditionalExpression);
			HandleType(ConditionalStatement);
			HandleType(DeclareVariable);
			HandleType(Discard);
			HandleType(ExpressionStatement);
			HandleType(Identifier);
			HandleType(IntrinsicCall);
			HandleType(NoOp);
			HandleType(ReturnStatement);
			HandleType(Sample2D);
			HandleType(SwizzleOp);
			HandleType(StatementBlock);
		}
#undef HandleType

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderAstUnserializer::Type(ShaderExpressionType& type)
	{
		UInt8 typeIndex;
		Value(typeIndex);

		switch (typeIndex)
		{
			case 0: //< Primitive
			{
				ShaderNodes::BasicType exprType;
				Enum(exprType);

				type = exprType;
				break;
			}

			case 1: //< Struct (name)
			{
				std::string structName;
				Value(structName);

				type = std::move(structName);
				break;
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
	
	void ShaderAstUnserializer::Variable(ShaderNodes::VariablePtr& var)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		ShaderNodes::VariableType nodeType = static_cast<ShaderNodes:: VariableType>(nodeTypeInt);

#define HandleType(Type) case ShaderNodes::VariableType:: Type : var = std::make_shared<ShaderNodes::Type>(); break
		switch (nodeType)
		{
			case ShaderNodes::VariableType::None: break;

			HandleType(BuiltinVariable);
			HandleType(InputVariable);
			HandleType(LocalVariable);
			HandleType(ParameterVariable);
			HandleType(OutputVariable);
			HandleType(UniformVariable);
		}
#undef HandleType

		if (var)
		{
			ShaderSerializerVisitor visitor(*this);
			var->Visit(visitor);
		}
	}


	ByteArray SerializeShader(const ShaderAst& shader)
	{
		ByteArray byteArray;
		ByteStream stream(&byteArray, OpenModeFlags(OpenMode_WriteOnly));

		ShaderAstSerializer serializer(stream);
		serializer.Serialize(shader);

		return byteArray;
	}

	ShaderAst UnserializeShader(ByteStream& stream)
	{
		ShaderAstUnserializer unserializer(stream);
		return unserializer.Unserialize();
	}
}

