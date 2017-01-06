// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
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
		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

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
		entryPoint.retType = ShaderAst::ExpressionType::None;

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
			case ShaderAst::VariableType::Input:
				m_currentState->inputs.insert(std::make_pair(type, name));
				break;

			case ShaderAst::VariableType::Output:
				m_currentState->outputs.insert(std::make_pair(type, name));
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
								throw std::runtime_error("Function uses parameter \"" + name + "\" with a different type than specified in the function arguments");
							}

							break;
						}
					}

					if (!found)
						//TODO: AstParseError
						throw std::runtime_error("Function has no parameter \"" + name + "\"");
				}

				break;
			}

			case ShaderAst::VariableType::Uniform:
				m_currentState->uniforms.insert(std::make_pair(type, name));
				break;

			case ShaderAst::VariableType::Variable:
			{
				if (m_currentFunction)
					m_currentFunction->variables.insert(std::make_pair(type, name));

				break;
			}
		}
	}

	void GlslWriter::SetGlslVersion(unsigned int version)
	{
		m_glslVersion = version;
	}

	void GlslWriter::Write(const ShaderAst::NodePtr& node)
	{
		node->Visit(*this);
	}

	void GlslWriter::Write(const ShaderAst::AssignOp& node)
	{
		Write(node.variable);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple:
				Append(" = ");
				break;
		}

		Write(node.right);
	}

	void GlslWriter::Write(const ShaderAst::Branch& node)
	{
		bool first = true;
		for (const auto& statement : node.condStatements)
		{
			if (!first)
				Append("else ");

			Append("if (");
			Write(statement.condition);
			AppendLine(")");

			EnterScope();
				Write(statement.statement);
			LeaveScope();

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			EnterScope();
				Write(node.elseStatement);
			LeaveScope();
		}
	}

	void GlslWriter::Write(const ShaderAst::BinaryOp& node)
	{
		Write(node.left);

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

		Write(node.right);
	}

	void GlslWriter::Write(const ShaderAst::BuiltinVariable& node)
	{
		Append(node.var);
	}

	void GlslWriter::Write(const ShaderAst::Cast& node)
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

			Write(exprPtr);
			requiredComponents -= ShaderAst::Node::GetComponentCount(exprPtr->GetExpressionType());
		}

		Append(")");
	}

	void GlslWriter::Write(const ShaderAst::Constant& node)
	{
		switch (node.exprType)
		{
			case ShaderAst::ExpressionType::Boolean:
				Append((node.values.bool1) ? "true" : "false");
				break;

			case ShaderAst::ExpressionType::Float1:
				Append(String::Format("%F", node.values.vec1));
				break;

			case ShaderAst::ExpressionType::Float2:
				Append(String::Format("vec2(%F, %F)", node.values.vec2.x, node.values.vec2.y));
				break;

			case ShaderAst::ExpressionType::Float3:
				Append(String::Format("vec3(%F, %F, %F)", node.values.vec3.x, node.values.vec3.y, node.values.vec3.z));
				break;

			case ShaderAst::ExpressionType::Float4:
				Append(String::Format("vec4(%F, %F, %F, %F)", node.values.vec4.x, node.values.vec4.y, node.values.vec4.z, node.values.vec4.w));
				break;
		}

		throw std::runtime_error("Unhandled expression type");
	}

	void GlslWriter::Write(const ShaderAst::ExpressionStatement& node)
	{
		Write(node.expression);
		Append(";");
	}

	void GlslWriter::Write(const ShaderAst::NamedVariable& node)
	{
		Append(node.name);
	}

	void GlslWriter::Write(const ShaderAst::StatementBlock& node)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : node.statements)
		{
			if (!first)
				AppendLine();

			Write(statement);

			first = false;
		}
	}

	void GlslWriter::Append(ShaderAst::Builtin builtin)
	{
		switch (builtin)
		{
			case ShaderAst::Builtin::VertexPosition:
				Append("gl_Position");
				break;
		}
	}

	void GlslWriter::Append(ShaderAst::ExpressionType type)
	{
		switch (type)
		{
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
			case ShaderAst::ExpressionType::None:
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
			DeclareVariables(func.variables);

			Write(func.node);
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
