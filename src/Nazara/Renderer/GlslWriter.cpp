// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/ShaderValidator.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	GlslWriter::GlslWriter() :
	m_currentFunction(nullptr),
	m_currentState(nullptr),
	m_glslVersion(110)
	{
	}

	String GlslWriter::Generate(const ShaderAst::StatementPtr& node)
	{
		std::string error;
		if (!ShaderAst::Validate(node, &error))
			throw std::runtime_error("Invalid shader AST: " + error);

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		// Register global variables (uniforms, varying, ..)
		node->Register(*this);

		// Header
		Append("#version ");
		AppendLine(String::Number(m_glslVersion));
		AppendLine();

		// Global variables (uniforms, input and outputs)
		DeclareVariables(state.uniforms, "uniform", "Uniforms");
		DeclareVariables(state.inputs,   "in",      "Inputs");
		DeclareVariables(state.outputs,  "out",     "Outputs");

		Function entryPoint;
		entryPoint.name = "main"; //< GLSL has only one entry point name possible
		entryPoint.node = node;
		entryPoint.retType = ShaderAst::ExpressionType::Void;

		AppendFunction(entryPoint);

		return state.stream;
	}

	void GlslWriter::RegisterFunction(const String& name, ShaderAst::StatementPtr statement, std::initializer_list<ShaderAst::NamedVariablePtr> parameters, ShaderAst::ExpressionType retType)
	{
		Function func;
		func.retType = retType;
		func.name = name;
		func.node = std::move(statement);
		func.parameters.assign(parameters);

		m_functions[name] = std::move(func);
	}

	void GlslWriter::RegisterVariable(ShaderAst::VariableType kind, const String& name, ShaderAst::ExpressionType type)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");
		NazaraAssert(kind != ShaderAst::VariableType::Builtin, "Builtin variables should not be registered");

		switch (kind)
		{
			case ShaderAst::VariableType::Builtin: //< Only there to make compiler happy
			case ShaderAst::VariableType::Variable:
				break;

			case ShaderAst::VariableType::Input:
				m_currentState->inputs.emplace(type, name);
				break;

			case ShaderAst::VariableType::Output:
				m_currentState->outputs.emplace(type, name);
				break;

			case ShaderAst::VariableType::Parameter:
			{
				if (m_currentFunction)
				{
					bool found = false;
					for (const auto& varPtr : m_currentFunction->parameters)
					{
						if (varPtr->name == name)
						{
							found = true;
							if (varPtr->type != type)
							{
								//TODO: AstParseError
								throw std::runtime_error("Function uses parameter \"" + name.ToStdString() + "\" with a different type than specified in the function arguments");
							}

							break;
						}
					}

					if (!found)
						//TODO: AstParseError
						throw std::runtime_error("Function has no parameter \"" + name.ToStdString() + "\"");
				}

				break;
			}

			case ShaderAst::VariableType::Uniform:
				m_currentState->uniforms.emplace(type, name);
				break;
		}
	}

	void GlslWriter::SetGlslVersion(unsigned int version)
	{
		m_glslVersion = version;
	}

	void GlslWriter::Visit(const ShaderAst::Sample2D& node)
	{
		Append("texture(");
		Visit(node.sampler);
		Append(", ");
		Visit(node.coordinates);
		Append(")");
	}

	void GlslWriter::Visit(const ShaderAst::AssignOp& node)
	{
		Visit(node.left);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple:
				Append(" = ");
				break;
		}

		Visit(node.right);
	}

	void GlslWriter::Visit(const ShaderAst::Branch& node)
	{
		bool first = true;
		for (const auto& statement : node.condStatements)
		{
			if (!first)
				Append("else ");

			Append("if (");
			Visit(statement.condition);
			AppendLine(")");

			EnterScope();
				Visit(statement.statement);
			LeaveScope();

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			EnterScope();
				Visit(node.elseStatement);
			LeaveScope();
		}
	}

	void GlslWriter::Visit(const ShaderAst::BinaryFunc& node)
	{
		switch (node.intrinsic)
		{
			case ShaderAst::BinaryIntrinsic::CrossProduct:
				Append("cross");
				break;

			case ShaderAst::BinaryIntrinsic::DotProduct:
				Append("dot");
				break;
		}

		Append("(");
		Visit(node.left);
		Append(", ");
		Visit(node.right);
		Append(")");
	}

	void GlslWriter::Visit(const ShaderAst::BinaryOp& node)
	{
		Visit(node.left);

		switch (node.op)
		{
			case ShaderAst::BinaryType::Add:
				Append(" + ");
				break;
			case ShaderAst::BinaryType::Substract:
				Append(" - ");
				break;
			case ShaderAst::BinaryType::Multiply:
				Append(" * ");
				break;
			case ShaderAst::BinaryType::Divide:
				Append(" / ");
				break;
			case ShaderAst::BinaryType::Equality:
				Append(" == ");
				break;
		}

		Visit(node.right);
	}

	void GlslWriter::Visit(const ShaderAst::BuiltinVariable& node)
	{
		Append(node.var);
	}

	void GlslWriter::Visit(const ShaderAst::Cast& node)
	{
		Append(node.exprType);
		Append("(");

		unsigned int i = 0;
		unsigned int requiredComponents = ShaderAst::Node::GetComponentCount(node.exprType);
		while (requiredComponents > 0)
		{
			if (i != 0)
				m_currentState->stream << ", ";

			const auto& exprPtr = node.expressions[i++];
			NazaraAssert(exprPtr, "Invalid expression");

			Visit(exprPtr);
			requiredComponents -= ShaderAst::Node::GetComponentCount(exprPtr->GetExpressionType());
		}

		Append(")");
	}

	void GlslWriter::Visit(const ShaderAst::Constant& node)
	{
		switch (node.exprType)
		{
			case ShaderAst::ExpressionType::Boolean:
				Append((node.values.bool1) ? "true" : "false");
				break;

			case ShaderAst::ExpressionType::Float1:
				Append(String::Number(node.values.vec1));
				break;

			case ShaderAst::ExpressionType::Float2:
				Append("vec2(" + String::Number(node.values.vec2.x) + ", " + String::Number(node.values.vec2.y) + ")");
				break;

			case ShaderAst::ExpressionType::Float3:
				Append("vec3(" + String::Number(node.values.vec3.x) + ", " + String::Number(node.values.vec3.y) + ", " + String::Number(node.values.vec3.z) + ")");
				break;

			case ShaderAst::ExpressionType::Float4:
				Append("vec4(" + String::Number(node.values.vec4.x) + ", " + String::Number(node.values.vec4.y) + ", " + String::Number(node.values.vec4.z) + ", " + String::Number(node.values.vec4.w) + ")");
				break;

			default:
				throw std::runtime_error("Unhandled expression type");
		}
	}

	void GlslWriter::Visit(const ShaderAst::DeclareVariable& node)
	{
		Append(node.variable->GetExpressionType());
		Append(" ");
		Append(node.variable->name);
		if (node.expression)
		{
			Append(" ");
			Append("=");
			Append(" ");
			Visit(node.expression);
		}

		AppendLine(";");
	}

	void GlslWriter::Visit(const ShaderAst::ExpressionStatement& node)
	{
		Visit(node.expression);
		Append(";");
	}

	void GlslWriter::Visit(const ShaderAst::NamedVariable& node)
	{
		Append(node.name);
	}

	void GlslWriter::Visit(const ShaderAst::StatementBlock& node)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : node.statements)
		{
			if (!first)
				AppendLine();

			Visit(statement);

			first = false;
		}
	}

	void GlslWriter::Visit(const ShaderAst::SwizzleOp& node)
	{
		Visit(node.expression);
		Append(".");

		for (std::size_t i = 0; i < node.componentCount; ++i)
		{
			switch (node.components[i])
			{
				case ShaderAst::SwizzleComponent::First:
					Append("x");
					break;

				case ShaderAst::SwizzleComponent::Second:
					Append("y");
					break;

				case ShaderAst::SwizzleComponent::Third:
					Append("z");
					break;

				case ShaderAst::SwizzleComponent::Fourth:
					Append("w");
					break;
			}
		}
	}

	void GlslWriter::Append(ShaderAst::BuiltinEntry builtin)
	{
		switch (builtin)
		{
			case ShaderAst::BuiltinEntry::VertexPosition:
				Append("gl_Position");
				break;
		}
	}

	void GlslWriter::Append(ShaderAst::ExpressionType type)
	{
		switch (type)
		{
			case ShaderAst::ExpressionType::Boolean:
				Append("bool");
				break;
			case ShaderAst::ExpressionType::Float1:
				Append("float");
				break;
			case ShaderAst::ExpressionType::Float2:
				Append("vec2");
				break;
			case ShaderAst::ExpressionType::Float3:
				Append("vec3");
				break;
			case ShaderAst::ExpressionType::Float4:
				Append("vec4");
				break;
			case ShaderAst::ExpressionType::Mat4x4:
				Append("mat4");
				break;
			case ShaderAst::ExpressionType::Sampler2D:
				Append("sampler2D");
				break;
			case ShaderAst::ExpressionType::Void:
				Append("void");
				break;
		}
	}

	void GlslWriter::Append(const String& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt;
	}

	void GlslWriter::AppendCommentSection(const String& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		String stars((section.GetSize() < 33) ? (36 - section.GetSize()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void GlslWriter::AppendFunction(Function& func)
	{
		NazaraAssert(!m_currentFunction, "A function is already being processed");
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentFunction = &func;
		CallOnExit onExit([this] ()
		{
			m_currentFunction = nullptr;
		});

		func.node->Register(*this);

		Append(func.retType);

		m_currentState->stream << ' ';
		Append(func.name);

		m_currentState->stream << '(';
		for (std::size_t i = 0; i < func.parameters.size(); ++i)
		{
			if (i != 0)
				m_currentState->stream << ", ";

			Append(func.parameters[i]->type);
			m_currentState->stream << ' ';
			Append(func.parameters[i]->name);
		}
		m_currentState->stream << ")\n";

		EnterScope();
		{
			Visit(func.node);
		}
		LeaveScope();
	}

	void GlslWriter::AppendLine(const String& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << String(m_currentState->indentLevel, '\t');
	}

	void GlslWriter::DeclareVariables(const VariableContainer& variables, const String& keyword, const String& section)
	{
		if (!variables.empty())
		{
			if (!section.IsEmpty())
				AppendCommentSection(section);

			for (const auto& pair : variables)
			{
				if (!keyword.IsEmpty())
				{
					Append(keyword);
					Append(" ");
				}

				Append(pair.first);
				Append(" ");
				Append(pair.second);
				AppendLine(";");
			}

			AppendLine();
		}
	}

	void GlslWriter::EnterScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel++;
		AppendLine("{");
	}

	void GlslWriter::LeaveScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel--;
		AppendLine();
		AppendLine("}");
	}

}
