// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/ShaderAstValidator.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	GlslWriter::GlslWriter() :
	m_currentState(nullptr)
	{
	}

	std::string GlslWriter::Generate(const ShaderAst& shader)
	{
		std::string error;
		if (!ValidateShader(shader, &error))
			throw std::runtime_error("Invalid shader AST: " + error);

		m_context.shader = &shader;

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		unsigned int glslVersion;
		if (m_environment.glES)
		{
			if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 2)
				glslVersion = 320;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 1)
				glslVersion = 310;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 0)
				glslVersion = 300;
			else if (m_environment.glMajorVersion >= 2 && m_environment.glMinorVersion >= 0)
				glslVersion = 100;
			else
				throw std::runtime_error("This version of OpenGL ES does not support shaders");
		}
		else
		{
			if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 3)
				glslVersion = m_environment.glMajorVersion * 100 + m_environment.glMinorVersion * 10;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 2)
				glslVersion = 150;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 1)
				glslVersion = 140;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 0)
				glslVersion = 130;
			else if (m_environment.glMajorVersion >= 2 && m_environment.glMinorVersion >= 1)
				glslVersion = 120;
			else if (m_environment.glMajorVersion >= 2 && m_environment.glMinorVersion >= 0)
				glslVersion = 110;
			else
				throw std::runtime_error("This version of OpenGL does not support shaders");
		}

		// Header
		Append("#version ");
		Append(glslVersion);
		if (m_environment.glES)
			Append(" es");

		AppendLine();
		AppendLine();

		// Extensions

		std::vector<std::string> requiredExtensions;

		if (!m_environment.glES && m_environment.extCallback)
		{
			// GL_ARB_shading_language_420pack (required for layout(binding = X))
			if (glslVersion < 420 && HasExplicitBinding(shader))
			{
				if (m_environment.extCallback("GL_ARB_shading_language_420pack"))
					requiredExtensions.emplace_back("GL_ARB_shading_language_420pack");
			}

			// GL_ARB_separate_shader_objects (required for layout(location = X))
			if (glslVersion < 410 && HasExplicitLocation(shader))
			{
				if (m_environment.extCallback("GL_ARB_separate_shader_objects"))
					requiredExtensions.emplace_back("GL_ARB_separate_shader_objects");
			}
		}

		if (!requiredExtensions.empty())
		{
			for (const std::string& ext : requiredExtensions)
				AppendLine("#extension " + ext + " : require");

			AppendLine();
		}

		if (m_environment.glES)
		{
			AppendLine("#if GL_FRAGMENT_PRECISION_HIGH");
			AppendLine("precision highp float;");
			AppendLine("#else");
			AppendLine("precision mediump float;");
			AppendLine("#endif");
			AppendLine();
		}

		// Structures
		/*if (shader.GetStructCount() > 0)
		{
			AppendCommentSection("Structures");
			for (const auto& s : shader.GetStructs())
			{
				Append("struct ");
				AppendLine(s.name);
				AppendLine("{");
				for (const auto& m : s.members)
				{
					Append("\t");
					Append(m.type);
					Append(" ");
					Append(m.name);
					AppendLine(";");
				}
				AppendLine("};");
				AppendLine();
			}
		}*/

		// Global variables (uniforms, input and outputs)
		const char* inKeyword = (glslVersion >= 130) ? "in" : "varying";
		const char* outKeyword = (glslVersion >= 130) ? "out" : "varying";

		DeclareVariables(shader, shader.GetUniforms(), "uniform", "Uniforms");
		DeclareVariables(shader, shader.GetInputs(),   inKeyword,      "Inputs");
		DeclareVariables(shader, shader.GetOutputs(),  outKeyword,     "Outputs");

		std::size_t functionCount = shader.GetFunctionCount();
		if (functionCount > 1)
		{
			AppendCommentSection("Prototypes");
			for (const auto& func : shader.GetFunctions())
			{
				if (func.name != "main")
				{
					AppendFunctionPrototype(func);
					AppendLine(";");
				}
			}
		}

		for (const auto& func : shader.GetFunctions())
			AppendFunction(func);

		return state.stream.str();
	}

	void GlslWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	void GlslWriter::Append(ShaderExpressionType type)
	{
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, type);
	}

	void GlslWriter::Append(ShaderNodes::BuiltinEntry builtin)
	{
		switch (builtin)
		{
			case ShaderNodes::BuiltinEntry::VertexPosition:
				Append("gl_Position");
				break;
		}
	}

	void GlslWriter::Append(ShaderNodes::BasicType type)
	{
		switch (type)
		{
			case ShaderNodes::BasicType::Boolean:
				Append("bool");
				break;
			case ShaderNodes::BasicType::Float1:
				Append("float");
				break;
			case ShaderNodes::BasicType::Float2:
				Append("vec2");
				break;
			case ShaderNodes::BasicType::Float3:
				Append("vec3");
				break;
			case ShaderNodes::BasicType::Float4:
				Append("vec4");
				break;
			case ShaderNodes::BasicType::Mat4x4:
				Append("mat4");
				break;
			case ShaderNodes::BasicType::Sampler2D:
				Append("sampler2D");
				break;
			case ShaderNodes::BasicType::Void:
				Append("void");
				break;
		}
	}

	void GlslWriter::Append(ShaderNodes::MemoryLayout layout)
	{
		switch (layout)
		{
			case ShaderNodes::MemoryLayout::Std140:
				Append("std140");
				break;
		}
	}

	void GlslWriter::AppendCommentSection(const std::string& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		String stars((section.size() < 33) ? (36 - section.size()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void GlslWriter::AppendFunction(const ShaderAst::Function& func)
	{
		NazaraAssert(!m_context.currentFunction, "A function is already being processed");
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		AppendFunctionPrototype(func);

		m_context.currentFunction = &func;
		CallOnExit onExit([this] ()
		{
			m_context.currentFunction = nullptr;
		});

		EnterScope();
		{
			Visit(func.statement);
		}
		LeaveScope();
	}

	void GlslWriter::AppendFunctionPrototype(const ShaderAst::Function& func)
	{
		Append(func.returnType);

		Append(" ");
		Append(func.name);

		Append("(");
		for (std::size_t i = 0; i < func.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			Append(func.parameters[i].type);
			Append(" ");
			Append(func.parameters[i].name);
		}
		Append(")\n");
	}

	void GlslWriter::AppendLine(const std::string& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << std::string(m_currentState->indentLevel, '\t');
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

	void GlslWriter::Visit(const ShaderNodes::ExpressionPtr& expr, bool encloseIfRequired)
	{
		bool enclose = encloseIfRequired && (expr->GetExpressionCategory() != ShaderNodes::ExpressionCategory::LValue);

		if (enclose)
			Append("(");

		ShaderAstVisitor::Visit(expr);

		if (enclose)
			Append(")");
	}

	void GlslWriter::Visit(const ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr, true);

		const ShaderExpressionType& exprType = node.structExpr->GetExpressionType();
		assert(std::holds_alternative<std::string>(exprType));

		const std::string& structName = std::get<std::string>(exprType);

		const auto& structs = m_context.shader->GetStructs();
		auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == structName; });
		assert(it != structs.end());

		const ShaderAst::Struct& s = *it;
		assert(node.memberIndex < s.members.size());

		const auto& member = s.members[node.memberIndex];
		Append(".");
		Append(member.name);
	}

	void GlslWriter::Visit(const ShaderNodes::AssignOp& node)
	{
		Visit(node.left);

		switch (node.op)
		{
			case ShaderNodes::AssignType::Simple:
				Append(" = ");
				break;
		}

		Visit(node.right);
	}

	void GlslWriter::Visit(const ShaderNodes::Branch& node)
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

	void GlslWriter::Visit(const ShaderNodes::BinaryOp& node)
	{
		Visit(node.left, true);

		switch (node.op)
		{
			case ShaderNodes::BinaryType::Add:
				Append(" + ");
				break;
			case ShaderNodes::BinaryType::Substract:
				Append(" - ");
				break;
			case ShaderNodes::BinaryType::Multiply:
				Append(" * ");
				break;
			case ShaderNodes::BinaryType::Divide:
				Append(" / ");
				break;
			case ShaderNodes::BinaryType::Equality:
				Append(" == ");
				break;
		}

		Visit(node.right, true);
	}

	void GlslWriter::Visit(const ShaderNodes::BuiltinVariable& var)
	{
		Append(var.entry);
	}

	void GlslWriter::Visit(const ShaderNodes::Cast& node)
	{
		Append(node.exprType);
		Append("(");

		for (std::size_t i = 0; node.expressions[i]; ++i)
		{
			if (i != 0)
				m_currentState->stream << ", ";

			const auto& exprPtr = node.expressions[i];
			NazaraAssert(exprPtr, "Invalid expression");

			Visit(exprPtr);
		}

		Append(")");
	}

	void GlslWriter::Visit(const ShaderNodes::Constant& node)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				Append((arg) ? "true" : "false");
			else if constexpr (std::is_same_v<T, float>)
				Append(std::to_string(arg));
			else if constexpr (std::is_same_v<T, Vector2f>)
				Append("vec2(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector3f>)
				Append("vec3(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector4f>)
				Append("vec4(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}

	void GlslWriter::Visit(const ShaderNodes::DeclareVariable& node)
	{
		assert(node.variable->GetType() == ShaderNodes::VariableType::LocalVariable);

		const auto& localVar = static_cast<const ShaderNodes::LocalVariable&>(*node.variable);

		Append(localVar.type);
		Append(" ");
		Append(localVar.name);
		if (node.expression)
		{
			Append(" = ");
			Visit(node.expression);
		}

		AppendLine(";");
	}

	void GlslWriter::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
		Append(";");
	}

	void GlslWriter::Visit(const ShaderNodes::Identifier& node)
	{
		Visit(node.var);
	}

	void GlslWriter::Visit(const ShaderNodes::InputVariable& var)
	{
		Append(var.name);
	}
	
	void GlslWriter::Visit(const ShaderNodes::IntrinsicCall& node)
	{
		switch (node.intrinsic)
		{
			case ShaderNodes::IntrinsicType::CrossProduct:
				Append("cross");
				break;

			case ShaderNodes::IntrinsicType::DotProduct:
				Append("dot");
				break;
		}

		Append("(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			Visit(node.parameters[i]);
		}
		Append(")");
	}

	void GlslWriter::Visit(const ShaderNodes::LocalVariable& var)
	{
		Append(var.name);
	}

	void GlslWriter::Visit(const ShaderNodes::ParameterVariable& var)
	{
		Append(var.name);
	}

	void GlslWriter::Visit(const ShaderNodes::OutputVariable& var)
	{
		Append(var.name);
	}

	void GlslWriter::Visit(const ShaderNodes::Sample2D& node)
	{
		Append("texture(");
		Visit(node.sampler);
		Append(", ");
		Visit(node.coordinates);
		Append(")");
	}

	void GlslWriter::Visit(const ShaderNodes::StatementBlock& node)
	{
		bool first = true;
		for (const ShaderNodes::StatementPtr& statement : node.statements)
		{
			if (!first)
				AppendLine();

			Visit(statement);

			first = false;
		}
	}

	void GlslWriter::Visit(const ShaderNodes::SwizzleOp& node)
	{
		Visit(node.expression);
		Append(".");

		for (std::size_t i = 0; i < node.componentCount; ++i)
		{
			switch (node.components[i])
			{
				case ShaderNodes::SwizzleComponent::First:
					Append("x");
					break;

				case ShaderNodes::SwizzleComponent::Second:
					Append("y");
					break;

				case ShaderNodes::SwizzleComponent::Third:
					Append("z");
					break;

				case ShaderNodes::SwizzleComponent::Fourth:
					Append("w");
					break;
			}
		}
	}

	void GlslWriter::Visit(const ShaderNodes::UniformVariable& var)
	{
		Append(var.name);
	}

	bool GlslWriter::HasExplicitBinding(const ShaderAst& shader)
	{
		for (const auto& uniform : shader.GetUniforms())
		{
			if (uniform.bindingIndex.has_value())
				return true;
		}

		return false;
	}

	bool GlslWriter::HasExplicitLocation(const ShaderAst& shader)
	{
		for (const auto& input : shader.GetInputs())
		{
			if (input.locationIndex.has_value())
				return true;
		}

		for (const auto& output : shader.GetOutputs())
		{
			if (output.locationIndex.has_value())
				return true;
		}

		return false;
	}
}
