// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Shader/ShaderAstUtils.hpp>
#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <optional>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		static const char* flipYUniformName = "_NzFlipValue";

		struct AstAdapter : ShaderAst::AstCloner
		{
			void Visit(ShaderAst::AssignExpression& node) override
			{
				if (!flipYPosition)
					return AstCloner::Visit(node);

				if (node.left->GetType() != ShaderAst::NodeType::IdentifierExpression)
					return AstCloner::Visit(node);

				/*
				FIXME:
				const auto& identifier = static_cast<const ShaderAst::Identifier&>(*node.left);
				if (identifier.var->GetType() != ShaderAst::VariableType::BuiltinVariable)
					return ShaderAstCloner::Visit(node);

				const auto& builtinVar = static_cast<const ShaderAst::BuiltinVariable&>(*identifier.var);
				if (builtinVar.entry != ShaderAst::BuiltinEntry::VertexPosition)
					return ShaderAstCloner::Visit(node);

				auto flipVar = ShaderBuilder::Uniform(flipYUniformName, ShaderAst::BasicType::Float1);

				auto oneConstant = ShaderBuilder::Constant(1.f);
				auto fixYValue = ShaderBuilder::Cast<ShaderAst::BasicType::Float4>(oneConstant, ShaderBuilder::Identifier(flipVar), oneConstant, oneConstant);
				auto mulFix = ShaderBuilder::Multiply(CloneExpression(node.right), fixYValue);

				PushExpression(ShaderAst::AssignOp::Build(node.op, CloneExpression(node.left), mulFix));*/
			}

			bool flipYPosition = false;
		};
	}


	struct GlslWriter::State
	{
		const States* states = nullptr;
		ShaderAst::AstCache cache;
		std::stringstream stream;
		unsigned int indentLevel = 0;
	};


	GlslWriter::GlslWriter() :
	m_currentState(nullptr)
	{
	}

	std::string GlslWriter::Generate(ShaderAst::StatementPtr& shader, const States& conditions)
	{
		/*const ShaderAst* selectedShader = &inputShader;
		std::optional<ShaderAst> modifiedShader;
		if (inputShader.GetStage() == ShaderStageType::Vertex && m_environment.flipYPosition)
		{
			modifiedShader.emplace(inputShader);

			modifiedShader->AddUniform(flipYUniformName, ShaderAst::BasicType::Float1);

			selectedShader = &modifiedShader.value();
		}*/
		
		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		std::string error;
		if (!ShaderAst::ValidateAst(shader, &error, &state.cache))
			throw std::runtime_error("Invalid shader AST: " + error);

		unsigned int glslVersion;
		if (m_environment.glES)
		{
			if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 2)
				glslVersion = 320;
			else if (m_environment.glMajorVersion >= 3 && m_environment.glMinorVersion >= 1)
				glslVersion = 310;
			else if (m_environment.glMajorVersion >= 3)
				glslVersion = 300;
			else if (m_environment.glMajorVersion >= 2)
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
			else if (m_environment.glMajorVersion >= 3)
				glslVersion = 130;
			else if (m_environment.glMajorVersion >= 2 && m_environment.glMinorVersion >= 1)
				glslVersion = 120;
			else if (m_environment.glMajorVersion >= 2)
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

		shader->Visit(*this);

		return state.stream.str();
	}

	void GlslWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	const char* GlslWriter::GetFlipYUniformName()
	{
		return flipYUniformName;
	}

	void GlslWriter::Append(ShaderAst::ShaderExpressionType type)
	{
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, type);
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

	void GlslWriter::Append(ShaderAst::BasicType type)
	{
		switch (type)
		{
			case ShaderAst::BasicType::Boolean:   return Append("bool");
			case ShaderAst::BasicType::Float1:    return Append("float");
			case ShaderAst::BasicType::Float2:    return Append("vec2");
			case ShaderAst::BasicType::Float3:    return Append("vec3");
			case ShaderAst::BasicType::Float4:    return Append("vec4");
			case ShaderAst::BasicType::Int1:      return Append("int");
			case ShaderAst::BasicType::Int2:      return Append("ivec2");
			case ShaderAst::BasicType::Int3:      return Append("ivec3");
			case ShaderAst::BasicType::Int4:      return Append("ivec4");
			case ShaderAst::BasicType::Mat4x4:    return Append("mat4");
			case ShaderAst::BasicType::Sampler2D: return Append("sampler2D");
			case ShaderAst::BasicType::UInt1:     return Append("uint");
			case ShaderAst::BasicType::UInt2:     return Append("uvec2");
			case ShaderAst::BasicType::UInt3:     return Append("uvec3");
			case ShaderAst::BasicType::UInt4:     return Append("uvec4");
			case ShaderAst::BasicType::Void:      return Append("void");
		}
	}

	void GlslWriter::Append(ShaderAst::MemoryLayout layout)
	{
		switch (layout)
		{
			case ShaderAst::MemoryLayout::Std140:
				Append("std140");
				break;
		}
	}

	template<typename T>
	void GlslWriter::Append(const T& param)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << param;
	}

	void GlslWriter::AppendCommentSection(const std::string& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		std::string stars((section.size() < 33) ? (36 - section.size()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void GlslWriter::AppendField(std::size_t scopeId, const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers)
	{
		Append(".");
		Append(memberIdentifier[0]);

		const ShaderAst::AstCache::Identifier* identifier = m_currentState->cache.FindIdentifier(scopeId, structName);
		assert(identifier);

		assert(std::holds_alternative<ShaderAst::StructDescription>(identifier->value));
		const auto& s = std::get<ShaderAst::StructDescription>(identifier->value);

		auto memberIt = std::find_if(s.members.begin(), s.members.begin(), [&](const auto& field) { return field.name == memberIdentifier[0]; });
		assert(memberIt != s.members.end());

		const auto& member = *memberIt;

		if (remainingMembers > 1)
			AppendField(scopeId, std::get<std::string>(member.type), memberIdentifier + 1, remainingMembers - 1);
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

	void GlslWriter::Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired)
	{
		bool enclose = encloseIfRequired && (GetExpressionCategory(*expr) != ShaderAst::ExpressionCategory::LValue);

		if (enclose)
			Append("(");

		expr->Visit(*this);

		if (enclose)
			Append(")");
	}

	void GlslWriter::Visit(ShaderAst::AccessMemberExpression& node)
	{
		Visit(node.structExpr, true);

		const ShaderAst::ShaderExpressionType& exprType = GetExpressionType(*node.structExpr, &m_currentState->cache);
		assert(IsStructType(exprType));

		std::size_t scopeId = m_currentState->cache.GetScopeId(&node);

		AppendField(scopeId, std::get<std::string>(exprType), node.memberIdentifiers.data(), node.memberIdentifiers.size());
	}

	void GlslWriter::Visit(ShaderAst::AssignExpression& node)
	{
		node.left->Visit(*this);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple:
				Append(" = ");
				break;
		}

		node.left->Visit(*this);
	}

	void GlslWriter::Visit(ShaderAst::BranchStatement& node)
	{
		bool first = true;
		for (const auto& statement : node.condStatements)
		{
			if (!first)
				Append("else ");

			Append("if (");
			statement.condition->Visit(*this);
			AppendLine(")");

			EnterScope();
			statement.statement->Visit(*this);
			LeaveScope();

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			EnterScope();
			node.elseStatement->Visit(*this);
			LeaveScope();
		}
	}

	void GlslWriter::Visit(ShaderAst::BinaryExpression& node)
	{
		Visit(node.left, true);

		switch (node.op)
		{
			case ShaderAst::BinaryType::Add:       Append(" + "); break;
			case ShaderAst::BinaryType::Subtract:  Append(" - "); break;
			case ShaderAst::BinaryType::Multiply:  Append(" * "); break;
			case ShaderAst::BinaryType::Divide:    Append(" / "); break;

			case ShaderAst::BinaryType::CompEq:    Append(" == "); break;
			case ShaderAst::BinaryType::CompGe:    Append(" >= "); break;
			case ShaderAst::BinaryType::CompGt:    Append(" > ");  break;
			case ShaderAst::BinaryType::CompLe:    Append(" <= "); break;
			case ShaderAst::BinaryType::CompLt:    Append(" < ");  break;
			case ShaderAst::BinaryType::CompNe:    Append(" != "); break;
		}

		Visit(node.right, true);
	}

	void GlslWriter::Visit(ShaderAst::CastExpression& node)
	{
		Append(node.targetType);
		Append("(");

		bool first = true;
		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			if (!first)
				m_currentState->stream << ", ";

			exprPtr->Visit(*this);
			first = false;
		}

		Append(")");
	}

	void GlslWriter::Visit(ShaderAst::ConditionalExpression& node)
	{
		/*std::size_t conditionIndex = m_context.shader->FindConditionByName(node.conditionName);
		assert(conditionIndex != ShaderAst::InvalidCondition);

		if (TestBit<Nz::UInt64>(m_context.states->enabledConditions, conditionIndex))
			Visit(node.truePath);
		else
			Visit(node.falsePath);*/
	}

	void GlslWriter::Visit(ShaderAst::ConditionalStatement& node)
	{
		/*std::size_t conditionIndex = m_context.shader->FindConditionByName(node.conditionName);
		assert(conditionIndex != ShaderAst::InvalidCondition);

		if (TestBit<Nz::UInt64>(m_context.states->enabledConditions, conditionIndex))
			Visit(node.statement);*/
	}

	void GlslWriter::Visit(ShaderAst::ConstantExpression& node)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, Vector2i32> || std::is_same_v<T, Vector3i32> || std::is_same_v<T, Vector4i32>)
				Append("i"); //< for ivec

			if constexpr (std::is_same_v<T, bool>)
				Append((arg) ? "true" : "false");
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, Int32> || std::is_same_v<T, UInt32>)
				Append(std::to_string(arg));
			else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i32>)
				Append("vec2(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i32>)
				Append("vec3(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i32>)
				Append("vec4(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}

	void GlslWriter::Visit(ShaderAst::DeclareFunctionStatement& node)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		Append(node.returnType);
		Append(" ");
		Append(node.name);
		Append("(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");
			Append(node.parameters[i].type);
			Append(" ");
			Append(node.parameters[i].name);
		}
		Append(")\n");

		EnterScope();
		{
			AstAdapter adapter;
			adapter.flipYPosition = m_environment.flipYPosition;

			for (auto& statement : node.statements)
				adapter.Clone(statement)->Visit(*this);
		}
		LeaveScope();
	}

	void GlslWriter::Visit(ShaderAst::DeclareVariableStatement& node)
	{
		Append(node.varType);
		Append(" ");
		Append(node.varName);
		if (node.initialExpression)
		{
			Append(" = ");
			node.initialExpression->Visit(*this);
		}

		AppendLine(";");
	}

	void GlslWriter::Visit(ShaderAst::DiscardStatement& /*node*/)
	{
		Append("discard;");
	}

	void GlslWriter::Visit(ShaderAst::ExpressionStatement& node)
	{
		node.expression->Visit(*this);
		Append(";");
	}

	void GlslWriter::Visit(ShaderAst::IdentifierExpression& node)
	{
		Append(node.identifier);
	}

	void GlslWriter::Visit(ShaderAst::IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case ShaderAst::IntrinsicType::CrossProduct:
				Append("cross");
				break;

			case ShaderAst::IntrinsicType::DotProduct:
				Append("dot");
				break;
		}

		Append("(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			node.parameters[i]->Visit(*this);
		}
		Append(")");
	}

	void GlslWriter::Visit(ShaderAst::MultiStatement& node)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : node.statements)
		{
			if (!first && statement->GetType() != ShaderAst::NodeType::NoOpStatement)
				AppendLine();

			statement->Visit(*this);

			first = false;
		}
	}

	void GlslWriter::Visit(ShaderAst::NoOpStatement& /*node*/)
	{
		/* nothing to do */
	}

	void GlslWriter::Visit(ShaderAst::ReturnStatement& node)
	{
		if (node.returnExpr)
		{
			Append("return ");
			node.returnExpr->Visit(*this);
			Append(";");
		}
		else
			Append("return;");
	}

	void GlslWriter::Visit(ShaderAst::SwizzleExpression& node)
	{
		Visit(node.expression, true);
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

	bool GlslWriter::HasExplicitBinding(ShaderAst::StatementPtr& shader)
	{
		/*for (const auto& uniform : shader.GetUniforms())
		{
			if (uniform.bindingIndex.has_value())
				return true;
		}*/

		return false;
	}

	bool GlslWriter::HasExplicitLocation(ShaderAst::StatementPtr& shader)
	{
		/*for (const auto& input : shader.GetInputs())
		{
			if (input.locationIndex.has_value())
				return true;
		}

		for (const auto& output : shader.GetOutputs())
		{
			if (output.locationIndex.has_value())
				return true;
		}*/

		return false;
	}
}
