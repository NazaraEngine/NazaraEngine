// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <Nazara/Renderer/ShaderVisitor.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr UInt32 s_magicNumber = 0x4E534852;
		constexpr UInt32 s_currentVersion = 1;

		class ShaderSerializerVisitor : public ShaderVisitor, public ShaderVarVisitor
		{
			public:
				ShaderSerializerVisitor(ShaderSerializerBase& serializer) :
				m_serializer(serializer)
				{
				}

				void Visit(const ShaderNodes::AssignOp& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::BinaryOp& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::Branch& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::Cast& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::Constant& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::DeclareVariable& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::ExpressionStatement& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::Identifier& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::IntrinsicCall& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::Sample2D& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::StatementBlock& node) override
				{
					Serialize(node);
				}

				void Visit(const ShaderNodes::SwizzleOp& node) override
				{
					Serialize(node);
				}


				void Visit(const ShaderNodes::BuiltinVariable& var) override
				{
					Serialize(var);
				}

				void Visit(const ShaderNodes::InputVariable& var) override
				{
					Serialize(var);
				}

				void Visit(const ShaderNodes::LocalVariable& var) override
				{
					Serialize(var);
				}

				void Visit(const ShaderNodes::OutputVariable& var) override
				{
					Serialize(var);
				}

				void Visit(const ShaderNodes::ParameterVariable& var) override
				{
					Serialize(var);
				}

				void Visit(const ShaderNodes::UniformVariable& var) override
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

				ShaderSerializerBase& m_serializer;
		};
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::AssignOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::BinaryOp& node)
	{
		Enum(node.op);
		Node(node.left);
		Node(node.right);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::Branch& node)
	{
		Container(node.condStatements);
		for (auto& condStatement : node.condStatements)
		{
			Node(condStatement.condition);
			Node(condStatement.statement);
		}

		Node(node.elseStatement);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::BuiltinVariable& node)
	{
		Enum(node.type);
		Enum(node.type);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::Cast& node)
	{
		Enum(node.exprType);
		for (auto& expr : node.expressions)
			Node(expr);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::Constant& node)
	{
		Enum(node.exprType);

		switch (node.exprType)
		{
			case ShaderNodes::ExpressionType::Boolean:
				Value(node.values.bool1);
				break;

			case ShaderNodes::ExpressionType::Float1:
				Value(node.values.vec1);
				break;

			case ShaderNodes::ExpressionType::Float2:
				Value(node.values.vec2);
				break;

			case ShaderNodes::ExpressionType::Float3:
				Value(node.values.vec3);
				break;

			case ShaderNodes::ExpressionType::Float4:
				Value(node.values.vec4);
				break;
		}
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::DeclareVariable& node)
	{
		Variable(node.variable);
		Node(node.expression);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::ExpressionStatement& node)
	{
		Node(node.expression);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::Identifier& node)
	{
		Variable(node.var);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::IntrinsicCall& node)
	{
		Enum(node.intrinsic);
		Container(node.parameters);
		for (auto& param : node.parameters)
			Node(param);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::NamedVariable& node)
	{
		Value(node.name);
		Enum(node.type);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::Sample2D& node)
	{
		Node(node.sampler);
		Node(node.coordinates);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::StatementBlock& node)
	{
		Container(node.statements);
		for (auto& statement : node.statements)
			Node(statement);
	}

	void ShaderSerializerBase::Serialize(ShaderNodes::SwizzleOp& node)
	{
		Value(node.componentCount);
		Node(node.expression);

		for (std::size_t i = 0; i < node.componentCount; ++i)
			Enum(node.components[i]);
	}


	void ShaderSerializer::Serialize(const ShaderAst& shader)
	{
		m_stream << s_magicNumber << s_currentVersion;

		auto SerializeInputOutput = [&](auto& inout)
		{
			m_stream << UInt32(inout.size());
			for (const auto& data : inout)
			{
				m_stream << data.name << UInt32(data.type);

				m_stream << data.locationIndex.has_value();
				if (data.locationIndex)
					m_stream << UInt32(data.locationIndex.value());
			}
		};

		SerializeInputOutput(shader.GetInputs());
		SerializeInputOutput(shader.GetOutputs());

		m_stream << UInt32(shader.GetUniformCount());
		for (const auto& uniform : shader.GetUniforms())
		{
			m_stream << uniform.name << UInt32(uniform.type);

			m_stream << uniform.bindingIndex.has_value();
			if (uniform.bindingIndex)
				m_stream << UInt32(uniform.bindingIndex.value());
		}

		m_stream << UInt32(shader.GetFunctionCount());
		for (const auto& func : shader.GetFunctions())
		{
			m_stream << func.name << UInt32(func.returnType);

			m_stream << UInt32(func.parameters.size());
			for (const auto& param : func.parameters)
				m_stream << param.name << UInt32(param.type);

			Node(func.statement);
		}

		m_stream.FlushBits();
	}

	bool ShaderSerializer::IsWriting() const
	{
		return true;
	}

	void ShaderSerializer::Node(ShaderNodes::NodePtr& node)
	{
		ShaderNodes::NodeType nodeType = (node) ? node->GetType() : ShaderNodes::NodeType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (node)
		{
			ShaderSerializerVisitor visitor(*this);
			node->Visit(visitor);
		}
	}

	void ShaderSerializer::Node(const ShaderNodes::NodePtr& node)
	{
		Node(const_cast<ShaderNodes::NodePtr&>(node)); //< Yes const_cast is ugly but it won't be used for writing
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

	void ShaderSerializer::Variable(ShaderNodes::VariablePtr& var)
	{
		ShaderNodes::VariableType nodeType = (var) ? var->GetType() : ShaderNodes::VariableType::None;
		m_stream << static_cast<Int32>(nodeType);

		if (var)
		{
			ShaderSerializerVisitor visitor(*this);
			var->Visit(visitor);
		}
	}

	ShaderAst ShaderUnserializer::Unserialize()
	{
		UInt32 magicNumber;
		UInt32 version;
		m_stream >> magicNumber;
		if (magicNumber != s_magicNumber)
			throw std::runtime_error("invalid shader file");

		m_stream >> version;
		if (version > s_currentVersion)
			throw std::runtime_error("unsupported version");

		ShaderAst shader;

		UInt32 inputCount;
		m_stream >> inputCount;
		for (UInt32 i = 0; i < inputCount; ++i)
		{
			std::string inputName;
			ShaderNodes::ExpressionType inputType;
			std::optional<std::size_t> location;

			Value(inputName);
			Enum(inputType);
			OptVal(location);

			shader.AddInput(std::move(inputName), inputType, location);
		}

		UInt32 outputCount;
		m_stream >> outputCount;
		for (UInt32 i = 0; i < outputCount; ++i)
		{
			std::string outputName;
			ShaderNodes::ExpressionType outputType;
			std::optional<std::size_t> location;

			Value(outputName);
			Enum(outputType);
			OptVal(location);

			shader.AddOutput(std::move(outputName), outputType, location);
		}

		UInt32 uniformCount;
		m_stream >> uniformCount;
		for (UInt32 i = 0; i < uniformCount; ++i)
		{
			std::string name;
			ShaderNodes::ExpressionType type;
			std::optional<std::size_t> binding;

			Value(name);
			Enum(type);
			OptVal(binding);

			shader.AddUniform(std::move(name), type, binding);
		}

		UInt32 funcCount;
		m_stream >> funcCount;
		for (UInt32 i = 0; i < funcCount; ++i)
		{
			std::string name;
			ShaderNodes::ExpressionType retType;
			std::vector<ShaderAst::FunctionParameter> parameters;

			Value(name);
			Enum(retType);
			Container(parameters);
			for (auto& param : parameters)
			{
				Value(param.name);
				Enum(param.type);
			}

			ShaderNodes::NodePtr node;
			Node(node);
			if (!node || !node->IsStatement())
				throw std::runtime_error("functions can only have statements");

			ShaderNodes::StatementPtr statement = std::static_pointer_cast<ShaderNodes::Statement>(node);

			shader.AddFunction(std::move(name), std::move(statement), std::move(parameters), retType);
		}

		return shader;
	}

	bool ShaderUnserializer::IsWriting() const
	{
		return false;
	}

	void ShaderUnserializer::Node(ShaderNodes::NodePtr& node)
	{
		Int32 nodeTypeInt;
		m_stream >> nodeTypeInt;

		ShaderNodes::NodeType nodeType = static_cast<ShaderNodes::NodeType>(nodeTypeInt);

#define HandleType(Type) case ShaderNodes::NodeType:: Type : node = std::make_shared<ShaderNodes:: Type>(); break
		switch (nodeType)
		{
			case ShaderNodes::NodeType::None: break;

			HandleType(AssignOp);
			HandleType(BinaryOp);
			HandleType(Branch);
			HandleType(Cast);
			HandleType(Constant);
			HandleType(ConditionalStatement);
			HandleType(DeclareVariable);
			HandleType(ExpressionStatement);
			HandleType(Identifier);
			HandleType(IntrinsicCall);
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

	void ShaderUnserializer::Variable(ShaderNodes::VariablePtr& var)
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

		ShaderSerializer serializer(stream);
		serializer.Serialize(shader);

		return byteArray;
	}

	ShaderAst UnserializeShader(ByteStream& stream)
	{
		ShaderUnserializer unserializer(stream);
		return unserializer.Unserialize();
	}
}

