// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
		const ShaderAst::ExpressionValue<UInt32>& bindingIndex;

		bool HasValue() const { return bindingIndex.HasValue(); }
	};

	struct LangWriter::BuiltinAttribute
	{
		const ShaderAst::ExpressionValue<ShaderAst::BuiltinEntry>& builtin;

		bool HasValue() const { return builtin.HasValue(); }
	};

	struct LangWriter::DepthWriteAttribute
	{
		const ShaderAst::ExpressionValue<ShaderAst::DepthWriteMode>& writeMode;

		bool HasValue() const { return writeMode.HasValue(); }
	};

	struct LangWriter::EarlyFragmentTestsAttribute
	{
		const ShaderAst::ExpressionValue<bool>& earlyFragmentTests;

		bool HasValue() const { return earlyFragmentTests.HasValue(); }
	};

	struct LangWriter::EntryAttribute
	{
		const ShaderAst::ExpressionValue<ShaderStageType>& stageType;

		bool HasValue() const { return stageType.HasValue(); }
	};

	struct LangWriter::LangVersionAttribute
	{
		UInt32 version;

		bool HasValue() const { return true; }
	};

	struct LangWriter::LayoutAttribute
	{
		const ShaderAst::ExpressionValue<StructLayout>& layout;

		bool HasValue() const { return layout.HasValue(); }
	};

	struct LangWriter::LocationAttribute
	{
		const ShaderAst::ExpressionValue<UInt32>& locationIndex;

		bool HasValue() const { return locationIndex.HasValue(); }
	};
	
	struct LangWriter::SetAttribute
	{
		const ShaderAst::ExpressionValue<UInt32>& setIndex;

		bool HasValue() const { return setIndex.HasValue(); }
	};

	struct LangWriter::UnrollAttribute
	{
		const ShaderAst::ExpressionValue<ShaderAst::LoopUnroll>& unroll;

		bool HasValue() const { return unroll.HasValue(); }
	};

	struct LangWriter::UuidAttribute
	{
		Uuid uuid;

		bool HasValue() const { return true; }
	};

	struct LangWriter::State
	{
		struct Identifier
		{
			std::size_t moduleIndex;
			std::string name;
		};

		const States* states = nullptr;
		ShaderAst::Module* module;
		std::size_t currentModuleIndex;
		std::stringstream stream;
		std::unordered_map<std::size_t, Identifier> constantNames;
		std::unordered_map<std::size_t, Identifier> structs;
		std::unordered_map<std::size_t, Identifier> variableNames;
		std::vector<std::string> moduleNames;
		bool isInEntryPoint = false;
		unsigned int indentLevel = 0;
	};

	std::string LangWriter::Generate(const ShaderAst::Module& module, const States& /*states*/)
	{
		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		ShaderAst::ModulePtr sanitizedModule = ShaderAst::Sanitize(module);
		state.module = sanitizedModule.get();

		AppendHeader();

		// Register imported modules
		m_currentState->currentModuleIndex = 0;
		for (const auto& importedModule : sanitizedModule->importedModules)
		{
			AppendAttributes(true, LangVersionAttribute{ importedModule.module->metadata->shaderLangVersion });
			AppendAttributes(true, UuidAttribute{ importedModule.module->metadata->moduleId });
			AppendLine("module ", importedModule.identifier);
			EnterScope();
			importedModule.module->rootNode->Visit(*this);
			LeaveScope(true);

			m_currentState->currentModuleIndex++;
			m_currentState->moduleNames.push_back(importedModule.identifier);
		}

		m_currentState->currentModuleIndex = std::numeric_limits<std::size_t>::max();
		sanitizedModule->rootNode->Visit(*this);

		return state.stream.str();
	}

	void LangWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	void LangWriter::Append(const ShaderAst::ArrayType& type)
	{
		Append("array[", type.containedType->type, ", ", type.length, "]");
	}

	void LangWriter::Append(const ShaderAst::ExpressionType& type)
	{
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, type);
	}

	void LangWriter::Append(const ShaderAst::ExpressionValue<ShaderAst::ExpressionType>& type)
	{
		Append(type.GetResultingValue());
	}

	void LangWriter::Append(const ShaderAst::FunctionType& /*functionType*/)
	{
		throw std::runtime_error("unexpected function type");
	}

	void LangWriter::Append(const ShaderAst::IdentifierType& /*identifierType*/)
	{
		throw std::runtime_error("unexpected identifier type");
	}

	void LangWriter::Append(const ShaderAst::IntrinsicFunctionType& /*functionType*/)
	{
		throw std::runtime_error("unexpected intrinsic function type");
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

		Append("[", matrixType.type, "]");
	}

	void LangWriter::Append(const ShaderAst::MethodType& /*functionType*/)
	{
		throw std::runtime_error("unexpected method type");
	}

	void LangWriter::Append(ShaderAst::PrimitiveType type)
	{
		switch (type)
		{
			case ShaderAst::PrimitiveType::Boolean: return Append("bool");
			case ShaderAst::PrimitiveType::Float32: return Append("f32");
			case ShaderAst::PrimitiveType::Int32:   return Append("i32");
			case ShaderAst::PrimitiveType::UInt32:  return Append("u32");
			case ShaderAst::PrimitiveType::String:  return Append("string");
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
			case ImageType::Cubemap:   Append("Cube");    break;
		}

		Append("[", samplerType.sampledType, "]");
	}

	void LangWriter::Append(const ShaderAst::StructType& structType)
	{
		AppendIdentifier(m_currentState->structs, structType.structIndex);
	}

	void LangWriter::Append(const ShaderAst::Type& /*type*/)
	{
		throw std::runtime_error("unexpected type?");
	}

	void LangWriter::Append(const ShaderAst::UniformType& uniformType)
	{
		Append("uniform[", uniformType.containedType, "]");
	}

	void LangWriter::Append(const ShaderAst::VectorType& vecType)
	{
		Append("vec", vecType.componentCount, "[", vecType.type, "]");
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

	void LangWriter::AppendAttribute(BindingAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("binding(");

		if (attribute.bindingIndex.IsResultingValue())
			Append(attribute.bindingIndex.GetResultingValue());
		else
			attribute.bindingIndex.GetExpression()->Visit(*this);

		Append(")");
	}

	void LangWriter::AppendAttribute(BuiltinAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("builtin(");

		if (attribute.builtin.IsResultingValue())
		{
			switch (attribute.builtin.GetResultingValue())
			{
				case ShaderAst::BuiltinEntry::FragCoord:
					Append("fragcoord");
					break;

				case ShaderAst::BuiltinEntry::FragDepth:
					Append("fragdepth");
					break;

				case ShaderAst::BuiltinEntry::VertexPosition:
					Append("position");
					break;
			}
		}
		else
			attribute.builtin.GetExpression()->Visit(*this);

		Append(")");
	}
	
	void LangWriter::AppendAttribute(DepthWriteAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("depth_write(");

		if (attribute.writeMode.IsResultingValue())
		{
			switch (attribute.writeMode.GetResultingValue())
			{
				case ShaderAst::DepthWriteMode::Greater:
					Append("greater");
					break;

				case ShaderAst::DepthWriteMode::Less:
					Append("less");
					break;

				case ShaderAst::DepthWriteMode::Replace:
					Append("replace");
					break;

				case ShaderAst::DepthWriteMode::Unchanged:
					Append("unchanged");
					break;
			}
		}
		else
			attribute.writeMode.GetExpression()->Visit(*this);

		Append(")");
	}

	void LangWriter::AppendAttribute(EarlyFragmentTestsAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("early_fragment_tests(");

		if (attribute.earlyFragmentTests.IsResultingValue())
		{
			if (attribute.earlyFragmentTests.GetResultingValue())
				Append("true");
			else
				Append("false");
		}
		else
			attribute.earlyFragmentTests.GetExpression()->Visit(*this);

		Append(")");
	}

	void LangWriter::AppendAttribute(EntryAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("entry(");

		if (attribute.stageType.IsResultingValue())
		{
			switch (attribute.stageType.GetResultingValue())
			{
				case ShaderStageType::Fragment:
					Append("frag");
					break;

				case ShaderStageType::Vertex:
					Append("vert");
					break;
			}
		}
		else
			attribute.stageType.GetExpression()->Visit(*this);

		Append(")");
	}

	void LangWriter::AppendAttribute(LangVersionAttribute attribute)
	{
		UInt32 shaderLangVersion = attribute.version;
		UInt32 majorVersion = shaderLangVersion / 100;
		shaderLangVersion -= majorVersion * 100;

		UInt32 minorVersion = shaderLangVersion / 10;
		shaderLangVersion -= minorVersion * 100;

		UInt32 patchVersion = shaderLangVersion;

		// nzsl_version
		Append("nzsl_version(\"", majorVersion, ".", minorVersion);
		if (patchVersion != 0)
			Append(".", patchVersion);

		Append("\")");
	}

	void LangWriter::AppendAttribute(LayoutAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("layout(");
		if (attribute.layout.IsResultingValue())
		{
			switch (attribute.layout.GetResultingValue())
			{
				case StructLayout::Packed:
					Append("packed");
					break;

				case StructLayout::Std140:
					Append("std140");
					break;
			}
		}
		else
			attribute.layout.GetExpression()->Visit(*this);
		Append(")");
	}

	void LangWriter::AppendAttribute(LocationAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("location(");

		if (attribute.locationIndex.IsResultingValue())
			Append(attribute.locationIndex.GetResultingValue());
		else
			attribute.locationIndex.GetExpression()->Visit(*this);

		Append(")");
	}
	
	void LangWriter::AppendAttribute(SetAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("set(");

		if (attribute.setIndex.IsResultingValue())
			Append(attribute.setIndex.GetResultingValue());
		else
			attribute.setIndex.GetExpression()->Visit(*this);

		Append(")");
	}

	void LangWriter::AppendAttribute(UnrollAttribute attribute)
	{
		if (!attribute.HasValue())
			return;

		Append("unroll(");

		if (attribute.unroll.IsResultingValue())
		{
			switch (attribute.unroll.GetResultingValue())
			{
				case ShaderAst::LoopUnroll::Always:
					Append("always");
					break;

				case ShaderAst::LoopUnroll::Hint:
					Append("hint");
					break;

				case ShaderAst::LoopUnroll::Never:
					Append("never");
					break;

				default:
					break;
			}
		}
		else
			attribute.unroll.GetExpression()->Visit(*this);
	}

	void LangWriter::AppendAttribute(UuidAttribute attribute)
	{
		Append("uuid(\"", attribute.uuid.ToString(), "\")");
	}

	void LangWriter::AppendComment(const std::string& section)
	{
		std::size_t lineFeed = section.find('\n');
		if (lineFeed != section.npos)
		{
			std::size_t previousCut = 0;

			AppendLine("/*");
			do
			{
				AppendLine(section.substr(previousCut, lineFeed - previousCut));
				previousCut = lineFeed + 1;
			}
			while ((lineFeed = section.find('\n', previousCut)) != section.npos);
			AppendLine(section.substr(previousCut));
			AppendLine("*/");
		}
		else
			AppendLine("// ", section);
	}

	void LangWriter::AppendCommentSection(const std::string& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		std::string stars((section.size() < 33) ? (36 - section.size()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void LangWriter::AppendLine(const std::string& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << std::string(m_currentState->indentLevel, '\t');
	}

	template<typename T>
	void LangWriter::AppendIdentifier(const T& map, std::size_t id)
	{
		const auto& structIdentifier = Retrieve(map, id);
		if (structIdentifier.moduleIndex != m_currentState->currentModuleIndex)
			Append(m_currentState->moduleNames[structIdentifier.moduleIndex], '.');

		Append(structIdentifier.name);
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
			if (statement->GetType() == ShaderAst::NodeType::NoOpStatement)
				continue;

			if (!first)
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
		State::Identifier identifier;
		identifier.moduleIndex = m_currentState->currentModuleIndex;
		identifier.name = std::move(constantName);

		assert(m_currentState->constantNames.find(constantIndex) == m_currentState->constantNames.end());
		m_currentState->constantNames.emplace(constantIndex, std::move(identifier));
	}

	void LangWriter::RegisterStruct(std::size_t structIndex, std::string structName)
	{
		State::Identifier identifier;
		identifier.moduleIndex = m_currentState->currentModuleIndex;
		identifier.name = std::move(structName);

		assert(m_currentState->structs.find(structIndex) == m_currentState->structs.end());
		m_currentState->structs.emplace(structIndex, std::move(identifier));
	}

	void LangWriter::RegisterVariable(std::size_t varIndex, std::string varName)
	{
		State::Identifier identifier;
		identifier.moduleIndex = m_currentState->currentModuleIndex;
		identifier.name = std::move(varName);

		assert(m_currentState->variableNames.find(varIndex) == m_currentState->variableNames.end());
		m_currentState->variableNames.emplace(varIndex, std::move(identifier));
	}

	void LangWriter::ScopeVisit(ShaderAst::Statement& node)
	{
		if (node.GetType() != ShaderAst::NodeType::ScopedStatement)
		{
			EnterScope();
			node.Visit(*this);
			LeaveScope(true);
		}
		else
			node.Visit(*this);
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

	void LangWriter::Visit(ShaderAst::AccessIdentifierExpression& node)
	{
		Visit(node.expr, true);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expr);
		assert(IsStructType(exprType));

		for (const std::string& identifier : node.identifiers)
			Append(".", identifier);
	}

	void LangWriter::Visit(ShaderAst::AccessIndexExpression& node)
	{
		Visit(node.expr, true);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expr);
		assert(!IsStructType(exprType));

		// Array access
		Append("[");

		bool first = true;
		for (ShaderAst::ExpressionPtr& expr : node.indices)
		{
			if (!first)
				Append(", ");

			expr->Visit(*this);
			first = false;
		}

		Append("]");
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
			if (first)
			{
				if (node.isConst)
					Append("const ");
			}
			else
				Append("else ");

			Append("if (");
			statement.condition->Visit(*this);
			AppendLine(")");

			ScopeVisit(*statement.statement);

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			ScopeVisit(*node.elseStatement);
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

	void LangWriter::Visit(ShaderAst::DeclareAliasStatement& node)
	{
		//throw std::runtime_error("TODO"); //< missing registering

		assert(node.aliasIndex);

		Append("alias ", node.name, " = ");
		assert(node.expression);
		node.expression->Visit(*this);
		AppendLine(";");
	}

	void LangWriter::Visit(ShaderAst::DeclareConstStatement& node)
	{
		assert(node.constIndex);
		RegisterConstant(*node.constIndex, node.name);

		Append("const ", node.name, ": ", node.type);
		if (node.expression)
		{
			Append(" = ");
			node.expression->Visit(*this);
		}

		AppendLine(";");
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
			else if constexpr (std::is_same_v<T, std::string>)
				Append('"', arg, '"'); //< TODO: Escape string
			else if constexpr (std::is_same_v<T, Vector2f>)
				Append("vec2[f32](" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector2i32>)
				Append("vec2<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector3f>)
				Append("vec3[f32](" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector3i32>)
				Append("vec3<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector4f>)
				Append("vec4[f32](" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else if constexpr (std::is_same_v<T, Vector4i32>)
				Append("vec4<i32>(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}

	void LangWriter::Visit(ShaderAst::ConstantExpression& node)
	{
		AppendIdentifier(m_currentState->constantNames, node.constantId);
	}

	void LangWriter::Visit(ShaderAst::DeclareExternalStatement& node)
	{
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

			assert(externalVar.varIndex);
			RegisterVariable(*externalVar.varIndex, externalVar.name);
		}

		LeaveScope();
	}

	void LangWriter::Visit(ShaderAst::DeclareFunctionStatement& node)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		AppendAttributes(true, EntryAttribute{ node.entryStage }, EarlyFragmentTestsAttribute{ node.earlyFragmentTests }, DepthWriteAttribute{ node.depthWrite });
		Append("fn ", node.name, "(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			const auto& parameter = node.parameters[i];

			if (i != 0)
				Append(", ");

			Append(parameter.name);
			Append(": ");
			Append(parameter.type);

			assert(parameter.varIndex);
			RegisterVariable(*parameter.varIndex, parameter.name);
		}
		Append(")");
		if (node.returnType.HasValue())
		{
			const ShaderAst::ExpressionType& returnType = node.returnType.GetResultingValue();
			if (!IsNoType(returnType))
				Append(" -> ", returnType);
		}

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
		RegisterStruct(*node.structIndex, node.description.name);

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

	void LangWriter::Visit(ShaderAst::ForStatement& node)
	{
		assert(node.varIndex);
		RegisterVariable(*node.varIndex, node.varName);

		AppendAttributes(true, UnrollAttribute{ node.unroll });
		Append("for ", node.varName, " in ");
		node.fromExpr->Visit(*this);
		Append(" -> ");
		node.toExpr->Visit(*this);

		if (node.stepExpr)
		{
			Append(" : ");
			node.stepExpr->Visit(*this);
		}

		AppendLine();

		ScopeVisit(*node.statement);
	}

	void LangWriter::Visit(ShaderAst::ForEachStatement& node)
	{
		assert(node.varIndex);
		RegisterVariable(*node.varIndex, node.varName);

		AppendAttributes(true, UnrollAttribute{ node.unroll });
		Append("for ", node.varName, " in ");
		node.expression->Visit(*this);
		AppendLine();

		ScopeVisit(*node.statement);
	}

	void LangWriter::Visit(ShaderAst::ImportStatement& node)
	{
		bool first = true;
		for (const std::string& path : node.modulePath)
		{
			if (!first)
				Append("/");

			Append(path);

			first = false;
		}
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

			case ShaderAst::IntrinsicType::Normalize:
				Append("normalize");
				break;

			case ShaderAst::IntrinsicType::Pow:
				Append("pow");
				break;

			case ShaderAst::IntrinsicType::Reflect:
				Append("reflect");
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

	void LangWriter::Visit(ShaderAst::StructTypeExpression& node)
	{
		AppendIdentifier(m_currentState->structs, node.structTypeId);
	}

	void LangWriter::Visit(ShaderAst::MultiStatement& node)
	{
		if (!node.sectionName.empty())
			AppendComment(node.sectionName);

		AppendStatementList(node.statements);

		if (!node.sectionName.empty())
			AppendComment("End: " + node.sectionName);
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

	void LangWriter::Visit(ShaderAst::ScopedStatement& node)
	{
		EnterScope();
		node.statement->Visit(*this);
		LeaveScope();
	}

	void LangWriter::Visit(ShaderAst::SwizzleExpression& node)
	{
		Visit(node.expression, true);
		Append(".");

		const char* componentStr = "xyzw";
		for (std::size_t i = 0; i < node.componentCount; ++i)
			Append(componentStr[node.components[i]]);
	}

	void LangWriter::Visit(ShaderAst::VariableExpression& node)
	{
		AppendIdentifier(m_currentState->variableNames, node.variableId);
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

	void LangWriter::Visit(ShaderAst::WhileStatement& node)
	{
		Append("while (");
		node.condition->Visit(*this);
		AppendLine(")");

		ScopeVisit(*node.body);
	}

	void LangWriter::AppendHeader()
	{
		AppendAttributes(true, LangVersionAttribute{ m_currentState->module->metadata->shaderLangVersion });
		AppendLine("module;");
		AppendLine();
	}
}
