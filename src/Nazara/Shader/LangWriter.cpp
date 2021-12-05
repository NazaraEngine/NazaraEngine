// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/LangWriter.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <optional>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<typename T> const T& Retrieve(const std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}
	}

	struct LangWriter::BindingAttribute
	{
		const ShaderAst::AttributeValue<UInt32>& bindingIndex;

		inline bool HasValue() const { return bindingIndex.HasValue(); }
	};

	struct LangWriter::BuiltinAttribute
	{
		const ShaderAst::AttributeValue<ShaderAst::BuiltinEntry>& builtin;

		inline bool HasValue() const { return builtin.HasValue(); }
	};

	struct LangWriter::DepthWriteAttribute
	{
		const ShaderAst::AttributeValue<ShaderAst::DepthWriteMode>& writeMode;

		inline bool HasValue() const { return writeMode.HasValue(); }
	};

	struct LangWriter::EarlyFragmentTestsAttribute
	{
		const ShaderAst::AttributeValue<bool>& earlyFragmentTests;

		inline bool HasValue() const { return earlyFragmentTests.HasValue(); }
	};

	struct LangWriter::EntryAttribute
	{
		const ShaderAst::AttributeValue<ShaderStageType>& stageType;

		inline bool HasValue() const { return stageType.HasValue(); }
	};

	struct LangWriter::LayoutAttribute
	{
		const ShaderAst::AttributeValue<StructLayout>& layout;

		inline bool HasValue() const { return layout.HasValue(); }
	};

	struct LangWriter::LocationAttribute
	{
		const ShaderAst::AttributeValue<UInt32>& locationIndex;

		inline bool HasValue() const { return locationIndex.HasValue(); }
	};

	struct LangWriter::SetAttribute
	{
		const ShaderAst::AttributeValue<UInt32>& setIndex;

		inline bool HasValue() const { return setIndex.HasValue(); }
	};

	struct LangWriter::State
	{
		const States* states = nullptr;
		std::stringstream stream;
		std::unordered_map<std::size_t, std::string> constantNames;
		std::unordered_map<std::size_t, ShaderAst::StructDescription*> structs;
		std::unordered_map<std::size_t, std::string> variableNames;
		bool isInEntryPoint = false;
		unsigned int indentLevel = 0;
	};

	std::string LangWriter::Generate(ShaderAst::Statement& shader, const States& /*states*/)
	{
		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		ShaderAst::SanitizeVisitor::Options options;
		options.removeOptionDeclaration = false;

		ShaderAst::StatementPtr sanitizedAst = ShaderAst::Sanitize(shader, options);

		AppendHeader();

		sanitizedAst->Visit(*this);

		return state.stream.str();
	}

	void LangWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	void LangWriter::Append(const ShaderAst::ExpressionType& type)
	{
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, type);
	}

	void LangWriter::Append(const ShaderAst::IdentifierType& /*identifierType*/)
	{
		throw std::runtime_error("unexpected identifier type");
	}

	void LangWriter::Append(const ShaderAst::MatrixType& matrixType)
	{
		if (matrixType.columnCount == matrixType.rowCount)
		{
			Append("mat");
			Append(matrixType.columnCount);
		}
		else
		{
			Append("mat");
			Append(matrixType.columnCount);
			Append("x");
			Append(matrixType.rowCount);
		}

		Append("<", matrixType.type, ">");
	}

	void LangWriter::Append(ShaderAst::PrimitiveType type)
	{
		switch (type)
		{
			case ShaderAst::PrimitiveType::Boolean: return Append("bool");
			case ShaderAst::PrimitiveType::Float32: return Append("f32");
			case ShaderAst::PrimitiveType::Int32:   return Append("i32");
			case ShaderAst::PrimitiveType::UInt32:  return Append("ui32");
		}
	}

	void LangWriter::Append(const ShaderAst::SamplerType& samplerType)
	{
		Append("sampler");

		switch (samplerType.dim)
		{
			case ImageType::E1D:       Append("1D");      break;
			case ImageType::E1D_Array: Append("1DArray"); break;
			case ImageType::E2D:       Append("2D");      break;
			case ImageType::E2D_Array: Append("2DArray"); break;
			case ImageType::E3D:       Append("3D");      break;
			case ImageType::Cubemap:  Append("Cube");    break;
		}

		Append("<", samplerType.sampledType, ">");
	}

	void LangWriter::Append(const ShaderAst::StructType& structType)
	{
		ShaderAst::StructDescription* structDesc = Retrieve(m_currentState->structs, structType.structIndex);
		Append(structDesc->name);
	}

	void LangWriter::Append(const ShaderAst::UniformType& uniformType)
	{
		Append("uniform<");
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, uniformType.containedType);
		Append(">");
	}

	void LangWriter::Append(const ShaderAst::VectorType& vecType)
	{
		Append("vec", vecType.componentCount, "<", vecType.type, ">");
	}

	void LangWriter::Append(ShaderAst::NoType)
	{
		return Append("()");
	}

	template<typename T>
	void LangWriter::Append(const T& param)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << param;
	}

	template<typename T1, typename T2, typename... Args>
	void LangWriter::Append(const T1& firstParam, const T2& secondParam, Args&&... params)
	{
		Append(firstParam);
		Append(secondParam, std::forward<Args>(params)...);
	}

	template<typename ...Args>
	void LangWriter::AppendAttributes(bool appendLine, Args&&... params)
	{
		bool hasAnyAttribute = (params.HasValue() || ...);
		if (!hasAnyAttribute)
			return;

		bool first = true;

		Append("[");
		AppendAttributesInternal(first, std::forward<Args>(params)...);
		Append("]");

		if (appendLine)
			AppendLine();
		else
			Append(" ");
	}

	template<typename T>
	void LangWriter::AppendAttributesInternal(bool& first, const T& param)
	{
		if (!param.HasValue())
			return;

		if (!first)
			Append(", ");

		first = false;

		AppendAttribute(param);
	}

	template<typename T1, typename T2, typename... Rest>
	void LangWriter::AppendAttributesInternal(bool& first, const T1& firstParam, const T2& secondParam, Rest&&... params)
	{
		AppendAttributesInternal(first, firstParam);
		AppendAttributesInternal(first, secondParam, std::forward<Rest>(params)...);
	}

	void LangWriter::AppendAttribute(BindingAttribute binding)
	{
		if (!binding.HasValue())
			return;

		if (binding.bindingIndex.IsResultingValue())
			Append("binding(", binding.bindingIndex.GetResultingValue(), ")");
		else
			binding.bindingIndex.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(BuiltinAttribute builtin)
	{
		if (!builtin.HasValue())
			return;

		if (builtin.builtin.IsResultingValue())
		{
			switch (builtin.builtin.GetResultingValue())
			{
				case ShaderAst::BuiltinEntry::FragCoord:
					Append("builtin(fragcoord)");
					break;

				case ShaderAst::BuiltinEntry::FragDepth:
					Append("builtin(fragdepth)");
					break;

				case ShaderAst::BuiltinEntry::VertexPosition:
					Append("builtin(position)");
					break;
			}
		}
		else
			builtin.builtin.GetExpression()->Visit(*this);
	}
	
	void LangWriter::AppendAttribute(DepthWriteAttribute depthWrite)
	{
		if (!depthWrite.HasValue())
			return;

		if (depthWrite.writeMode.IsResultingValue())
		{
			switch (depthWrite.writeMode.GetResultingValue())
			{
				case ShaderAst::DepthWriteMode::Greater:
					Append("depth_write(greater)");
					break;

				case ShaderAst::DepthWriteMode::Less:
					Append("depth_write(less)");
					break;

				case ShaderAst::DepthWriteMode::Replace:
					Append("depth_write(replace)");
					break;

				case ShaderAst::DepthWriteMode::Unchanged:
					Append("depth_write(unchanged)");
					break;
			}
		}
		else
			depthWrite.writeMode.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(EarlyFragmentTestsAttribute earlyFragmentTests)
	{
		if (!earlyFragmentTests.HasValue())
			return;

		if (earlyFragmentTests.earlyFragmentTests.IsResultingValue())
		{
			if (earlyFragmentTests.earlyFragmentTests.GetResultingValue())
				Append("early_fragment_tests(true)");
			else
				Append("early_fragment_tests(false)");
		}
		else
			earlyFragmentTests.earlyFragmentTests.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(EntryAttribute entry)
	{
		if (!entry.HasValue())
			return;

		if (entry.stageType.IsResultingValue())
		{
			switch (entry.stageType.GetResultingValue())
			{
				case ShaderStageType::Fragment:
					Append("entry(frag)");
					break;

				case ShaderStageType::Vertex:
					Append("entry(vert)");
					break;
			}
		}
		else
			entry.stageType.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(LayoutAttribute entry)
	{
		if (!entry.HasValue())
			return;

		if (entry.layout.IsResultingValue())
		{
			switch (entry.layout.GetResultingValue())
			{
				case StructLayout::Std140:
					Append("layout(std140)");
					break;
			}
		}
		else
			entry.layout.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(LocationAttribute location)
	{
		if (!location.HasValue())
			return;

		if (location.locationIndex.IsResultingValue())
			Append("location(", location.locationIndex.GetResultingValue(), ")");
		else
			location.locationIndex.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(SetAttribute set)
	{
		if (!set.HasValue())
			return;

		if (set.setIndex.IsResultingValue())
			Append("set(", set.setIndex.GetResultingValue(), ")");
		else
			set.setIndex.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendCommentSection(const std::string& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		std::string stars((section.size() < 33) ? (36 - section.size()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void LangWriter::AppendField(std::size_t structIndex, const ShaderAst::ExpressionPtr* memberIndices, std::size_t remainingMembers)
	{
		ShaderAst::StructDescription* structDesc = Retrieve(m_currentState->structs, structIndex);

		assert((*memberIndices)->GetType() == ShaderAst::NodeType::ConstantValueExpression);
		auto& constantValue = static_cast<ShaderAst::ConstantValueExpression&>(**memberIndices);
		Int32 index = std::get<Int32>(constantValue.value);

		const auto& member = structDesc->members[index];

		Append(".");
		Append(member.name);

		if (remainingMembers > 1)
		{
			assert(IsStructType(member.type));
			AppendField(std::get<ShaderAst::StructType>(member.type).structIndex, memberIndices + 1, remainingMembers - 1);
		}
	}

	void LangWriter::AppendLine(const std::string& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << std::string(m_currentState->indentLevel, '\t');
	}

	template<typename... Args>
	void LangWriter::AppendLine(Args&&... params)
	{
		(Append(std::forward<Args>(params)), ...);
		AppendLine();
	}

	void LangWriter::AppendStatementList(std::vector<ShaderAst::StatementPtr>& statements)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : statements)
		{
			if (!first && statement->GetType() != ShaderAst::NodeType::NoOpStatement)
				AppendLine();

			statement->Visit(*this);

			first = false;
		}
	}

	void LangWriter::EnterScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel++;
		AppendLine("{");
	}

	void LangWriter::LeaveScope(bool skipLine)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel--;
		AppendLine();

		if (skipLine)
			AppendLine("}");
		else
			Append("}");
	}

	void LangWriter::RegisterConstant(std::size_t constantIndex, std::string constantName)
	{
		assert(m_currentState->constantNames.find(constantIndex) == m_currentState->constantNames.end());
		m_currentState->constantNames.emplace(constantIndex, std::move(constantName));
	}

	void LangWriter::RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* desc)
	{
		assert(m_currentState->structs.find(structIndex) == m_currentState->structs.end());
		m_currentState->structs.emplace(structIndex, desc);
	}

	void LangWriter::RegisterVariable(std::size_t varIndex, std::string varName)
	{
		assert(m_currentState->variableNames.find(varIndex) == m_currentState->variableNames.end());
		m_currentState->variableNames.emplace(varIndex, std::move(varName));
	}

	void LangWriter::Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired)
	{
		bool enclose = encloseIfRequired && (GetExpressionCategory(*expr) != ShaderAst::ExpressionCategory::LValue);

		if (enclose)
			Append("(");

		expr->Visit(*this);

		if (enclose)
			Append(")");
	}

	void LangWriter::Visit(ShaderAst::AccessIndexExpression& node)
	{
		Visit(node.expr, true);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expr);

		// For structs, convert indices to field names
		if (IsStructType(exprType))
			AppendField(std::get<ShaderAst::StructType>(exprType).structIndex, node.indices.data(), node.indices.size());
		else
		{
			// Array access
			for (ShaderAst::ExpressionPtr& expr : node.indices)
			{
				Append("[");
				Visit(expr);
				Append("]");
			}
		}
	}

	void LangWriter::Visit(ShaderAst::AssignExpression& node)
	{
		node.left->Visit(*this);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple: Append(" = "); break;
			case ShaderAst::AssignType::CompoundAdd: Append(" += "); break;
			case ShaderAst::AssignType::CompoundDivide: Append(" /= "); break;
			case ShaderAst::AssignType::CompoundMultiply: Append(" *= "); break;
			case ShaderAst::AssignType::CompoundLogicalAnd: Append(" &&= "); break;
			case ShaderAst::AssignType::CompoundLogicalOr: Append(" ||= "); break;
			case ShaderAst::AssignType::CompoundSubtract: Append(" -= "); break;
		}

		node.right->Visit(*this);
	}

	void LangWriter::Visit(ShaderAst::BranchStatement& node)
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

	void LangWriter::Visit(ShaderAst::BinaryExpression& node)
	{
		Visit(node.left, true);

		switch (node.op)
		{
			case ShaderAst::BinaryType::Add:        Append(" + "); break;
			case ShaderAst::BinaryType::Subtract:   Append(" - "); break;
			case ShaderAst::BinaryType::Multiply:   Append(" * "); break;
			case ShaderAst::BinaryType::Divide:     Append(" / "); break;

			case ShaderAst::BinaryType::CompEq:     Append(" == "); break;
			case ShaderAst::BinaryType::CompGe:     Append(" >= "); break;
			case ShaderAst::BinaryType::CompGt:     Append(" > ");  break;
			case ShaderAst::BinaryType::CompLe:     Append(" <= "); break;
			case ShaderAst::BinaryType::CompLt:     Append(" < ");  break;
			case ShaderAst::BinaryType::CompNe:     Append(" != "); break;

			case ShaderAst::BinaryType::LogicalAnd: Append(" && "); break;
			case ShaderAst::BinaryType::LogicalOr:  Append(" || "); break;
		}

		Visit(node.right, true);
	}

	void LangWriter::Visit(ShaderAst::CastExpression& node)
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

	void LangWriter::Visit(ShaderAst::ConditionalExpression& node)
	{
		throw std::runtime_error("fixme");
	}

	void LangWriter::Visit(ShaderAst::ConditionalStatement& node)
	{
		Append("[cond(");
		node.condition->Visit(*this);
		AppendLine(")]");
		node.statement->Visit(*this);
	}

	void LangWriter::Visit(ShaderAst::ConstantValueExpression& node)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, ShaderAst::NoValue>)
				throw std::runtime_error("invalid type (value expected)");
			else if constexpr (std::is_same_v<T, bool>)
				Append((arg) ? "true" : "false");
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, Int32> || std::is_same_v<T, UInt32>)
				Append(std::to_string(arg));
			else if constexpr (std::is_same_v<T, Vector2f>)
				Append("vec2<f32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector2i32>)
				Append("vec2<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector3f>)
				Append("vec3<f32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector3i32>)
				Append("vec3<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector4f>)
				Append("vec4<f32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else if constexpr (std::is_same_v<T, Vector4i32>)
				Append("vec4<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}

	void LangWriter::Visit(ShaderAst::ConstantExpression& node)
	{
		Append(Retrieve(m_currentState->constantNames, node.constantId));
	}

	void LangWriter::Visit(ShaderAst::DeclareExternalStatement& node)
	{
		assert(node.varIndex);
		std::size_t varIndex = *node.varIndex;

		AppendLine("external");
		EnterScope();

		bool first = true;
		for (const auto& externalVar : node.externalVars)
		{
			if (!first)
				AppendLine(",");

			first = false;

			AppendAttributes(false, SetAttribute{ externalVar.bindingSet }, BindingAttribute{ externalVar.bindingIndex });
			Append(externalVar.name, ": ", externalVar.type);

			RegisterVariable(varIndex++, externalVar.name);
		}

		LeaveScope();
	}

	void LangWriter::Visit(ShaderAst::DeclareFunctionStatement& node)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		std::optional<std::size_t> varIndexOpt = node.varIndex;

		AppendAttributes(true, EntryAttribute{ node.entryStage }, EarlyFragmentTestsAttribute{ node.earlyFragmentTests }, DepthWriteAttribute{ node.depthWrite });
		Append("fn ", node.name, "(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			Append(node.parameters[i].name);
			Append(": ");
			Append(node.parameters[i].type);

			assert(varIndexOpt);
			std::size_t& varIndex = *varIndexOpt;
			RegisterVariable(varIndex++, node.parameters[i].name);
		}
		Append(")");
		if (!IsNoType(node.returnType))
			Append(" -> ", node.returnType);

		AppendLine();
		EnterScope();
		{
			AppendStatementList(node.statements);
		}
		LeaveScope();
	}

	void LangWriter::Visit(ShaderAst::DeclareOptionStatement& node)
	{
		assert(node.optIndex);
		RegisterConstant(*node.optIndex, node.optName);

		Append("option ", node.optName, ": ", node.optType);
		if (node.defaultValue)
		{
			Append(" = ");
			node.defaultValue->Visit(*this);
		}

		Append(";");
	}

	void LangWriter::Visit(ShaderAst::DeclareStructStatement& node)
	{
		assert(node.structIndex);
		RegisterStruct(*node.structIndex, &node.description);

		AppendAttributes(true, LayoutAttribute{ node.description.layout });
		Append("struct ");
		AppendLine(node.description.name);
		EnterScope();
		{
			bool first = true;
			for (const auto& member : node.description.members)
			{
				if (!first)
					AppendLine(",");

				first = false;

				AppendAttributes(false, LocationAttribute{ member.locationIndex }, BuiltinAttribute{ member.builtin });
				Append(member.name, ": ", member.type);
			}
		}
		LeaveScope();
	}

	void LangWriter::Visit(ShaderAst::DeclareVariableStatement& node)
	{
		assert(node.varIndex);
		RegisterVariable(*node.varIndex, node.varName);

		Append("let ", node.varName, ": ", node.varType);
		if (node.initialExpression)
		{
			Append(" = ");
			node.initialExpression->Visit(*this);
		}

		Append(";");
	}

	void LangWriter::Visit(ShaderAst::DiscardStatement& /*node*/)
	{
		Append("discard;");
	}

	void LangWriter::Visit(ShaderAst::ExpressionStatement& node)
	{
		node.expression->Visit(*this);
		Append(";");
	}

	void LangWriter::Visit(ShaderAst::IntrinsicExpression& node)
	{
		bool method = false;
		switch (node.intrinsic)
		{
			case ShaderAst::IntrinsicType::CrossProduct:
				Append("cross");
				break;

			case ShaderAst::IntrinsicType::DotProduct:
				Append("dot");
				break;

			case ShaderAst::IntrinsicType::Exp:
				Append("exp");
				break;

			case ShaderAst::IntrinsicType::Length:
				Append("length");
				break;

			case ShaderAst::IntrinsicType::Max:
				Append("max");
				break;

			case ShaderAst::IntrinsicType::Min:
				Append("min");
				break;

			case ShaderAst::IntrinsicType::Pow:
				Append("pow");
				break;

			case ShaderAst::IntrinsicType::SampleTexture:
				assert(!node.parameters.empty());
				Visit(node.parameters.front(), true);
				Append(".Sample");
				method = true;
				break;
		}

		Append("(");
		bool first = true;
		for (std::size_t i = (method) ? 1 : 0; i < node.parameters.size(); ++i)
		{
			if (!first)
				Append(", ");

			first = false;

			node.parameters[i]->Visit(*this);
		}
		Append(")");
	}

	void LangWriter::Visit(ShaderAst::MultiStatement& node)
	{
		AppendStatementList(node.statements);
	}

	void LangWriter::Visit(ShaderAst::NoOpStatement& /*node*/)
	{
		/* nothing to do */
	}

	void LangWriter::Visit(ShaderAst::ReturnStatement& node)
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

	void LangWriter::Visit(ShaderAst::SwizzleExpression& node)
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

	void LangWriter::Visit(ShaderAst::VariableExpression& node)
	{
		const std::string& varName = Retrieve(m_currentState->variableNames, node.variableId);
		Append(varName);
	}

	void LangWriter::Visit(ShaderAst::UnaryExpression& node)
	{
		switch (node.op)
		{
			case ShaderAst::UnaryType::LogicalNot:
				Append("!");
				break;

			case ShaderAst::UnaryType::Minus:
				Append("-");
				break;

			case ShaderAst::UnaryType::Plus:
				Append("+");
				break;
		}

		node.expression->Visit(*this);
	}

	void LangWriter::AppendHeader()
	{
		// Nothing yet
	}
}
