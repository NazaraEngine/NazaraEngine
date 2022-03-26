// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangErrors.hpp>
#include <Nazara/Shader/Ast/AstConstantPropagationVisitor.hpp>
#include <Nazara/Shader/Ast/AstExportVisitor.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Ast/AstReflect.hpp>
#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <Nazara/Shader/Ast/DependencyCheckerVisitor.hpp>
#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Shader/Ast/IndexRemapperVisitor.hpp>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	struct SanitizeVisitor::AstError
	{
		std::string errMsg;
	};

	struct SanitizeVisitor::CurrentFunctionData
	{
		std::optional<ShaderStageType> stageType;
		Bitset<> calledFunctions;
		DeclareFunctionStatement* statement;
		FunctionFlags flags;
	};

	template<typename T>
	struct SanitizeVisitor::IdentifierList
	{
		Bitset<UInt64> availableIndices;
		Bitset<UInt64> preregisteredIndices;
		std::unordered_map<std::size_t, T> values;

		void PreregisterIndex(std::size_t index)
		{
			if (index < availableIndices.GetSize())
			{
				if (!availableIndices.Test(index))
					throw AstError{ "cannot preregister used index  " + std::to_string(index) + " as its already used" };
			}
			else if (index >= availableIndices.GetSize())
				availableIndices.Resize(index + 1, true);

			availableIndices.Set(index, false);
			preregisteredIndices.UnboundedSet(index);
		}

		template<typename U>
		std::size_t Register(U&& data, std::optional<std::size_t> index = {})
		{
			std::size_t dataIndex;
			if (index.has_value())
			{
				dataIndex = *index;

				if (dataIndex >= availableIndices.GetSize())
					availableIndices.Resize(dataIndex + 1, true);
				else if (!availableIndices.Test(dataIndex))
				{
					if (preregisteredIndices.UnboundedTest(dataIndex))
						preregisteredIndices.Reset(dataIndex);
					else
						throw AstError{ "index " + std::to_string(dataIndex) + " is already used" };
				}
			}
			else
				dataIndex = RegisterNewIndex();

			assert(values.find(dataIndex) == values.end());

			availableIndices.Set(dataIndex, false);
			values.emplace(dataIndex, std::forward<U>(data));
			return dataIndex;
		}

		std::size_t RegisterNewIndex(bool preregister = false)
		{
			std::size_t index = availableIndices.FindFirst();
			if (index == availableIndices.npos)
			{
				index = availableIndices.GetSize();
				availableIndices.Resize(index + 1, true);
			}

			availableIndices.Set(index, false);

			if (preregister)
				preregisteredIndices.UnboundedSet(index);

			return index;
		}

		T& Retrieve(std::size_t index)
		{
			auto it = values.find(index);
			if (it == values.end())
				throw AstError{ "invalid index " + std::to_string(index) };

			return it->second;
		}

		T* TryRetrieve(std::size_t index)
		{
			auto it = values.find(index);
			if (it == values.end())
			{
				if (!preregisteredIndices.UnboundedTest(index))
					throw AstError{ "invalid index " + std::to_string(index) };

				return nullptr;
			}

			return &it->second;
		}
	};

	struct SanitizeVisitor::Scope
	{
		std::size_t previousSize;
	};

	struct SanitizeVisitor::Environment
	{
		Uuid moduleId;
		std::shared_ptr<Environment> parentEnv;
		std::vector<Identifier> identifiersInScope;
		std::vector<Scope> scopes;
	};

	struct SanitizeVisitor::Context
	{
		struct ModuleData
		{
			std::unordered_map<Uuid, DependencyCheckerVisitor::UsageSet> exportedSetByModule;
			std::shared_ptr<Environment> environment;
			std::unique_ptr<DependencyCheckerVisitor> dependenciesVisitor;
		};

		struct PendingFunction
		{
			DeclareFunctionStatement* cloneNode;
			const DeclareFunctionStatement* node;
		};

		static constexpr std::size_t ModuleIdSentinel = std::numeric_limits<std::size_t>::max();

		std::array<DeclareFunctionStatement*, ShaderStageTypeCount> entryFunctions = {};
		std::vector<ModuleData> modules;
		std::vector<PendingFunction> pendingFunctions;
		std::vector<StatementPtr>* currentStatementList = nullptr;
		std::unordered_map<Uuid, std::size_t> moduleByUuid;
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<UInt64> usedBindingIndexes;
		std::shared_ptr<Environment> globalEnv;
		std::shared_ptr<Environment> currentEnv;
		std::shared_ptr<Environment> moduleEnv;
		IdentifierList<ConstantValue> constantValues;
		IdentifierList<FunctionData> functions;
		IdentifierList<IdentifierData> aliases;
		IdentifierList<IntrinsicType> intrinsics;
		IdentifierList<std::size_t> moduleIndices;
		IdentifierList<StructDescription*> structs;
		IdentifierList<std::variant<ExpressionType, PartialType>> types;
		IdentifierList<ExpressionType> variableTypes;
		ModulePtr currentModule;
		Options options;
		CurrentFunctionData* currentFunction = nullptr;
		bool allowUnknownIdentifiers = false;
	};

	ModulePtr SanitizeVisitor::Sanitize(const Module& module, const Options& options, std::string* error)
	{
		ModulePtr clone = std::make_shared<Module>(module.metadata, module.importedModules);

		Context currentContext;
		currentContext.options = options;
		currentContext.currentModule = clone;

		m_context = &currentContext;
		CallOnExit resetContext([&] { m_context = nullptr; });

		PreregisterIndices(module);

		// Register global env
		m_context->globalEnv = std::make_shared<Environment>();
		m_context->currentEnv = m_context->globalEnv;
		RegisterBuiltin();

		m_context->moduleEnv = std::make_shared<Environment>();
		m_context->moduleEnv->moduleId = clone->metadata->moduleId;
		m_context->moduleEnv->parentEnv = m_context->globalEnv;

		for (std::size_t moduleId = 0; moduleId < clone->importedModules.size(); ++moduleId)
		{
			auto importedModuleEnv = std::make_shared<Environment>();
			importedModuleEnv->moduleId = clone->importedModules[moduleId].module->metadata->moduleId;
			importedModuleEnv->parentEnv = m_context->globalEnv;

			m_context->currentEnv = importedModuleEnv;

			auto& importedModule = clone->importedModules[moduleId];
			importedModule.module->rootNode = SanitizeInternal(*importedModule.module->rootNode, error);
			if (!importedModule.module->rootNode)
				return {};

			m_context->moduleByUuid[importedModule.module->metadata->moduleId] = moduleId;
			auto& moduleData = m_context->modules.emplace_back();
			moduleData.environment = std::move(importedModuleEnv);

			m_context->currentEnv = m_context->globalEnv;
			RegisterModule(importedModule.identifier, moduleId);
		}

		m_context->currentEnv = m_context->moduleEnv;

		clone->rootNode = SanitizeInternal(*module.rootNode, error);
		if (!clone->rootNode)
			return {};

		// Remove unused statements of imported modules
		for (std::size_t moduleId = 0; moduleId < clone->importedModules.size(); ++moduleId)
		{
			auto& moduleData = m_context->modules[moduleId];
			auto& importedModule = clone->importedModules[moduleId];

			if (moduleData.dependenciesVisitor)
			{
				moduleData.dependenciesVisitor->Resolve();
				importedModule.module = EliminateUnusedPass(*importedModule.module, moduleData.dependenciesVisitor->GetUsage());
			}
		}

		return clone;
	}
	
	UInt32 SanitizeVisitor::ToSwizzleIndex(char c)
	{
		switch (c)
		{
			case 'r':
			case 'x':
			case 's':
				return 0u;

			case 'g':
			case 'y':
			case 't':
				return 1u;

			case 'b':
			case 'z':
			case 'p':
				return 2u;

			case 'a':
			case 'w':
			case 'q':
				return 3u;

			default:
				throw AstError{ "unexpected character '" + std::string(&c, 1) + "' on swizzle " };
		}
	}

	ExpressionValue<ExpressionType> SanitizeVisitor::CloneType(const ExpressionValue<ExpressionType>& exprType)
	{
		if (!exprType.HasValue())
			return {};

		std::optional<ExpressionType> resolvedType = ResolveTypeExpr(exprType);
		if (!resolvedType.has_value())
			return AstCloner::CloneType(exprType);

		return std::move(resolvedType).value();
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessIdentifierExpression& node)
	{
		if (node.identifiers.empty())
			throw AstError{ "AccessIdentifierExpression must have at least one identifier" };

		MandatoryExpr(node.expr);

		// Handle module access (TODO: Add namespace expression?)
		if (node.expr->GetType() == NodeType::IdentifierExpression && node.identifiers.size() == 1)
		{
			auto& identifierExpr = static_cast<IdentifierExpression&>(*node.expr);
			const IdentifierData* identifierData = FindIdentifier(identifierExpr.identifier);
			if (identifierData && identifierData->category == IdentifierCategory::Module)
			{
				std::size_t moduleIndex = m_context->moduleIndices.Retrieve(identifierData->index);

				const auto& env = *m_context->modules[moduleIndex].environment;
				identifierData = FindIdentifier(env, node.identifiers.front());
				if (identifierData)
					return HandleIdentifier(identifierData);
			}
		}

		ExpressionPtr indexedExpr = CloneExpression(node.expr);
		for (const std::string& identifier : node.identifiers)
		{
			if (identifier.empty())
				throw AstError{ "empty identifier" };

			const ExpressionType* exprType = GetExpressionType(*indexedExpr);
			if (!exprType)
				return AstCloner::Clone(node); //< unresolved type

			const ExpressionType& resolvedType = ResolveAlias(*exprType);
			// TODO: Add proper support for methods
			if (IsSamplerType(resolvedType))
			{
				if (identifier == "Sample")
				{
					// TODO: Add a MethodExpression?
					auto identifierExpr = std::make_unique<AccessIdentifierExpression>();
					identifierExpr->expr = std::move(indexedExpr);
					identifierExpr->identifiers.push_back(identifier);

					MethodType methodType;
					methodType.methodIndex = 0; //< FIXME
					methodType.objectType = std::make_unique<ContainedType>();
					methodType.objectType->type = resolvedType;

					identifierExpr->cachedExpressionType = std::move(methodType);
					indexedExpr = std::move(identifierExpr);
				}
				else
					throw AstError{ "type has no method " + identifier };
			}
			else if (IsStructType(resolvedType))
			{
				std::size_t structIndex = ResolveStruct(resolvedType);
				const StructDescription* s = m_context->structs.Retrieve(structIndex);

				// Retrieve member index (not counting disabled fields)
				Int32 fieldIndex = 0;
				const StructDescription::StructMember* fieldPtr = nullptr;
				for (const auto& field : s->members)
				{
					if (field.cond.HasValue())
					{
						if (!field.cond.IsResultingValue())
						{
							if (m_context->options.allowPartialSanitization)
								return AstCloner::Clone(node); //< unresolved

							throw AstError{ "cond attribute is not constant" };
						}
						else if (!field.cond.GetResultingValue())
							continue;
					}

					if (field.name == identifier)
					{
						fieldPtr = &field;
						break;
					}

					fieldIndex++;
				}

				if (!fieldPtr)
					throw AstError{ "unknown field " + identifier };

				if (m_context->options.useIdentifierAccessesForStructs)
				{
					// Use a AccessIdentifierExpression
					AccessIdentifierExpression* accessIdentifierPtr;
					if (indexedExpr->GetType() != NodeType::AccessIdentifierExpression)
					{
						std::unique_ptr<AccessIdentifierExpression> accessIndex = std::make_unique<AccessIdentifierExpression>();
						accessIndex->expr = std::move(indexedExpr);

						accessIdentifierPtr = accessIndex.get();
						indexedExpr = std::move(accessIndex);
					}
					else
						accessIdentifierPtr = static_cast<AccessIdentifierExpression*>(indexedExpr.get());

					accessIdentifierPtr->identifiers.push_back(fieldPtr->name);
					accessIdentifierPtr->cachedExpressionType = ResolveTypeExpr(fieldPtr->type);
				}
				else
				{
					// Transform to AccessIndexExpression
					std::unique_ptr<AccessIndexExpression> accessIndex = std::make_unique<AccessIndexExpression>();
					accessIndex->expr = std::move(indexedExpr);
					accessIndex->indices.push_back(ShaderBuilder::Constant(fieldIndex));
					accessIndex->cachedExpressionType = ResolveTypeExpr(fieldPtr->type);

					indexedExpr = std::move(accessIndex);
				}
			}
			else if (IsPrimitiveType(resolvedType) || IsVectorType(resolvedType))
			{
				// Swizzle expression
				std::size_t swizzleComponentCount = identifier.size();
				if (swizzleComponentCount > 4)
					throw AstError{ "cannot swizzle more than four elements" };

				if (m_context->options.removeScalarSwizzling && IsPrimitiveType(resolvedType))
				{
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
					{
						if (ToSwizzleIndex(identifier[j]) != 0)
							throw AstError{ "invalid swizzle" };
							//throw ShaderLang::CompilerInvalidSwizzleError{};
					}

					if (swizzleComponentCount == 1)
						continue; //< ignore this swizzle (a.x == a)

					// Use a Cast expression to replace swizzle
					indexedExpr = CacheResult(std::move(indexedExpr)); //< Since we are going to use a value multiple times, cache it if required

					PrimitiveType baseType;
					if (IsVectorType(resolvedType))
						baseType = std::get<VectorType>(resolvedType).type;
					else
						baseType = std::get<PrimitiveType>(resolvedType);

					auto cast = std::make_unique<CastExpression>();
					cast->targetType = ExpressionType{ VectorType{ swizzleComponentCount, baseType } };
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
						cast->expressions[j] = CloneExpression(indexedExpr);

					Validate(*cast);

					indexedExpr = std::move(cast);
				}
				else
				{
					auto swizzle = std::make_unique<SwizzleExpression>();
					swizzle->expression = std::move(indexedExpr);

					swizzle->componentCount = swizzleComponentCount;
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
						swizzle->components[j] = ToSwizzleIndex(identifier[j]);

					Validate(*swizzle);

					indexedExpr = std::move(swizzle);
				}
			}
			else
				throw AstError{ "unexpected type (only struct and vectors can be indexed with identifiers)" }; //< TODO: Add support for arrays
		}

		return indexedExpr;
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessIndexExpression& node)
	{
		MandatoryExpr(node.expr);
		for (auto& index : node.indices)
			MandatoryExpr(index);

		auto clone = StaticUniquePointerCast<AccessIndexExpression>(AstCloner::Clone(node));
		Validate(*clone);

		// TODO: Handle AccessIndex on structs with m_context->options.useIdentifierAccessesForStructs

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(AliasValueExpression& node)
	{
		const IdentifierData* targetIdentifier = ResolveAliasIdentifier(&m_context->aliases.Retrieve(node.aliasId));
		ExpressionPtr targetExpr = HandleIdentifier(targetIdentifier);

		if (m_context->options.removeAliases)
			return targetExpr;

		AliasType aliasType;
		aliasType.aliasIndex = node.aliasId;
		aliasType.targetType = std::make_unique<ContainedType>();
		aliasType.targetType->type = *targetExpr->cachedExpressionType;

		auto clone = StaticUniquePointerCast<AliasValueExpression>(AstCloner::Clone(node));
		clone->cachedExpressionType = std::move(aliasType);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(AssignExpression& node)
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		auto clone = StaticUniquePointerCast<AssignExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(BinaryExpression& node)
	{
		auto clone = StaticUniquePointerCast<BinaryExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(CallFunctionExpression& node)
	{
		ExpressionPtr targetExpr = CloneExpression(MandatoryExpr(node.targetFunction));
		const ExpressionType* targetExprType = GetExpressionType(*targetExpr);
		if (!targetExprType)
			return AstCloner::Clone(node); //< unresolved type

		const ExpressionType& resolvedType = ResolveAlias(*targetExprType);

		if (IsFunctionType(resolvedType))
		{
			if (!m_context->currentFunction)
				throw AstError{ "function calls must happen inside a function" };

			std::size_t targetFuncIndex;
			if (targetExpr->GetType() == NodeType::FunctionExpression)
				targetFuncIndex = static_cast<FunctionExpression&>(*targetExpr).funcId;
			else if (targetExpr->GetType() == NodeType::AliasValueExpression)
			{
				const auto& alias = static_cast<AliasValueExpression&>(*targetExpr);

				const IdentifierData* targetIdentifier = ResolveAliasIdentifier(&m_context->aliases.Retrieve(alias.aliasId));
				if (targetIdentifier->category != IdentifierCategory::Function)
					throw AstError{ "expected function expression" };

				targetFuncIndex = targetIdentifier->index;
			}
			else
				throw AstError{ "expected function expression" };

			auto clone = std::make_unique<CallFunctionExpression>();
			clone->targetFunction = std::move(targetExpr);

			clone->parameters.reserve(node.parameters.size());
			for (const auto& parameter : node.parameters)
				clone->parameters.push_back(CloneExpression(parameter));

			m_context->currentFunction->calledFunctions.UnboundedSet(targetFuncIndex);

			Validate(*clone);

			return clone;
		}
		else if (IsIntrinsicFunctionType(resolvedType))
		{
			if (targetExpr->GetType() != NodeType::IntrinsicFunctionExpression)
				throw AstError{ "expected intrinsic function expression" };

			std::size_t targetIntrinsicId = static_cast<IntrinsicFunctionExpression&>(*targetExpr).intrinsicId;

			std::vector<ExpressionPtr> parameters;
			parameters.reserve(node.parameters.size());

			for (const auto& param : node.parameters)
				parameters.push_back(CloneExpression(param));

			auto intrinsic = ShaderBuilder::Intrinsic(m_context->intrinsics.Retrieve(targetIntrinsicId), std::move(parameters));
			Validate(*intrinsic);

			return intrinsic;
		}
		else if (IsMethodType(resolvedType))
		{
			const MethodType& methodType = std::get<MethodType>(resolvedType);

			std::vector<ExpressionPtr> parameters;
			parameters.reserve(node.parameters.size() + 1);

			// TODO: Add MethodExpression
			assert(targetExpr->GetType() == NodeType::AccessIdentifierExpression);

			parameters.push_back(std::move(static_cast<AccessIdentifierExpression&>(*targetExpr).expr));
			for (const auto& param : node.parameters)
				parameters.push_back(CloneExpression(param));

			assert(IsSamplerType(methodType.objectType->type) && methodType.methodIndex == 0);
			auto intrinsic = ShaderBuilder::Intrinsic(IntrinsicType::SampleTexture, std::move(parameters));
			Validate(*intrinsic);

			return intrinsic;
		}
		else
		{
			// Calling a type - vec3[f32](0.0, 1.0, 2.0) - it's a cast
			auto clone = std::make_unique<CastExpression>();
			clone->targetType = *targetExprType;

			if (node.parameters.size() > clone->expressions.size())
				throw AstError{ "component count doesn't match required component count" };

			for (std::size_t i = 0; i < node.parameters.size(); ++i)
				clone->expressions[i] = CloneExpression(node.parameters[i]);

			Validate(*clone);

			return Clone(*clone); //< Necessary because cast has to be modified (FIXME)
		}
	}

	ExpressionPtr SanitizeVisitor::Clone(CastExpression& node)
	{
		auto clone = StaticUniquePointerCast<CastExpression>(AstCloner::Clone(node));
		if (Validate(*clone) == ValidationResult::Unresolved)
			return clone; //< unresolved

		const ExpressionType& targetType = clone->targetType.GetResultingValue();

		if (m_context->options.removeMatrixCast && IsMatrixType(targetType))
		{
			const MatrixType& targetMatrixType = std::get<MatrixType>(targetType);

			const ExpressionType& frontExprType = ResolveAlias(GetExpressionTypeSecure(*clone->expressions.front()));
			bool isMatrixCast = IsMatrixType(frontExprType);
			if (isMatrixCast && std::get<MatrixType>(frontExprType) == targetMatrixType)
			{
				// Nothing to do
				return std::move(clone->expressions.front());
			}

			auto variableDeclaration = ShaderBuilder::DeclareVariable("temp", targetType); //< Validation will prevent name-clash if required
			Validate(*variableDeclaration);

			std::size_t variableIndex = *variableDeclaration->varIndex;

			m_context->currentStatementList->emplace_back(std::move(variableDeclaration));

			for (std::size_t i = 0; i < targetMatrixType.columnCount; ++i)
			{
				// temp[i]
				auto columnExpr = ShaderBuilder::AccessIndex(ShaderBuilder::Variable(variableIndex, targetType), ShaderBuilder::Constant(UInt32(i)));
				Validate(*columnExpr);

				// vector expression
				ExpressionPtr vectorExpr;
				std::size_t vectorComponentCount;
				if (isMatrixCast)
				{
					// fromMatrix[i]
					auto matrixColumnExpr = ShaderBuilder::AccessIndex(CloneExpression(clone->expressions.front()), ShaderBuilder::Constant(UInt32(i)));
					Validate(*matrixColumnExpr);

					vectorExpr = std::move(matrixColumnExpr);
					vectorComponentCount = std::get<MatrixType>(frontExprType).rowCount;
				}
				else
				{
					// parameter #i
					vectorExpr = std::move(clone->expressions[i]);
					vectorComponentCount = std::get<VectorType>(ResolveAlias(GetExpressionTypeSecure(*vectorExpr))).componentCount;
				}

				// cast expression (turn fromMatrix[i] to vec3[f32](fromMatrix[i]))
				ExpressionPtr castExpr;
				if (vectorComponentCount != targetMatrixType.rowCount)
				{
					CastExpressionPtr vecCast;
					if (vectorComponentCount < targetMatrixType.rowCount)
					{
						std::array<ExpressionPtr, 4> expressions;
						expressions[0] = std::move(vectorExpr);
						for (std::size_t j = 0; j < targetMatrixType.rowCount - vectorComponentCount; ++j)
							expressions[j + 1] = ShaderBuilder::Constant(ExpressionType{ targetMatrixType.type }, (i == j + vectorComponentCount) ? 1 : 0); //< set 1 to diagonal

						vecCast = ShaderBuilder::Cast(ExpressionType{ VectorType{ targetMatrixType.rowCount, targetMatrixType.type } }, std::move(expressions));
						Validate(*vecCast);

						castExpr = std::move(vecCast);
					}
					else
					{
						std::array<UInt32, 4> swizzleComponents;
						std::iota(swizzleComponents.begin(), swizzleComponents.begin() + targetMatrixType.rowCount, 0);

						auto swizzleExpr = ShaderBuilder::Swizzle(std::move(vectorExpr), swizzleComponents, targetMatrixType.rowCount);
						Validate(*swizzleExpr);

						castExpr = std::move(swizzleExpr);
					}
				}
				else
					castExpr = std::move(vectorExpr);

				// temp[i] = castExpr
				m_context->currentStatementList->emplace_back(ShaderBuilder::ExpressionStatement(ShaderBuilder::Assign(AssignType::Simple, std::move(columnExpr), std::move(castExpr))));
			}

			return ShaderBuilder::Variable(variableIndex, targetType);
		}

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConditionalExpression& node)
	{
		MandatoryExpr(node.condition);
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		ExpressionPtr cloneCondition = AstCloner::Clone(*node.condition);

		std::optional<ConstantValue> conditionValue = ComputeConstantValue(*cloneCondition);
		if (!conditionValue.has_value())
		{
			// Unresolvable condition
			return AstCloner::Clone(node);
		}

		if (GetConstantType(*conditionValue) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };

		if (std::get<bool>(*conditionValue))
			return AstCloner::Clone(*node.truePath);
		else
			return AstCloner::Clone(*node.falsePath);
	}

	ExpressionPtr SanitizeVisitor::Clone(ConstantValueExpression& node)
	{
		if (std::holds_alternative<NoValue>(node.value))
			throw std::runtime_error("expected a value");

		auto clone = StaticUniquePointerCast<ConstantValueExpression>(AstCloner::Clone(node));
		clone->cachedExpressionType = GetConstantType(clone->value);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConstantExpression& node)
	{
		const ConstantValue* value = m_context->constantValues.TryRetrieve(node.constantId);
		if (!value)
		{
			if (!m_context->options.allowPartialSanitization)
				throw std::runtime_error("invalid constant index #" + std::to_string(node.constantId));

			return AstCloner::Clone(node); //< unresolved
		}

		// Replace by constant value
		auto constant = ShaderBuilder::Constant(*value);
		constant->cachedExpressionType = GetConstantType(constant->value);

		return constant;
	}

	ExpressionPtr SanitizeVisitor::Clone(IdentifierExpression& node)
	{
		assert(m_context);

		const IdentifierData* identifierData = FindIdentifier(node.identifier);
		if (!identifierData)
		{
			if (m_context->allowUnknownIdentifiers)
				return AstCloner::Clone(node);

			throw AstError{ "unknown identifier " + node.identifier };
		}

		if (identifierData->category == IdentifierCategory::Unresolved)
			return AstCloner::Clone(node);

		return HandleIdentifier(identifierData);
	}

	ExpressionPtr SanitizeVisitor::Clone(IntrinsicExpression& node)
	{
		auto clone = StaticUniquePointerCast<IntrinsicExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(SwizzleExpression& node)
	{
		auto expression = CloneExpression(MandatoryExpr(node.expression));

		const ExpressionType* exprType = GetExpressionType(*expression);
		if (!exprType)
			return ShaderBuilder::Swizzle(std::move(expression), node.components, node.componentCount); //< unresolved

		const ExpressionType& resolvedExprType = ResolveAlias(*exprType);

		if (m_context->options.removeScalarSwizzling && IsPrimitiveType(resolvedExprType))
		{
			for (std::size_t i = 0; i < node.componentCount; ++i)
			{
				if (node.components[i] != 0)
					throw AstError{ "invalid swizzle" };

			}
			if (node.componentCount == 1)
				return expression; //< ignore this swizzle (a.x == a)

			// Use a Cast expression to replace swizzle
			expression = CacheResult(std::move(expression)); //< Since we are going to use a value multiple times, cache it if required

			PrimitiveType baseType;
			if (IsVectorType(resolvedExprType))
				baseType = std::get<VectorType>(resolvedExprType).type;
			else
				baseType = std::get<PrimitiveType>(resolvedExprType);

			auto cast = std::make_unique<CastExpression>();
			cast->targetType = ExpressionType{ VectorType{ node.componentCount, baseType } };
			for (std::size_t j = 0; j < node.componentCount; ++j)
				cast->expressions[j] = CloneExpression(expression);

			Validate(*cast);

			return cast;
		}
		else
		{
			auto clone = std::make_unique<SwizzleExpression>();
			clone->componentCount = node.componentCount;
			clone->components = node.components;
			clone->expression = std::move(expression);
			Validate(*clone);

			return clone;
		}
	}

	ExpressionPtr SanitizeVisitor::Clone(UnaryExpression& node)
	{
		auto clone = StaticUniquePointerCast<UnaryExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(VariableValueExpression& node)
	{
		auto clone = StaticUniquePointerCast<VariableValueExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(BranchStatement& node)
	{
		if (node.isConst)
		{
			// Evaluate every condition at compilation and select the right statement
			for (auto& cond : node.condStatements)
			{
				MandatoryExpr(cond.condition);

				std::optional<ConstantValue> conditionValue = ComputeConstantValue(*AstCloner::Clone(*cond.condition));
				if (!conditionValue.has_value())
					return AstCloner::Clone(node); //< Unresolvable condition

				if (GetConstantType(*conditionValue) != ExpressionType{ PrimitiveType::Boolean })
					throw AstError{ "expected a boolean value" };

				if (std::get<bool>(*conditionValue))
					return Unscope(AstCloner::Clone(*cond.statement));
			}

			// Every condition failed, fallback to else if any
			if (node.elseStatement)
				return Unscope(AstCloner::Clone(*node.elseStatement));
			else
				return ShaderBuilder::NoOp();
		}

		auto clone = std::make_unique<BranchStatement>();
		clone->condStatements.reserve(node.condStatements.size());

		if (!m_context->currentFunction)
			throw AstError{ "non-const branching statements can only exist inside a function" };

		BranchStatement* root = clone.get();
		for (std::size_t condIndex = 0; condIndex < node.condStatements.size(); ++condIndex)
		{
			auto& cond = node.condStatements[condIndex];

			PushScope();

			auto BuildCondStatement = [&](BranchStatement::ConditionalStatement& condStatement)
			{
				condStatement.condition = CloneExpression(MandatoryExpr(cond.condition));

				const ExpressionType* condType = GetExpressionType(*condStatement.condition);
				if (!condType)
					return ValidationResult::Unresolved;

				if (!IsPrimitiveType(*condType) || std::get<PrimitiveType>(*condType) != PrimitiveType::Boolean)
					throw AstError{ "branch expressions must resolve to boolean type" };

				condStatement.statement = CloneStatement(MandatoryStatement(cond.statement));
				return ValidationResult::Validated;
			};

			if (m_context->options.splitMultipleBranches && condIndex > 0)
			{
				auto currentBranch = std::make_unique<BranchStatement>();

				if (BuildCondStatement(currentBranch->condStatements.emplace_back()) == ValidationResult::Unresolved)
					return AstCloner::Clone(node);

				root->elseStatement = std::move(currentBranch);
				root = static_cast<BranchStatement*>(root->elseStatement.get());
			}
			else
			{
				if (BuildCondStatement(clone->condStatements.emplace_back()) == ValidationResult::Unresolved)
					return AstCloner::Clone(node);
			}

			PopScope();
		}

		if (node.elseStatement)
		{
			PushScope();
			root->elseStatement = CloneStatement(node.elseStatement);
			PopScope();
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(ConditionalStatement& node)
	{
		MandatoryExpr(node.condition);
		MandatoryStatement(node.statement);

		ExpressionPtr cloneCondition = AstCloner::Clone(*node.condition);

		std::optional<ConstantValue> conditionValue = ComputeConstantValue(*cloneCondition);
		if (!conditionValue.has_value())
		{
			// Unresolvable condition
			return ShaderBuilder::ConditionalStatement(std::move(cloneCondition), AstCloner::Clone(*node.statement));
		}

		if (GetConstantType(*conditionValue) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };

		if (std::get<bool>(*conditionValue))
			return AstCloner::Clone(*node.statement);
		else
			return ShaderBuilder::NoOp();
	}

	StatementPtr SanitizeVisitor::Clone(DeclareAliasStatement& node)
	{
		auto clone = StaticUniquePointerCast<DeclareAliasStatement>(AstCloner::Clone(node));
		Validate(*clone);

		if (m_context->options.removeAliases)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareConstStatement& node)
	{
		auto clone = StaticUniquePointerCast<DeclareConstStatement>(AstCloner::Clone(node));

		if (!clone->expression)
			throw AstError{ "const variables must have an expression" };

		clone->expression = PropagateConstants(*clone->expression);
		if (clone->expression->GetType() != NodeType::ConstantValueExpression)
		{
			if (!m_context->options.allowPartialSanitization)
				throw AstError{ "const variable must have constant expressions " };

			clone->constIndex = RegisterConstant(clone->name, std::nullopt, clone->constIndex);
			return clone;
		}

		const ConstantValue& value = static_cast<ConstantValueExpression&>(*clone->expression).value;

		ExpressionType expressionType = GetConstantType(value);

		std::optional<ExpressionType> constType = ResolveTypeExpr(clone->type, true);

		if (clone->type.HasValue() && constType.has_value() && *constType != ResolveAlias(expressionType))
			throw AstError{ "constant expression doesn't match type" };

		clone->type = expressionType;

		clone->constIndex = RegisterConstant(clone->name, value, clone->constIndex);

		if (m_context->options.removeConstDeclaration)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareExternalStatement& node)
	{
		assert(m_context);

		auto clone = StaticUniquePointerCast<DeclareExternalStatement>(AstCloner::Clone(node));

		std::optional<UInt32> defaultBlockSet = 0;
		if (clone->bindingSet.HasValue())
		{
			if (ComputeExprValue(clone->bindingSet) == ValidationResult::Validated)
				defaultBlockSet = clone->bindingSet.GetResultingValue();
			else
				defaultBlockSet.reset(); //< Unresolved value
		}

		for (auto& extVar : clone->externalVars)
		{
			if (!extVar.bindingIndex.HasValue())
				throw AstError{ "external variable " + extVar.name + " requires a binding index" };

			if (extVar.bindingSet.HasValue())
				ComputeExprValue(extVar.bindingSet);
			else if (defaultBlockSet)
				extVar.bindingSet = *defaultBlockSet;

			ComputeExprValue(extVar.bindingIndex);

			if (extVar.bindingSet.IsResultingValue() && extVar.bindingIndex.IsResultingValue())
			{
				UInt64 bindingSet = extVar.bindingSet.GetResultingValue();
				UInt64 bindingIndex = extVar.bindingIndex.GetResultingValue();

				UInt64 bindingKey = bindingSet << 32 | bindingIndex;
				if (m_context->usedBindingIndexes.find(bindingKey) != m_context->usedBindingIndexes.end())
					throw AstError{ "binding (set=" + std::to_string(bindingSet) + ", binding=" + std::to_string(bindingIndex) + ") is already in use" };

				m_context->usedBindingIndexes.insert(bindingKey);
			}

			if (m_context->declaredExternalVar.find(extVar.name) != m_context->declaredExternalVar.end())
				throw AstError{ "external variable " + extVar.name + " is already declared" };

			m_context->declaredExternalVar.insert(extVar.name);

			std::optional<ExpressionType> resolvedType = ResolveTypeExpr(extVar.type);
			if (!resolvedType.has_value())
			{
				RegisterUnresolved(extVar.name);
				continue;
			}

			const ExpressionType& targetType = ResolveAlias(*resolvedType);

			ExpressionType varType;
			if (IsUniformType(targetType))
				varType = std::get<UniformType>(targetType).containedType;
			else if (IsSamplerType(targetType))
				varType = targetType;
			else
				throw AstError{ "external variable " + extVar.name + " is of wrong type: only uniform and sampler are allowed in external blocks" };

			extVar.type = std::move(resolvedType).value();
			extVar.varIndex = RegisterVariable(extVar.name, std::move(varType), extVar.varIndex);

			SanitizeIdentifier(extVar.name);
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareFunctionStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "a function cannot be defined inside another function" };

		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->name = node.name;

		clone->parameters.reserve(node.parameters.size());
		for (auto& parameter : node.parameters)
		{
			auto& cloneParam = clone->parameters.emplace_back();
			cloneParam.name = parameter.name;
			cloneParam.type = CloneType(parameter.type);
			cloneParam.varIndex = parameter.varIndex;
		}

		if (node.returnType.HasValue())
			clone->returnType = CloneType(node.returnType);
		else
			clone->returnType = ExpressionType{ NoType{} };

		if (node.depthWrite.HasValue())
			ComputeExprValue(node.depthWrite, clone->depthWrite);

		if (node.earlyFragmentTests.HasValue())
			ComputeExprValue(node.earlyFragmentTests, clone->earlyFragmentTests);

		if (node.entryStage.HasValue())
			ComputeExprValue(node.entryStage, clone->entryStage);

		if (node.isExported.HasValue())
			ComputeExprValue(node.isExported, clone->isExported);

		if (clone->entryStage.IsResultingValue())
		{
			ShaderStageType stageType = clone->entryStage.GetResultingValue();

			if (!m_context->options.allowPartialSanitization)
			{
				if (m_context->entryFunctions[UnderlyingCast(stageType)])
					throw AstError{ "the same entry type has been defined multiple times" };

				m_context->entryFunctions[UnderlyingCast(stageType)] = &node;
			}

			if (node.parameters.size() > 1)
				throw AstError{ "entry functions can either take one struct parameter or no parameter" };

			if (stageType != ShaderStageType::Fragment)
			{
				if (node.depthWrite.HasValue())
					throw AstError{ "only fragment entry-points can have the depth_write attribute" };

				if (node.earlyFragmentTests.HasValue())
					throw AstError{ "only functions with entry(frag) attribute can have the early_fragments_tests attribute" };
			}
		}

		// Function content is resolved in a second pass
		auto& pendingFunc = m_context->pendingFunctions.emplace_back();
		pendingFunc.cloneNode = clone.get();
		pendingFunc.node = &node;

		if (clone->earlyFragmentTests.HasValue() && clone->earlyFragmentTests.GetResultingValue())
		{
			//TODO: warning and disable early fragment tests
			throw AstError{ "discard is not compatible with early fragment tests" };
		}

		FunctionData funcData;
		funcData.node = clone.get(); //< update function node

		std::size_t funcIndex = RegisterFunction(clone->name, std::move(funcData), node.funcIndex);
		clone->funcIndex = funcIndex;

		SanitizeIdentifier(clone->name);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareOptionStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "options must be declared outside of functions" };

		auto clone = StaticUniquePointerCast<DeclareOptionStatement>(AstCloner::Clone(node));
		if (clone->optName.empty())
			throw AstError{ "empty option name" };

		std::optional<ExpressionType> resolvedOptionType = ResolveTypeExpr(clone->optType);
		if (!resolvedOptionType)
		{
			clone->optIndex = RegisterConstant(clone->optName, std::nullopt, clone->optIndex);
			return clone;
		}

		ExpressionType resolvedType = ResolveType(*resolvedOptionType);
		const ExpressionType& targetType = ResolveAlias(resolvedType);

		if (clone->defaultValue)
		{
			const ExpressionType* defaultValueType = GetExpressionType(*clone->defaultValue);
			if (!defaultValueType)
			{
				clone->optIndex = RegisterConstant(clone->optName, std::nullopt, clone->optIndex);
				return clone; //< unresolved
			}

			if (targetType != *defaultValueType)
				throw AstError{ "option " + clone->optName + " default expression must be of the same type than the option" };
		}

		clone->optType = std::move(resolvedType);

		UInt32 optionHash = CRC32(reinterpret_cast<const UInt8*>(clone->optName.data()), clone->optName.size());

		if (auto optionValueIt = m_context->options.optionValues.find(optionHash); optionValueIt != m_context->options.optionValues.end())
			clone->optIndex = RegisterConstant(clone->optName, optionValueIt->second, node.optIndex);
		else
		{
			if (m_context->options.allowPartialSanitization)
			{
				// Partial sanitization, we cannot give a value to this option
				clone->optIndex = RegisterConstant(clone->optName, std::nullopt, clone->optIndex);
			}
			else
			{

				if (!clone->defaultValue)
					throw AstError{ "missing option " + clone->optName + " value (has no default value)" };

				clone->optIndex = RegisterConstant(clone->optName, ComputeConstantValue(*clone->defaultValue), node.optIndex);
			}
		}

		if (m_context->options.removeOptionDeclaration)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareStructStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "structs must be declared outside of functions" };

		auto clone = StaticUniquePointerCast<DeclareStructStatement>(AstCloner::Clone(node));

		if (clone->isExported.HasValue())
			ComputeExprValue(clone->isExported);

		if (clone->description.layout.HasValue())
			ComputeExprValue(clone->description.layout);

		std::unordered_set<std::string> declaredMembers;
		for (auto& member : clone->description.members)
		{
			if (member.cond.HasValue())
			{
				ComputeExprValue(member.cond);
				if (member.cond.IsResultingValue() && !member.cond.GetResultingValue())
					continue;
			}

			if (member.builtin.HasValue())
				ComputeExprValue(member.builtin);

			if (member.locationIndex.HasValue())
				ComputeExprValue(member.locationIndex);

			if (member.builtin.HasValue() && member.locationIndex.HasValue())
				throw AstError{ "A struct field cannot have both builtin and location attributes" };

			if (declaredMembers.find(member.name) != declaredMembers.end())
			{
				if ((!member.cond.HasValue() || !member.cond.IsResultingValue()) && !m_context->options.allowPartialSanitization)
					throw AstError{ "struct member " + member.name + " found multiple time" };
			}

			declaredMembers.insert(member.name);

			if (member.type.HasValue() && member.type.IsExpression())
			{
				assert(m_context->options.allowPartialSanitization);
				continue;
			}

			ExpressionType resolvedType = member.type.GetResultingValue();
			if (clone->description.layout.IsResultingValue() && clone->description.layout.GetResultingValue() == StructLayout::Std140)
			{
				const ExpressionType& targetType = ResolveAlias(member.type.GetResultingValue());

				if (IsPrimitiveType(targetType) && std::get<PrimitiveType>(targetType) == PrimitiveType::Boolean)
					throw AstError{ "boolean type is not allowed in std140 layout" };
				else if (IsStructType(targetType))
				{
					std::size_t structIndex = std::get<StructType>(targetType).structIndex;
					const StructDescription* desc = m_context->structs.Retrieve(structIndex);
					if (!desc->layout.HasValue() || desc->layout.GetResultingValue() != clone->description.layout.GetResultingValue())
						throw AstError{ "inner struct layout mismatch" };
				}
			}
		}

		clone->structIndex = RegisterStruct(clone->description.name, &clone->description, clone->structIndex);

		SanitizeIdentifier(clone->description.name);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareVariableStatement& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "global variables outside of external blocks are forbidden" };

		auto clone = StaticUniquePointerCast<DeclareVariableStatement>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DiscardStatement& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "discard can only be used inside a function" };

		m_context->currentFunction->flags |= FunctionFlag::DoesDiscard;

		return AstCloner::Clone(node);
	}

	StatementPtr SanitizeVisitor::Clone(ExpressionStatement& node)
	{
		MandatoryExpr(node.expression);

		return AstCloner::Clone(node);
	}

	StatementPtr SanitizeVisitor::Clone(ForStatement& node)
	{
		if (node.varName.empty())
			throw AstError{ "numerical for variable name cannot be empty" };

		auto fromExpr = CloneExpression(MandatoryExpr(node.fromExpr));
		auto stepExpr = CloneExpression(node.stepExpr);
		auto toExpr = CloneExpression(MandatoryExpr(node.toExpr));
		MandatoryStatement(node.statement);

		const ExpressionType* fromExprType = GetExpressionType(*fromExpr);
		const ExpressionType* toExprType = GetExpressionType(*fromExpr);

		ExpressionValue<LoopUnroll> unrollValue;

		auto CloneFor = [&]
		{
			auto clone = std::make_unique<ForStatement>();
			clone->fromExpr = std::move(fromExpr);
			clone->stepExpr = std::move(stepExpr);
			clone->toExpr = std::move(toExpr);
			clone->varName = node.varName;
			clone->unroll = std::move(unrollValue);

			PushScope();
			{
				if (fromExprType)
					clone->varIndex = RegisterVariable(node.varName, *fromExprType, node.varIndex);
				else
				{
					RegisterUnresolved(node.varName);
					clone->varIndex = node.varIndex; //< preserve var index, if set
				}
				clone->statement = CloneStatement(node.statement);
			}
			PopScope();

			SanitizeIdentifier(clone->varName);

			return clone;
		};

		if (node.unroll.HasValue() && ComputeExprValue(node.unroll, unrollValue) == ValidationResult::Unresolved)
			return CloneFor(); //< unresolved unroll

		if (!fromExprType || !toExprType)
			return CloneFor(); //< unresolved from/to type

		const ExpressionType& resolvedFromExprType = ResolveAlias(*fromExprType);
		if (!IsPrimitiveType(resolvedFromExprType))
			throw AstError{ "numerical for from expression must be an integer or unsigned integer" };

		PrimitiveType counterType = std::get<PrimitiveType>(resolvedFromExprType);
		if (counterType != PrimitiveType::Int32 && counterType != PrimitiveType::UInt32)
			throw AstError{ "numerical for from expression must be an integer or unsigned integer" };

		const ExpressionType& resolvedToExprType = ResolveAlias(*toExprType);
		if (resolvedToExprType != resolvedFromExprType)
			throw AstError{ "numerical for to expression type must match from expression type" };

		if (stepExpr)
		{
			const ExpressionType* stepExprType = GetExpressionType(*stepExpr);
			if (!stepExprType)
				return CloneFor(); //< unresolved step type

			const ExpressionType& resolvedStepExprType = ResolveAlias(*stepExprType);
			if (resolvedStepExprType != resolvedFromExprType)
				throw AstError{ "numerical for step expression type must match from expression type" };
		}

		if (unrollValue.HasValue())
		{
			assert(unrollValue.IsResultingValue());
			if (unrollValue.GetResultingValue() == LoopUnroll::Always)
			{
				std::optional<ConstantValue> fromValue = ComputeConstantValue(*fromExpr);
				std::optional<ConstantValue> toValue = ComputeConstantValue(*toExpr);
				if (!fromValue.has_value() || !toValue.has_value())
					return CloneFor(); //< can't resolve step value

				std::optional<ConstantValue> stepValue;
				if (stepExpr)
				{
					stepValue = ComputeConstantValue(*stepExpr);
					if (!stepValue.has_value())
						return CloneFor(); //< can't resolve step value
				}

				PushScope();

				auto multi = std::make_unique<MultiStatement>();

				auto Unroll = [&](auto dummy)
				{
					using T = std::decay_t<decltype(dummy)>;

					T counter = std::get<T>(*fromValue);
					T to = std::get<T>(*toValue);
					T step = (stepExpr) ? std::get<T>(*stepValue) : T(1);

					for (; counter < to; counter += step)
					{
						auto var = ShaderBuilder::DeclareVariable(node.varName, ShaderBuilder::Constant(counter));
						Validate(*var);
						multi->statements.emplace_back(std::move(var));

						multi->statements.emplace_back(Unscope(CloneStatement(node.statement)));
					}
				};

				switch (counterType)
				{
					case PrimitiveType::Int32:
						Unroll(Int32{});
						break;

					case PrimitiveType::UInt32:
						Unroll(UInt32{});
						break;

					default:
						throw AstError{ "internal error" };
				}

				PopScope();

				return multi;
			}
		}

		if (m_context->options.reduceLoopsToWhile)
		{
			PushScope();

			auto multi = std::make_unique<MultiStatement>();

			// Counter variable
			auto counterVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(fromExpr));
			counterVariable->varIndex = node.varIndex;
			Validate(*counterVariable);

			std::size_t counterVarIndex = counterVariable->varIndex.value();
			multi->statements.emplace_back(std::move(counterVariable));

			// Target variable
			auto targetVariable = ShaderBuilder::DeclareVariable("to", std::move(toExpr));
			Validate(*targetVariable);

			std::size_t targetVarIndex = targetVariable->varIndex.value();
			multi->statements.emplace_back(std::move(targetVariable));

			// Step variable
			std::optional<std::size_t> stepVarIndex;

			if (stepExpr)
			{
				auto stepVariable = ShaderBuilder::DeclareVariable("step", std::move(stepExpr));
				Validate(*stepVariable);

				stepVarIndex = stepVariable->varIndex;
				multi->statements.emplace_back(std::move(stepVariable));
			}

			// While
			auto whileStatement = std::make_unique<WhileStatement>();
			whileStatement->unroll = std::move(unrollValue);

			// While condition
			auto condition = ShaderBuilder::Binary(BinaryType::CompLt, ShaderBuilder::Variable(counterVarIndex, counterType), ShaderBuilder::Variable(targetVarIndex, counterType));
			Validate(*condition);

			whileStatement->condition = std::move(condition);

			// While body
			auto body = std::make_unique<MultiStatement>();
			body->statements.reserve(2);

			body->statements.emplace_back(Unscope(CloneStatement(node.statement)));

			ExpressionPtr incrExpr;
			if (stepVarIndex)
				incrExpr = ShaderBuilder::Variable(*stepVarIndex, counterType);
			else
				incrExpr = (counterType == PrimitiveType::Int32) ? ShaderBuilder::Constant(1) : ShaderBuilder::Constant(1u);

			auto incrCounter = ShaderBuilder::Assign(AssignType::CompoundAdd, ShaderBuilder::Variable(counterVarIndex, counterType), std::move(incrExpr));
			Validate(*incrCounter);

			body->statements.emplace_back(ShaderBuilder::ExpressionStatement(std::move(incrCounter)));

			whileStatement->body = std::move(body);

			multi->statements.emplace_back(std::move(whileStatement));

			PopScope();

			return multi;
		}
		else
			return CloneFor();
	}

	StatementPtr SanitizeVisitor::Clone(ForEachStatement& node)
	{
		auto expr = CloneExpression(MandatoryExpr(node.expression));

		if (node.varName.empty())
			throw AstError{ "for-each variable name cannot be empty"};

		const ExpressionType* exprType = GetExpressionType(*expr);
		if (!exprType)
			return AstCloner::Clone(node); //< unresolved expression type

		const ExpressionType& resolvedExprType = ResolveAlias(*exprType);

		ExpressionType innerType;
		if (IsArrayType(resolvedExprType))
		{
			const ArrayType& arrayType = std::get<ArrayType>(resolvedExprType);
			innerType = arrayType.containedType->type;
		}
		else
			throw AstError{ "for-each is only supported on arrays and range expressions" };

		ExpressionValue<LoopUnroll> unrollValue;
		if (node.unroll.HasValue())
		{
			if (ComputeExprValue(node.unroll, unrollValue) == ValidationResult::Unresolved)
				return AstCloner::Clone(node); //< unresolved unroll type

			if (unrollValue.GetResultingValue() == LoopUnroll::Always)
			{
				PushScope();

				// Repeat code
				auto multi = std::make_unique<MultiStatement>();
				if (IsArrayType(resolvedExprType))
				{
					const ArrayType& arrayType = std::get<ArrayType>(resolvedExprType);

					for (UInt32 i = 0; i < arrayType.length; ++i)
					{
						auto accessIndex = ShaderBuilder::AccessIndex(CloneExpression(expr), ShaderBuilder::Constant(i));
						Validate(*accessIndex);

						auto elementVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(accessIndex));
						Validate(*elementVariable);

						multi->statements.emplace_back(std::move(elementVariable));
						multi->statements.emplace_back(Unscope(CloneStatement(node.statement)));
					}
				}

				PopScope();

				return multi;
			}
		}

		if (m_context->options.reduceLoopsToWhile)
		{
			PushScope();

			auto multi = std::make_unique<MultiStatement>();

			if (IsArrayType(resolvedExprType))
			{
				const ArrayType& arrayType = std::get<ArrayType>(resolvedExprType);

				multi->statements.reserve(2);

				// Counter variable
				auto counterVariable = ShaderBuilder::DeclareVariable("i", ShaderBuilder::Constant(0u));
				Validate(*counterVariable);

				std::size_t counterVarIndex = counterVariable->varIndex.value();

				multi->statements.emplace_back(std::move(counterVariable));

				auto whileStatement = std::make_unique<WhileStatement>();
				whileStatement->unroll = std::move(unrollValue);

				// While condition
				auto condition = ShaderBuilder::Binary(BinaryType::CompLt, ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32), ShaderBuilder::Constant(arrayType.length));
				Validate(*condition);
				whileStatement->condition = std::move(condition);

				// While body
				auto body = std::make_unique<MultiStatement>();
				body->statements.reserve(3);

				auto accessIndex = ShaderBuilder::AccessIndex(std::move(expr), ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32));
				Validate(*accessIndex);

				auto elementVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(accessIndex));
				elementVariable->varIndex = node.varIndex; //< Preserve var index
				Validate(*elementVariable);
				body->statements.emplace_back(std::move(elementVariable));

				body->statements.emplace_back(Unscope(CloneStatement(node.statement)));

				auto incrCounter = ShaderBuilder::Assign(AssignType::CompoundAdd, ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32), ShaderBuilder::Constant(1u));
				Validate(*incrCounter);

				body->statements.emplace_back(ShaderBuilder::ExpressionStatement(std::move(incrCounter)));

				whileStatement->body = std::move(body);

				multi->statements.emplace_back(std::move(whileStatement));
			}

			PopScope();

			return multi;
		}
		else
		{
			auto clone = std::make_unique<ForEachStatement>();
			clone->expression = std::move(expr);
			clone->varName = node.varName;
			clone->unroll = std::move(unrollValue);

			PushScope();
			{
				clone->varIndex = RegisterVariable(node.varName, innerType, node.varIndex);
				clone->statement = CloneStatement(node.statement);
			}
			PopScope();

			SanitizeIdentifier(clone->varName);

			return clone;
		}
	}

	StatementPtr SanitizeVisitor::Clone(ImportStatement& node)
	{
		if (!m_context->options.moduleResolver)
		{
			if (!m_context->options.allowPartialSanitization)
				throw AstError{ "module " + node.moduleName + " not found" };

			// when partially sanitizing, importing a whole module could register any identifier, so at this point we can't see unknown identifiers as errors
			m_context->allowUnknownIdentifiers = true;

			return StaticUniquePointerCast<ImportStatement>(AstCloner::Clone(node));
		}

		ModulePtr targetModule = m_context->options.moduleResolver->Resolve(node.moduleName);
		if (!targetModule)
			throw AstError{ "module " + node.moduleName + " not found" };

		std::size_t moduleIndex;

		const Uuid& moduleUuid = targetModule->metadata->moduleId;
		auto it = m_context->moduleByUuid.find(moduleUuid);
		if (it == m_context->moduleByUuid.end())
		{
			m_context->moduleByUuid[moduleUuid] = Context::ModuleIdSentinel;

			// Generate module identifier (based on UUID)
			const auto& moduleUuidBytes = moduleUuid.ToArray();

			SHA256Hash hasher;
			hasher.Begin();
			hasher.Append(moduleUuidBytes.data(), moduleUuidBytes.size());
			hasher.End();

			std::string identifier = "_" + hasher.End().ToHex().substr(0, 8);

			// Load new module
			auto moduleEnvironment = std::make_shared<Environment>();
			moduleEnvironment->parentEnv = m_context->globalEnv;

			auto previousEnv = m_context->currentEnv;
			m_context->currentEnv = moduleEnvironment;

			ModulePtr sanitizedModule = std::make_shared<Module>(targetModule->metadata);

			std::string error;
			sanitizedModule->rootNode = SanitizeInternal(*targetModule->rootNode, &error);
			if (!sanitizedModule->rootNode)
				throw AstError{ "module " + node.moduleName + " compilation failed: " + error };

			moduleIndex = m_context->modules.size();

			assert(m_context->modules.size() == moduleIndex);
			auto& moduleData = m_context->modules.emplace_back();

			// Don't run dependency checker when partially sanitizing
			if (!m_context->options.allowPartialSanitization)
			{
				moduleData.dependenciesVisitor = std::make_unique<DependencyCheckerVisitor>();
				moduleData.dependenciesVisitor->Process(*sanitizedModule->rootNode);
			}

			moduleData.environment = std::move(moduleEnvironment);

			assert(m_context->currentModule->importedModules.size() == moduleIndex);
			auto& importedModule = m_context->currentModule->importedModules.emplace_back();
			importedModule.identifier = identifier;
			importedModule.module = std::move(sanitizedModule);

			m_context->currentEnv = std::move(previousEnv);

			RegisterModule(identifier, moduleIndex);

			m_context->moduleByUuid[moduleUuid] = moduleIndex;
		}
		else
		{
			// Module has already been imported
			moduleIndex = it->second;
			if (moduleIndex == Context::ModuleIdSentinel)
				throw AstError{ "circular import detected" };
		}

		auto& moduleData = m_context->modules[moduleIndex];

		auto& exportedSet = moduleData.exportedSetByModule[m_context->currentEnv->moduleId];

		// Extract exported nodes and their dependencies
		std::vector<DeclareAliasStatementPtr> aliasStatements;

		AstExportVisitor::Callbacks callbacks;
		callbacks.onExportedFunc = [&](DeclareFunctionStatement& node)
		{
			assert(node.funcIndex);

			if (moduleData.dependenciesVisitor)
				moduleData.dependenciesVisitor->MarkFunctionAsUsed(*node.funcIndex);

			if (!exportedSet.usedFunctions.UnboundedTest(*node.funcIndex))
			{
				exportedSet.usedFunctions.UnboundedSet(*node.funcIndex);
				aliasStatements.emplace_back(ShaderBuilder::DeclareAlias(node.name, ShaderBuilder::Function(*node.funcIndex)));
			}
		};

		callbacks.onExportedStruct = [&](DeclareStructStatement& node)
		{
			assert(node.structIndex);

			if (moduleData.dependenciesVisitor)
				moduleData.dependenciesVisitor->MarkStructAsUsed(*node.structIndex);

			if (!exportedSet.usedStructs.UnboundedTest(*node.structIndex))
			{
				exportedSet.usedStructs.UnboundedSet(*node.structIndex);
				aliasStatements.emplace_back(ShaderBuilder::DeclareAlias(node.description.name, ShaderBuilder::StructType(*node.structIndex)));
			}
		};

		AstExportVisitor exportVisitor;
		exportVisitor.Visit(*m_context->currentModule->importedModules[moduleIndex].module->rootNode, callbacks);

		if (aliasStatements.empty())
			return ShaderBuilder::NoOp();

		// Register aliases
		for (auto& aliasPtr : aliasStatements)
			Validate(*aliasPtr);

		if (m_context->options.removeAliases)
			return ShaderBuilder::NoOp();

		// Generate alias statements
		MultiStatementPtr aliasBlock = std::make_unique<MultiStatement>();
		for (auto& aliasPtr : aliasStatements)
			aliasBlock->statements.push_back(std::move(aliasPtr));

		m_context->allowUnknownIdentifiers = true; //< if module uses a unresolved and non-exported symbol, we need to allow unknown identifiers

		return aliasBlock;
	}

	StatementPtr SanitizeVisitor::Clone(MultiStatement& node)
	{
		auto clone = std::make_unique<MultiStatement>();
		clone->statements.reserve(node.statements.size());

		std::vector<StatementPtr>* previousList = m_context->currentStatementList;
		m_context->currentStatementList = &clone->statements;

		for (auto& statement : node.statements)
			clone->statements.push_back(AstCloner::Clone(MandatoryStatement(statement)));

		m_context->currentStatementList = previousList;

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(ScopedStatement& node)
	{
		MandatoryStatement(node.statement);

		PushScope();

		auto scopedClone = AstCloner::Clone(node);

		PopScope();

		return scopedClone;
	}

	StatementPtr SanitizeVisitor::Clone(WhileStatement& node)
	{
		MandatoryExpr(node.condition);
		MandatoryStatement(node.body);

		auto clone = StaticUniquePointerCast<WhileStatement>(AstCloner::Clone(node));
		if (Validate(*clone) == ValidationResult::Unresolved)
			return clone;

		if (clone->unroll.HasValue())
		{
			if (ComputeExprValue(clone->unroll) == ValidationResult::Validated && clone->unroll.GetResultingValue() == LoopUnroll::Always)
				throw AstError{ "unroll(always) is not yet supported on while" };
		}

		return clone;
	}

	auto SanitizeVisitor::FindIdentifier(const std::string_view& identifierName) const -> const IdentifierData*
	{
		return FindIdentifier(*m_context->currentEnv, identifierName);
	}

	template<typename F>
	auto SanitizeVisitor::FindIdentifier(const std::string_view& identifierName, F&& functor) const -> const IdentifierData*
	{
		return FindIdentifier(*m_context->currentEnv, identifierName, std::forward<F>(functor));
	}

	auto SanitizeVisitor::FindIdentifier(const Environment& environment, const std::string_view& identifierName) const -> const IdentifierData*
	{
		auto it = std::find_if(environment.identifiersInScope.rbegin(), environment.identifiersInScope.rend(), [&](const Identifier& identifier) { return identifier.name == identifierName; });
		if (it == environment.identifiersInScope.rend())
		{
			if (environment.parentEnv)
				return FindIdentifier(*environment.parentEnv, identifierName);
			else
				return nullptr;
		}

		return &it->data;
	}

	template<typename F>
	auto SanitizeVisitor::FindIdentifier(const Environment& environment, const std::string_view& identifierName, F&& functor) const -> const IdentifierData*
	{
		auto it = std::find_if(environment.identifiersInScope.rbegin(), environment.identifiersInScope.rend(), [&](const Identifier& identifier)
		{
			if (identifier.name == identifierName)
			{
				if (functor(identifier.data))
					return true;
			}

			return false;
		});
		if (it == environment.identifiersInScope.rend())
		{
			if (environment.parentEnv)
				return FindIdentifier(*environment.parentEnv, identifierName, std::forward<F>(functor));
			else
				return nullptr;
		}

		return &it->data;
	}

	ExpressionPtr SanitizeVisitor::HandleIdentifier(const IdentifierData* identifierData)
	{
		switch (identifierData->category)
		{
			case IdentifierCategory::Alias:
			{
				AliasValueExpression aliasValue;
				aliasValue.aliasId = identifierData->index;

				return Clone(aliasValue);
			}

			case IdentifierCategory::Constant:
			{
				// Replace IdentifierExpression by Constant(Value)Expression
				ConstantExpression constantExpr;
				constantExpr.constantId = identifierData->index;

				return Clone(constantExpr); //< Turn ConstantExpression into ConstantValueExpression
			}

			case IdentifierCategory::Function:
			{
				// Replace IdentifierExpression by FunctionExpression
				auto funcExpr = std::make_unique<FunctionExpression>();
				funcExpr->cachedExpressionType = FunctionType{ identifierData->index }; //< FIXME: Functions (and intrinsic) should be typed by their parameters/return type
				funcExpr->funcId = identifierData->index;

				return funcExpr;
			}

			case IdentifierCategory::Intrinsic:
			{
				IntrinsicType intrinsicType = m_context->intrinsics.Retrieve(identifierData->index);

				// Replace IdentifierExpression by IntrinsicFunctionExpression
				auto intrinsicExpr = std::make_unique<IntrinsicFunctionExpression>();
				intrinsicExpr->cachedExpressionType = IntrinsicFunctionType{ intrinsicType }; //< FIXME: Functions (and intrinsic) should be typed by their parameters/return type
				intrinsicExpr->intrinsicId = identifierData->index;

				return intrinsicExpr;
			}

			case IdentifierCategory::Module:
				throw AstError{ "unexpected module identifier" };

			case IdentifierCategory::Struct:
			{
				// Replace IdentifierExpression by StructTypeExpression
				auto structExpr = std::make_unique<StructTypeExpression>();
				structExpr->cachedExpressionType = StructType{ identifierData->index };
				structExpr->structTypeId = identifierData->index;

				return structExpr;
			}

			case IdentifierCategory::Type:
			{
				auto typeExpr = std::make_unique<TypeExpression>();
				typeExpr->cachedExpressionType = Type{ identifierData->index };
				typeExpr->typeId = identifierData->index;

				return typeExpr;
			}

			case IdentifierCategory::Unresolved:
				throw AstError{ "unexpected unresolved identifier" };

			case IdentifierCategory::Variable:
			{
				// Replace IdentifierExpression by VariableExpression
				auto varExpr = std::make_unique<VariableValueExpression>();
				varExpr->cachedExpressionType = m_context->variableTypes.Retrieve(identifierData->index);
				varExpr->variableId = identifierData->index;

				return varExpr;
			}
		}

		throw AstError{ "internal error" };
	}

	const ExpressionType* SanitizeVisitor::GetExpressionType(Expression& expr) const
	{
		const ExpressionType* expressionType = ShaderAst::GetExpressionType(expr);
		if (!expressionType)
		{
			if (!m_context->options.allowPartialSanitization)
				throw AstError{ "unexpected missing expression type" }; //< InternalError
		}

		return expressionType;
	}

	const ExpressionType& SanitizeVisitor::GetExpressionTypeSecure(Expression& expr) const
	{
		const ExpressionType* expressionType = GetExpressionType(expr);
		if (!expressionType)
			throw AstError{ "unexpected missing expression type" }; //< InternalError

		return *expressionType;
	}

	Expression& SanitizeVisitor::MandatoryExpr(const ExpressionPtr& node) const
	{
		if (!node)
			throw AstError{ "Invalid expression" };

		return *node;
	}

	Statement& SanitizeVisitor::MandatoryStatement(const StatementPtr& node) const
	{
		if (!node)
			throw AstError{ "Invalid statement" };

		return *node;
	}

	void SanitizeVisitor::PushScope()
	{
		auto& scope = m_context->currentEnv->scopes.emplace_back();
		scope.previousSize = m_context->currentEnv->identifiersInScope.size();
	}

	void SanitizeVisitor::PopScope()
	{
		assert(!m_context->currentEnv->scopes.empty());
		auto& scope = m_context->currentEnv->scopes.back();
		m_context->currentEnv->identifiersInScope.resize(scope.previousSize);
		m_context->currentEnv->scopes.pop_back();
	}

	ExpressionPtr SanitizeVisitor::CacheResult(ExpressionPtr expression)
	{
		// No need to cache LValues (variables/constants) (TODO: Improve this, as constants don't need to be cached as well)
		if (GetExpressionCategory(*expression) == ExpressionCategory::LValue)
			return expression;

		assert(m_context->currentStatementList);

		auto variableDeclaration = ShaderBuilder::DeclareVariable("cachedResult", std::move(expression)); //< Validation will prevent name-clash if required
		Validate(*variableDeclaration);

		auto varExpr = std::make_unique<VariableValueExpression>();
		varExpr->variableId = *variableDeclaration->varIndex;

		m_context->currentStatementList->push_back(std::move(variableDeclaration));

		return varExpr;
	}

	std::optional<ConstantValue> SanitizeVisitor::ComputeConstantValue(Expression& expr) const
	{
		// Run optimizer on constant value to hopefully retrieve a single constant value
		ExpressionPtr optimizedExpr = PropagateConstants(expr);
		if (optimizedExpr->GetType() != NodeType::ConstantValueExpression)
		{
			if (!m_context->options.allowPartialSanitization)
				throw AstError{ "expected a constant expression" };

			return std::nullopt;
		}

		return static_cast<ConstantValueExpression&>(*optimizedExpr).value;
	}

	template<typename T>
	auto SanitizeVisitor::ComputeExprValue(ExpressionValue<T>& attribute) const -> ValidationResult
	{
		if (!attribute.HasValue())
			throw AstError{ "attribute expected a value" };

		if (attribute.IsExpression())
		{
			std::optional<ConstantValue> value = ComputeConstantValue(*attribute.GetExpression());
			if (!value)
				return ValidationResult::Unresolved;

			if constexpr (TypeListFind<ConstantTypes, T>)
			{
				if (!std::holds_alternative<T>(*value))
				{
					// HAAAAAX
					if (std::holds_alternative<Int32>(*value) && std::is_same_v<T, UInt32>)
						attribute = static_cast<UInt32>(std::get<Int32>(*value));
					else
						throw AstError{ "unexpected attribute type" };
				}
				else
					attribute = std::get<T>(*value);
			}
			else
				throw AstError{ "unexpected expression for this type" };
		}

		return ValidationResult::Validated;
	}

	template<typename T>
	auto SanitizeVisitor::ComputeExprValue(const ExpressionValue<T>& attribute, ExpressionValue<T>& targetAttribute) -> ValidationResult
	{
		if (!attribute.HasValue())
			throw AstError{ "attribute expected a value" };

		if (attribute.IsExpression())
		{
			std::optional<ConstantValue> value = ComputeConstantValue(*attribute.GetExpression());
			if (!value)
			{
				targetAttribute = AstCloner::Clone(*attribute.GetExpression());
				return ValidationResult::Unresolved;
			}

			if constexpr (TypeListFind<ConstantTypes, T>)
			{
				if (!std::holds_alternative<T>(*value))
				{
					// HAAAAAX
					if (std::holds_alternative<Int32>(*value) && std::is_same_v<T, UInt32>)
						targetAttribute = static_cast<UInt32>(std::get<Int32>(*value));
					else
						throw AstError{ "unexpected attribute type" };
				}
				else
					targetAttribute = std::get<T>(*value);
			}
			else
				throw AstError{ "unexpected expression for this type" };
		}
		else
		{
			assert(attribute.IsResultingValue());
			targetAttribute = attribute.GetResultingValue();
		}

		return ValidationResult::Validated;
	}

	template<typename T>
	std::unique_ptr<T> SanitizeVisitor::PropagateConstants(T& node) const
	{
		AstConstantPropagationVisitor::Options optimizerOptions;
		optimizerOptions.constantQueryCallback = [this](std::size_t constantId) -> const ConstantValue*
		{
			const ConstantValue* value = m_context->constantValues.TryRetrieve(constantId);
			if (!value && !m_context->options.allowPartialSanitization)
				throw AstError{ "invalid constant index #" + std::to_string(constantId) };

			return value;
		};

		// Run optimizer on constant value to hopefully retrieve a single constant value
		return StaticUniquePointerCast<T>(ShaderAst::PropagateConstants(node, optimizerOptions));
	}

	void SanitizeVisitor::PreregisterIndices(const Module& module)
	{
		// If AST has been sanitized before and is sanitized again but with differents options that may introduce new variables (for example reduceLoopsToWhile)
		// we have to make sure we won't override variable indices. This is done by visiting the AST a first time and preregistering all indices.
		// TODO: Only do this is the AST has been already sanitized, maybe using a flag stored in the module?

		AstReflect::Callbacks registerCallbacks;
		registerCallbacks.onAliasIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->aliases.PreregisterIndex(index); };
		registerCallbacks.onConstIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->constantValues.PreregisterIndex(index); };
		registerCallbacks.onFunctionIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->functions.PreregisterIndex(index); };
		registerCallbacks.onOptionIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->constantValues.PreregisterIndex(index); };
		registerCallbacks.onStructIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->structs.PreregisterIndex(index); };
		registerCallbacks.onVariableIndex = [this](const std::string& /*name*/, std::size_t index) { m_context->variableTypes.PreregisterIndex(index); };

		AstReflect reflectVisitor;
		for (const auto& importedModule : module.importedModules)
			reflectVisitor.Reflect(*importedModule.module->rootNode, registerCallbacks);

		reflectVisitor.Reflect(*module.rootNode, registerCallbacks);
	}

	void SanitizeVisitor::PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen)
	{
		auto& funcData = m_context->functions.Retrieve(funcIndex);
		funcData.flags |= flags;

		for (std::size_t i = funcData.calledByFunctions.FindFirst(); i != funcData.calledByFunctions.npos; i = funcData.calledByFunctions.FindNext(i))
			PropagateFunctionFlags(i, funcData.flags, seen);
	}
	
	void SanitizeVisitor::RegisterBuiltin()
	{
		// Primitive types
		RegisterType("bool", PrimitiveType::Boolean);
		RegisterType("f32", PrimitiveType::Float32);
		RegisterType("i32", PrimitiveType::Int32);
		RegisterType("u32", PrimitiveType::UInt32);

		// Partial types

		// Array
		RegisterType("array", PartialType {
			{ TypeParameterCategory::FullType, TypeParameterCategory::ConstantValue },
			[=](const TypeParameter* parameters, std::size_t parameterCount) -> ExpressionType
			{
				assert(parameterCount == 2);
				assert(std::holds_alternative<ExpressionType>(parameters[0]));
				assert(std::holds_alternative<ConstantValue>(parameters[1]));

				const ExpressionType& exprType = std::get<ExpressionType>(parameters[0]);
				const ConstantValue& length = std::get<ConstantValue>(parameters[1]);

				UInt32 lengthValue;
				if (std::holds_alternative<Int32>(length))
				{
					Int32 value = std::get<Int32>(length);
					if (value <= 0)
						throw AstError{ "array length must a positive integer" };

					lengthValue = SafeCast<UInt32>(value);
				}
				else if (std::holds_alternative<UInt32>(length))
				{
					lengthValue = std::get<UInt32>(length);
					if (lengthValue == 0)
						throw AstError{ "array length must a positive integer" };
				}
				else
					throw AstError{ "array length must a positive integer" };

				ArrayType arrayType;
				arrayType.containedType = std::make_unique<ContainedType>();
				arrayType.containedType->type = exprType;
				arrayType.length = lengthValue;

				return arrayType;
			}
		});

		// matX
		for (std::size_t componentCount = 2; componentCount <= 4; ++componentCount)
		{
			RegisterType("mat" + std::to_string(componentCount), PartialType {
				{ TypeParameterCategory::PrimitiveType },
				[=](const TypeParameter* parameters, std::size_t parameterCount) -> ExpressionType
				{
					assert(parameterCount == 1);
					assert(std::holds_alternative<ExpressionType>(*parameters));

					const ExpressionType& exprType = std::get<ExpressionType>(*parameters);
					assert(IsPrimitiveType(exprType));

					return MatrixType {
						componentCount, componentCount, std::get<PrimitiveType>(exprType)
					};
				}
			});
		}

		// vecX
		for (std::size_t componentCount = 2; componentCount <= 4; ++componentCount)
		{
			RegisterType("vec" + std::to_string(componentCount), PartialType {
				{ TypeParameterCategory::PrimitiveType },
				[=](const TypeParameter* parameters, std::size_t parameterCount) -> ExpressionType
				{
					assert(parameterCount == 1);
					assert(std::holds_alternative<ExpressionType>(*parameters));

					const ExpressionType& exprType = std::get<ExpressionType>(*parameters);
					assert(IsPrimitiveType(exprType));

					return VectorType {
						componentCount, std::get<PrimitiveType>(exprType)
					};
				}
			});
		}

		// samplers
		struct SamplerInfo
		{
			std::string typeName;
			ImageType imageType;
		};

		std::array<SamplerInfo, 2> samplerInfos = {
			{
				{
					"sampler2D",
					ImageType::E2D
				},
				{
					"samplerCube",
					ImageType::Cubemap
				}
			}
		};

		for (SamplerInfo& sampler : samplerInfos)
		{
			RegisterType(std::move(sampler.typeName), PartialType {
				{ TypeParameterCategory::PrimitiveType },
				[=](const TypeParameter* parameters, std::size_t parameterCount) -> ExpressionType
				{
					assert(parameterCount == 1);
					assert(std::holds_alternative<ExpressionType>(*parameters));

					const ExpressionType& exprType = std::get<ExpressionType>(*parameters);
					assert(IsPrimitiveType(exprType));

					PrimitiveType primitiveType = std::get<PrimitiveType>(exprType);

					// TODO: Add support for integer samplers
					if (primitiveType != PrimitiveType::Float32)
						throw AstError{ "for now only f32 samplers are supported" };

					return SamplerType {
						sampler.imageType, primitiveType
					};
				}
			});
		}

		// uniform
		RegisterType("uniform", PartialType {
			{ TypeParameterCategory::StructType },
			[=](const TypeParameter* parameters, std::size_t parameterCount) -> ExpressionType
			{
				assert(parameterCount == 1);
				assert(std::holds_alternative<ExpressionType>(*parameters));

				const ExpressionType& exprType = std::get<ExpressionType>(*parameters);
				assert(IsStructType(exprType));

				StructType structType = std::get<StructType>(exprType);
				return UniformType {
					structType
				};
			}
		});

		// Intrinsics
		RegisterIntrinsic("cross", IntrinsicType::CrossProduct);
		RegisterIntrinsic("dot", IntrinsicType::DotProduct);
		RegisterIntrinsic("exp", IntrinsicType::Exp);
		RegisterIntrinsic("length", IntrinsicType::Length);
		RegisterIntrinsic("max", IntrinsicType::Max);
		RegisterIntrinsic("min", IntrinsicType::Min);
		RegisterIntrinsic("normalize", IntrinsicType::Normalize);
		RegisterIntrinsic("pow", IntrinsicType::Pow);
		RegisterIntrinsic("reflect", IntrinsicType::Reflect);
	}

	std::size_t SanitizeVisitor::RegisterAlias(std::string name, std::optional<IdentifierData> aliasData, std::optional<std::size_t> index)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t aliasIndex;
		if (aliasData)
			aliasIndex = m_context->aliases.Register(std::move(*aliasData), index);
		else if (index)
		{
			m_context->aliases.PreregisterIndex(*index);
			aliasIndex = *index;
		}
		else
			aliasIndex = m_context->aliases.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			aliasIndex,
			IdentifierCategory::Alias
		});

		return aliasIndex;
	}

	std::size_t SanitizeVisitor::RegisterConstant(std::string name, std::optional<ConstantValue> value, std::optional<std::size_t> index)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t constantIndex;
		if (value)
			constantIndex = m_context->constantValues.Register(std::move(*value), index);
		else if (index)
		{
			m_context->constantValues.PreregisterIndex(*index);
			constantIndex = *index;
		}
		else
			constantIndex = m_context->constantValues.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			constantIndex,
			IdentifierCategory::Constant
		});

		return constantIndex;
	}

	std::size_t SanitizeVisitor::RegisterFunction(std::string name, std::optional<FunctionData> funcData, std::optional<std::size_t> index)
	{
		if (auto* identifier = FindIdentifier(name))
		{
			// Functions can be conditionally defined and condition not resolved yet, allow duplicates when partially sanitizing
			bool duplicate = !m_context->options.allowPartialSanitization;

			// Functions cannot be declared twice, except for entry ones if their stages are different
			if (funcData)
			{
				if (funcData->node->entryStage.HasValue() && identifier->category == IdentifierCategory::Function)
				{
					auto& otherFunction = m_context->functions.Retrieve(identifier->index);
					if (funcData->node->entryStage.GetResultingValue() != otherFunction.node->entryStage.GetResultingValue())
						duplicate = false;
				}
			}
			else
			{
				if (!m_context->options.allowPartialSanitization)
					throw AstError{ "internal error" };

				duplicate = false;
			}

			if (duplicate)
				throw AstError{ name + " is already used" };
		}

		std::size_t functionIndex;
		if (funcData)
			functionIndex = m_context->functions.Register(std::move(*funcData), index);
		else if (index)
		{
			m_context->functions.PreregisterIndex(*index);
			functionIndex = *index;
		}
		else
			functionIndex = m_context->functions.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			functionIndex,
			IdentifierCategory::Function
		});

		return functionIndex;
	}

	std::size_t SanitizeVisitor::RegisterIntrinsic(std::string name, IntrinsicType type)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t intrinsicIndex = m_context->intrinsics.Register(std::move(type));

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			intrinsicIndex,
			IdentifierCategory::Intrinsic
		});

		return intrinsicIndex;
	}

	std::size_t SanitizeVisitor::RegisterModule(std::string moduleIdentifier, std::size_t index)
	{
		if (FindIdentifier(moduleIdentifier))
			throw AstError{ moduleIdentifier + " is already used" };

		std::size_t moduleIndex = m_context->moduleIndices.Register(index);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(moduleIdentifier),
			moduleIndex,
			IdentifierCategory::Module
		});

		return moduleIndex;
	}

	std::size_t SanitizeVisitor::RegisterStruct(std::string name, std::optional<StructDescription*> description, std::optional<std::size_t> index)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t structIndex;
		if (description)
			structIndex = m_context->structs.Register(*description, index);
		else if (index)
		{
			m_context->structs.PreregisterIndex(*index);
			structIndex = *index;
		}
		else
			structIndex = m_context->structs.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			structIndex,
			IdentifierCategory::Struct
		});

		return structIndex;
	}

	std::size_t SanitizeVisitor::RegisterType(std::string name, std::optional<ExpressionType> expressionType, std::optional<std::size_t> index)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t typeIndex;
		if (expressionType)
			typeIndex = m_context->types.Register(std::move(*expressionType), index);
		else if (index)
		{
			m_context->types.PreregisterIndex(*index);
			typeIndex = *index;
		}
		else
			typeIndex = m_context->types.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			typeIndex,
			IdentifierCategory::Type
		});

		return typeIndex;
	}

	std::size_t SanitizeVisitor::RegisterType(std::string name, std::optional<PartialType> partialType, std::optional<std::size_t> index)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t typeIndex;
		if (partialType)
			typeIndex = m_context->types.Register(std::move(*partialType), index);
		else if (index)
		{
			m_context->types.PreregisterIndex(*index);
			typeIndex = *index;
		}
		else
			typeIndex = m_context->types.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			typeIndex,
			IdentifierCategory::Type
		});

		return typeIndex;
	}

	void SanitizeVisitor::RegisterUnresolved(std::string name)
	{
		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			std::numeric_limits<std::size_t>::max(),
			IdentifierCategory::Unresolved
		});
	}

	std::size_t SanitizeVisitor::RegisterVariable(std::string name, std::optional<ExpressionType> type, std::optional<std::size_t> index)
	{
		if (auto* identifier = FindIdentifier(name))
		{
			// Allow variable shadowing
			if (identifier->category != IdentifierCategory::Variable)
				throw AstError{ name + " is already used" };
		}

		std::size_t varIndex;
		if (type)
			varIndex = m_context->variableTypes.Register(std::move(*type), index);
		else if (index)
		{
			m_context->variableTypes.PreregisterIndex(*index);
			varIndex = *index;
		}
		else
			varIndex = m_context->variableTypes.RegisterNewIndex(true);

		m_context->currentEnv->identifiersInScope.push_back({
			std::move(name),
			varIndex,
			IdentifierCategory::Variable
		});

		return varIndex;
	}

	auto SanitizeVisitor::ResolveAliasIdentifier(const IdentifierData* identifier) const -> const IdentifierData*
	{
		while (identifier->category == IdentifierCategory::Alias)
			identifier = &m_context->aliases.Retrieve(identifier->index);

		return identifier;
	}

	void SanitizeVisitor::ResolveFunctions()
	{
		// Once every function is known, we can evaluate function content
		for (auto& pendingFunc : m_context->pendingFunctions)
		{
			PushScope();

			for (auto& parameter : pendingFunc.cloneNode->parameters)
			{
				parameter.varIndex = RegisterVariable(parameter.name, parameter.type.GetResultingValue(), parameter.varIndex);
				SanitizeIdentifier(parameter.name);
			}

			CurrentFunctionData tempFuncData;
			if (pendingFunc.cloneNode->entryStage.HasValue())
				tempFuncData.stageType = pendingFunc.cloneNode->entryStage.GetResultingValue();

			m_context->currentFunction = &tempFuncData;

			std::vector<StatementPtr>* previousList = m_context->currentStatementList;
			m_context->currentStatementList = &pendingFunc.cloneNode->statements;

			pendingFunc.cloneNode->statements.reserve(pendingFunc.node->statements.size());
			for (auto& statement : pendingFunc.node->statements)
				pendingFunc.cloneNode->statements.push_back(CloneStatement(MandatoryStatement(statement)));

			m_context->currentStatementList = previousList;
			m_context->currentFunction = nullptr;

			std::size_t funcIndex = *pendingFunc.cloneNode->funcIndex;
			for (std::size_t i = tempFuncData.calledFunctions.FindFirst(); i != tempFuncData.calledFunctions.npos; i = tempFuncData.calledFunctions.FindNext(i))
			{
				auto& targetFunc = m_context->functions.Retrieve(i);
				targetFunc.calledByFunctions.UnboundedSet(funcIndex);
			}

			PopScope();
		}
		m_context->pendingFunctions.clear();

		Bitset<> seen;
		for (const auto& [funcIndex, funcData] : m_context->functions.values)
		{
			PropagateFunctionFlags(funcIndex, funcData.flags, seen);
			seen.Clear();
		}

		for (const auto& [funcIndex, funcData] : m_context->functions.values)
		{
			if (funcData.flags.Test(FunctionFlag::DoesDiscard) && funcData.node->entryStage.HasValue() && funcData.node->entryStage.GetResultingValue() != ShaderStageType::Fragment)
				throw AstError{ "discard can only be used in the fragment stage" };
		}
	}

	std::size_t SanitizeVisitor::ResolveStruct(const AliasType& aliasType)
	{
		return ResolveStruct(aliasType.targetType->type);
	}

	std::size_t SanitizeVisitor::ResolveStruct(const ExpressionType& exprType)
	{
		return std::visit([&](auto&& arg) -> std::size_t
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, IdentifierType> || std::is_same_v<T, StructType> || std::is_same_v<T, UniformType> || std::is_same_v<T, AliasType>)
				return ResolveStruct(arg);
			else if constexpr (std::is_same_v<T, NoType> ||
			                   std::is_same_v<T, ArrayType> ||
			                   std::is_same_v<T, FunctionType> ||
			                   std::is_same_v<T, IntrinsicFunctionType> ||
			                   std::is_same_v<T, PrimitiveType> ||
			                   std::is_same_v<T, MatrixType> ||
			                   std::is_same_v<T, MethodType> ||
			                   std::is_same_v<T, SamplerType> ||
			                   std::is_same_v<T, Type> ||
			                   std::is_same_v<T, VectorType>)
			{
				throw AstError{ "expression is not a structure" };
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, exprType);
	}

	std::size_t SanitizeVisitor::ResolveStruct(const IdentifierType& identifierType)
	{
		const IdentifierData* identifierData = FindIdentifier(identifierType.name);
		if (!identifierData)
			throw AstError{ "unknown identifier " + identifierType.name };

		if (identifierData->category != IdentifierCategory::Struct)
			throw AstError{ identifierType.name + " is not a struct" };

		return identifierData->index;
	}

	std::size_t SanitizeVisitor::ResolveStruct(const StructType& structType)
	{
		return structType.structIndex;
	}

	std::size_t SanitizeVisitor::ResolveStruct(const UniformType& uniformType)
	{
		return uniformType.containedType.structIndex;
	}

	ExpressionType SanitizeVisitor::ResolveType(const ExpressionType& exprType, bool resolveAlias)
	{
		if (!IsTypeExpression(exprType))
		{
			if (resolveAlias || m_context->options.removeAliases)
				return ResolveAlias(exprType);
			else
				return exprType;
		}

		std::size_t typeIndex = std::get<Type>(exprType).typeIndex;

		const auto& type = m_context->types.Retrieve(typeIndex);
		if (std::holds_alternative<PartialType>(type))
			throw AstError{ "full type expected" };

		return std::get<ExpressionType>(type);
	}

	std::optional<ExpressionType> SanitizeVisitor::ResolveTypeExpr(const ExpressionValue<ExpressionType>& exprTypeValue, bool resolveAlias)
	{
		if (!exprTypeValue.HasValue())
			return NoType{};

		if (exprTypeValue.IsResultingValue())
			return ResolveType(exprTypeValue.GetResultingValue(), resolveAlias);

		assert(exprTypeValue.IsExpression());
		ExpressionPtr expression = CloneExpression(exprTypeValue.GetExpression());
		const ExpressionType* exprType = GetExpressionType(*expression);
		if (!exprType)
			return std::nullopt;

		//if (!IsTypeType(exprType))
		//	throw AstError{ "type expected" };

		return ResolveType(*exprType, resolveAlias);
	}

	void SanitizeVisitor::SanitizeIdentifier(std::string& identifier)
	{
		// Append _ until the identifier is no longer found
		while (m_context->options.reservedIdentifiers.find(identifier) != m_context->options.reservedIdentifiers.end())
		{
			do 
			{
				identifier += "_";
			}
			while (FindIdentifier(identifier) != nullptr);
		}
	}
	
	MultiStatementPtr SanitizeVisitor::SanitizeInternal(MultiStatement& rootNode, std::string* error)
	{
		MultiStatementPtr output;
		{
			// First pass, evaluate everything except function code
			try
			{
				output = StaticUniquePointerCast<MultiStatement>(AstCloner::Clone(rootNode));
			}
			catch (const AstError& err)
			{
				if (!error)
					throw std::runtime_error(err.errMsg);

				*error = err.errMsg;
			}
			catch (const std::runtime_error& err)
			{
				if (!error)
					throw;

				*error = err.what();
			}

			ResolveFunctions();
		}

		return output;
	}

	auto SanitizeVisitor::TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right) const -> ValidationResult
	{
		const ExpressionType* leftType = GetExpressionType(*left);
		const ExpressionType* rightType = GetExpressionType(*right);
		if (!leftType || !rightType)
			return ValidationResult::Unresolved;

		TypeMustMatch(*leftType, *rightType);
		return ValidationResult::Validated;
	}

	void SanitizeVisitor::TypeMustMatch(const ExpressionType& left, const ExpressionType& right) const
	{
		if (ResolveAlias(left) != ResolveAlias(right))
			throw AstError{ "Left expression type must match right expression type" };
	}

	StatementPtr SanitizeVisitor::Unscope(StatementPtr node)
	{
		assert(node);

		if (node->GetType() == NodeType::ScopedStatement)
			return std::move(static_cast<ScopedStatement&>(*node).statement);
		else
			return node;
	}

	auto SanitizeVisitor::Validate(DeclareAliasStatement& node) -> ValidationResult
	{
		if (node.name.empty())
			throw std::runtime_error("invalid alias name");

		const ExpressionType* exprType = GetExpressionType(*node.expression);
		if (!exprType)
			return ValidationResult::Unresolved;

		const ExpressionType& resolvedType = ResolveAlias(*exprType);

		IdentifierData targetIdentifier;
		if (IsStructType(resolvedType))
		{
			std::size_t structIndex = ResolveStruct(resolvedType);
			targetIdentifier = { structIndex, IdentifierCategory::Struct };
		}
		else if (IsFunctionType(resolvedType))
		{
			std::size_t funcIndex = std::get<FunctionType>(resolvedType).funcIndex;
			targetIdentifier = { funcIndex, IdentifierCategory::Function };
		}
		else if (IsAliasType(resolvedType))
		{
			const AliasType& alias = std::get<AliasType>(resolvedType);
			targetIdentifier = { alias.aliasIndex, IdentifierCategory::Alias };
		}
		else
			throw AstError{ "for now, only aliases, functions and structs can be aliased" };

		node.aliasIndex = RegisterAlias(node.name, targetIdentifier, node.aliasIndex);
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(WhileStatement& node) -> ValidationResult
	{
		const ExpressionType* conditionType = GetExpressionType(MandatoryExpr(node.condition));
		MandatoryStatement(node.body);

		if (!conditionType)
			return ValidationResult::Unresolved;

		if (ResolveAlias(*conditionType) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };

		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(AccessIndexExpression& node) -> ValidationResult
	{
		const ExpressionType* exprType = GetExpressionType(MandatoryExpr(node.expr));
		if (!exprType)
			return ValidationResult::Unresolved;

		ExpressionType resolvedExprType = ResolveAlias(*exprType);

		if (IsTypeExpression(resolvedExprType))
		{
			std::size_t typeIndex = std::get<Type>(resolvedExprType).typeIndex;
			const auto& type = m_context->types.Retrieve(typeIndex);

			if (!std::holds_alternative<PartialType>(type))
				throw std::runtime_error("only partial types can be specialized");

			const PartialType& partialType = std::get<PartialType>(type);
			if (partialType.parameters.size() != node.indices.size())
				throw std::runtime_error("parameter count mismatch");

			StackVector<TypeParameter> parameters = NazaraStackVector(TypeParameter, partialType.parameters.size());
			for (std::size_t i = 0; i < partialType.parameters.size(); ++i)
			{
				const ExpressionPtr& indexExpr = node.indices[i];
				switch (partialType.parameters[i])
				{
					case TypeParameterCategory::ConstantValue:
					{
						std::optional<ConstantValue> value = ComputeConstantValue(*indexExpr);
						if (!value.has_value())
							return ValidationResult::Unresolved;

						parameters.push_back(std::move(*value));
						break;
					}

					case TypeParameterCategory::FullType:
					case TypeParameterCategory::PrimitiveType:
					case TypeParameterCategory::StructType:
					{
						const ExpressionType* indexExprType = GetExpressionType(*indexExpr);
						if (!indexExprType)
							return ValidationResult::Unresolved;

						ExpressionType resolvedType = ResolveType(*indexExprType, true);

						switch (partialType.parameters[i])
						{
							case TypeParameterCategory::PrimitiveType:
							{
								if (!IsPrimitiveType(resolvedType))
									throw std::runtime_error("expected a primitive type");

								break;
							}

							case TypeParameterCategory::StructType:
							{
								if (!IsStructType(resolvedType))
									throw std::runtime_error("expected a struct type");

								break;
							}

							default:
								break;
						}

						parameters.push_back(resolvedType);
						break;
					}
				}
			}

			assert(parameters.size() == partialType.parameters.size());
			node.cachedExpressionType = partialType.buildFunc(parameters.data(), parameters.size());
		}
		else
		{
			if (node.indices.size() != 1)
				throw AstError{ "AccessIndexExpression must have at one index" };

			for (const auto& indexExpr : node.indices)
			{
				const ExpressionType* indexType = GetExpressionType(*indexExpr);
				if (!indexType)
					return ValidationResult::Unresolved;

				if (!IsPrimitiveType(*indexType))
					throw AstError{ "AccessIndex expects integer indices" };

				PrimitiveType primitiveIndexType = std::get<PrimitiveType>(*indexType);
				if (primitiveIndexType != PrimitiveType::Int32 && primitiveIndexType != PrimitiveType::UInt32)
					throw AstError{ "AccessIndex expects integer indices" };

				if (IsArrayType(resolvedExprType))
				{
					const ArrayType& arrayType = std::get<ArrayType>(resolvedExprType);
					ExpressionType containedType = arrayType.containedType->type; //< Don't overwrite exprType directly since it contains arrayType
					resolvedExprType = std::move(containedType);
				}
				else if (IsStructType(resolvedExprType))
				{
					if (primitiveIndexType != PrimitiveType::Int32)
						throw AstError{ "struct can only be accessed with constant i32 indices" };

					ConstantValueExpression& constantExpr = static_cast<ConstantValueExpression&>(*indexExpr);

					Int32 index = std::get<Int32>(constantExpr.value);

					std::size_t structIndex = ResolveStruct(resolvedExprType);
					const StructDescription* s = m_context->structs.Retrieve(structIndex);

					std::optional<ExpressionType> resolvedExprTypeOpt = ResolveTypeExpr(s->members[index].type, true);
					if (!resolvedExprTypeOpt.has_value())
						return ValidationResult::Unresolved;

					resolvedExprType = std::move(resolvedExprTypeOpt).value();
				}
				else if (IsMatrixType(resolvedExprType))
				{
					// Matrix index (ex: mat[2])
					MatrixType matrixType = std::get<MatrixType>(resolvedExprType);

					//TODO: Handle row-major matrices
					resolvedExprType = VectorType{ matrixType.rowCount, matrixType.type };
				}
				else if (IsVectorType(resolvedExprType))
				{
					// Swizzle expression with one component (ex: vec[2])
					VectorType swizzledVec = std::get<VectorType>(resolvedExprType);

					resolvedExprType = swizzledVec.type;
				}
				else
					throw AstError{ "unexpected type (only struct, vectors and matrices can be indexed)" }; //< TODO: Add support for arrays
			}

			node.cachedExpressionType = std::move(resolvedExprType);
		}

		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(AssignExpression& node) -> ValidationResult
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		const ExpressionType* leftExprType = GetExpressionType(MandatoryExpr(node.left));
		if (!leftExprType)
			return ValidationResult::Unresolved;

		const ExpressionType* rightExprType = GetExpressionType(MandatoryExpr(node.right));
		if (!rightExprType)
			return ValidationResult::Unresolved;

		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError{ "Assignation is only possible with a l-value" };

		std::optional<BinaryType> binaryType;
		switch (node.op)
		{
			case AssignType::Simple:
				if (TypeMustMatch(node.left, node.right) == ValidationResult::Unresolved)
					return ValidationResult::Unresolved;

				break;

			case AssignType::CompoundAdd: binaryType = BinaryType::Add; break;
			case AssignType::CompoundDivide: binaryType = BinaryType::Divide; break;
			case AssignType::CompoundMultiply: binaryType = BinaryType::Multiply; break;
			case AssignType::CompoundLogicalAnd: binaryType = BinaryType::LogicalAnd; break;
			case AssignType::CompoundLogicalOr: binaryType = BinaryType::LogicalOr; break;
			case AssignType::CompoundSubtract: binaryType = BinaryType::Subtract; break;
		}

		if (binaryType)
		{
			ExpressionType expressionType = ValidateBinaryOp(*binaryType, ResolveAlias(*leftExprType), ResolveAlias(*rightExprType));
			TypeMustMatch(*leftExprType, expressionType);

			if (m_context->options.removeCompoundAssignments)
			{
				node.op = AssignType::Simple;
				node.right = ShaderBuilder::Binary(*binaryType, AstCloner::Clone(*node.left), std::move(node.right));
				node.right->cachedExpressionType = std::move(expressionType);
			}
		}

		node.cachedExpressionType = *leftExprType;
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(BinaryExpression& node) -> ValidationResult
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		const ExpressionType* leftExprType = GetExpressionType(MandatoryExpr(node.left));
		if (!leftExprType)
			return ValidationResult::Unresolved;

		const ExpressionType* rightExprType = GetExpressionType(MandatoryExpr(node.right));
		if (!rightExprType)
			return ValidationResult::Unresolved;

		node.cachedExpressionType = ValidateBinaryOp(node.op, ResolveAlias(*leftExprType), ResolveAlias(*rightExprType));
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(CallFunctionExpression& node) -> ValidationResult
	{
		std::size_t targetFuncIndex;
		if (node.targetFunction->GetType() == NodeType::FunctionExpression)
			targetFuncIndex = static_cast<FunctionExpression&>(*node.targetFunction).funcId;
		else if (node.targetFunction->GetType() == NodeType::AliasValueExpression)
		{
			const auto& alias = static_cast<AliasValueExpression&>(*node.targetFunction);

			const IdentifierData* targetIdentifier = ResolveAliasIdentifier(&m_context->aliases.Retrieve(alias.aliasId));
			if (targetIdentifier->category != IdentifierCategory::Function)
				throw AstError{ "expected function expression" };

			targetFuncIndex = targetIdentifier->index;
		}
		else
			throw AstError{ "expected function expression" };

		auto& funcData = m_context->functions.Retrieve(targetFuncIndex);

		const DeclareFunctionStatement* referenceDeclaration = funcData.node;

		if (referenceDeclaration->entryStage.HasValue())
			throw AstError{ referenceDeclaration->name + " is an entry function which cannot be called by the program" };

		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			const ExpressionType* parameterType = GetExpressionType(*node.parameters[i]);
			if (!parameterType)
				return ValidationResult::Unresolved;

			if (ResolveAlias(*parameterType) != ResolveAlias(referenceDeclaration->parameters[i].type.GetResultingValue()))
				throw AstError{ "function " + referenceDeclaration->name + " parameter " + std::to_string(i) + " type mismatch" };
		}

		if (node.parameters.size() != referenceDeclaration->parameters.size())
			throw AstError{ "function " + referenceDeclaration->name + " expected " + std::to_string(referenceDeclaration->parameters.size()) + " parameters, got " + std::to_string(node.parameters.size()) };

		node.cachedExpressionType = referenceDeclaration->returnType.GetResultingValue();
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(CastExpression& node) -> ValidationResult
	{
		std::optional<ExpressionType> targetTypeOpt = ResolveTypeExpr(node.targetType);
		if (!targetTypeOpt)
			return ValidationResult::Unresolved;

		const ExpressionType& targetType = ResolveAlias(*targetTypeOpt);

		const auto& firstExprPtr = node.expressions.front();
		if (!firstExprPtr)
			throw AstError{ "expected at least one expression" };

		if (IsMatrixType(targetType))
		{
			const MatrixType& targetMatrixType = std::get<MatrixType>(targetType);

			const ExpressionType* firstExprType = GetExpressionType(*firstExprPtr);
			if (!firstExprType)
				return ValidationResult::Unresolved;

			if (IsMatrixType(ResolveAlias(*firstExprType)))
			{
				if (node.expressions[1])
					throw AstError{ "too many expressions" };

				// Matrix to matrix cast: always valid
			}
			else
			{
				assert(targetMatrixType.columnCount <= 4);
				for (std::size_t i = 0; i < targetMatrixType.columnCount; ++i)
				{
					const auto& exprPtr = node.expressions[i];
					if (!exprPtr)
						throw AstError{ "component count doesn't match required component count" };

					const ExpressionType* exprType = GetExpressionType(*exprPtr);
					if (!exprType)
						return ValidationResult::Unresolved;

					const ExpressionType& resolvedExprType = ResolveAlias(*exprType);
					if (!IsVectorType(resolvedExprType))
						throw AstError{ "expected vector type" };

					const VectorType& vecType = std::get<VectorType>(resolvedExprType);
					if (vecType.componentCount != targetMatrixType.rowCount)
						throw AstError{ "vector component count must match target matrix row count" };
				}
			}
		}
		else
		{
			auto GetComponentCount = [](const ExpressionType& exprType) -> std::size_t
			{
				if (IsVectorType(exprType))
					return std::get<VectorType>(exprType).componentCount;
				else
				{
					assert(IsPrimitiveType(exprType));
					return 1;
				}
			};

			std::size_t componentCount = 0;
			std::size_t requiredComponents = GetComponentCount(targetType);

			for (auto& exprPtr : node.expressions)
			{
				if (!exprPtr)
					break;

				const ExpressionType* exprType = GetExpressionType(*exprPtr);
				if (!exprType)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedExprType = ResolveAlias(*exprType);
				if (!IsPrimitiveType(resolvedExprType) && !IsVectorType(resolvedExprType))
					throw AstError{ "incompatible type" };

				componentCount += GetComponentCount(resolvedExprType);
			}

			if (componentCount != requiredComponents)
				throw AstError{ "component count doesn't match required component count" };
		}

		node.cachedExpressionType = targetType;
		node.targetType = targetType;

		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(DeclareVariableStatement& node) -> ValidationResult
	{
		ExpressionType resolvedType;
		if (!node.varType.HasValue())
		{
			if (!node.initialExpression)
				throw AstError{ "variable must either have a type or an initial value" };

			const ExpressionType* initialExprType = GetExpressionType(*node.initialExpression);
			if (!initialExprType)
			{
				RegisterUnresolved(node.varName);
				return ValidationResult::Unresolved;
			}

			resolvedType = *initialExprType;
		}
		else
		{
			std::optional<ExpressionType> varType = ResolveTypeExpr(node.varType);
			if (!varType)
			{
				RegisterUnresolved(node.varName);
				return ValidationResult::Unresolved;
			}

			resolvedType = std::move(varType).value();
			if (node.initialExpression)
			{
				const ExpressionType* initialExprType = GetExpressionType(*node.initialExpression);
				if (!initialExprType)
				{
					RegisterUnresolved(node.varName);
					return ValidationResult::Unresolved;
				}

				TypeMustMatch(resolvedType, *initialExprType);
			}
		}

		node.varIndex = RegisterVariable(node.varName, resolvedType, node.varIndex);
		node.varType = std::move(resolvedType);

		if (m_context->options.makeVariableNameUnique)
		{
			// Since we are registered, FindIdentifier will find us
			auto IgnoreOurself = [varIndex = *node.varIndex](const IdentifierData& identifierData)
			{
				if (identifierData.category == IdentifierCategory::Variable && identifierData.index == varIndex)
					return false;

				return true;
			};

			if (FindIdentifier(node.varName, IgnoreOurself) != nullptr)
			{
				// Try to make variable name unique by appending _X to its name (incrementing X until it's unique) to the variable name until by incrementing X
				unsigned int cloneIndex = 2;
				std::string candidateName;
				do
				{
					candidateName = node.varName + "_" + std::to_string(cloneIndex++);
				}
				while (FindIdentifier(candidateName, IgnoreOurself) != nullptr);

				node.varName = std::move(candidateName);
			}
		}

		SanitizeIdentifier(node.varName);
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(IntrinsicExpression& node) -> ValidationResult
	{
		// Parameter validation
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			case IntrinsicType::Max:
			case IntrinsicType::Min:
			case IntrinsicType::Pow:
			case IntrinsicType::Reflect:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				const ExpressionType* firstParameterType = GetExpressionType(*node.parameters.front());
				if (!firstParameterType)
					return ValidationResult::Unresolved;

				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					const ExpressionType* parameterType = GetExpressionType(*node.parameters[i]);
					if (!parameterType)
						return ValidationResult::Unresolved;

					if (ResolveAlias(*firstParameterType) != ResolveAlias(*parameterType))
						throw AstError{ "All type must match" };
				}

				break;
			}

			case IntrinsicType::Exp:
			{
				if (node.parameters.size() != 1)
					throw AstError{ "Expected only one parameters" };

				MandatoryExpr(node.parameters.front());
				break;
			}

			case IntrinsicType::Length:
			case IntrinsicType::Normalize:
			{
				if (node.parameters.size() != 1)
					throw AstError{ "Expected only one parameters" };

				const ExpressionType* type = GetExpressionType(MandatoryExpr(node.parameters.front()));
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				if (!IsVectorType(resolvedType))
					throw AstError{ "Expected a vector" };

				break;
			}

			case IntrinsicType::SampleTexture:
			{
				if (node.parameters.size() != 2)
					throw AstError{ "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				const ExpressionType* firstParameterType = GetExpressionType(*node.parameters[0]);
				if (!firstParameterType)
					return ValidationResult::Unresolved;

				if (!IsSamplerType(*firstParameterType))
					throw AstError{ "First parameter must be a sampler" };

				const ExpressionType* secondParameterType = GetExpressionType(*node.parameters[1]);
				if (!secondParameterType)
					return ValidationResult::Unresolved;

				if (!IsVectorType(*secondParameterType))
					throw AstError{ "Second parameter must be a vector" };

				break;
			}
		}

		// Return type attribution
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				if (ResolveAlias(*type) != ExpressionType{ VectorType{ 3, PrimitiveType::Float32 } })
					throw AstError{ "CrossProduct only works with vec3[f32] expressions" };

				node.cachedExpressionType = *type;
				break;
			}

			case IntrinsicType::DotProduct:
			case IntrinsicType::Length:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				if (!IsVectorType(resolvedType))
					throw AstError{ "DotProduct expects vector types" }; //< FIXME

				node.cachedExpressionType = std::get<VectorType>(resolvedType).type;
				break;
			}

			case IntrinsicType::Normalize:
			case IntrinsicType::Reflect:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				if (!IsVectorType(resolvedType))
					throw AstError{ "DotProduct expects vector types" }; //< FIXME

				node.cachedExpressionType = *type;
				break;
			}

			case IntrinsicType::Max:
			case IntrinsicType::Min:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				if (!IsPrimitiveType(resolvedType) && !IsVectorType(resolvedType))
					throw AstError{ "max and min only work with primitive and vector types" };

				if ((IsPrimitiveType(resolvedType) && std::get<PrimitiveType>(resolvedType) == PrimitiveType::Boolean) ||
				    (IsVectorType(resolvedType) && std::get<VectorType>(resolvedType).type == PrimitiveType::Boolean))
					throw AstError{ "max and min do not work with booleans" };

				node.cachedExpressionType = *type;
				break;
			}

			case IntrinsicType::Exp:
			case IntrinsicType::Pow:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				if (!IsPrimitiveType(resolvedType) && !IsVectorType(resolvedType))
					throw AstError{ "pow only works with primitive and vector types" };

				if ((IsPrimitiveType(resolvedType) && std::get<PrimitiveType>(resolvedType) != PrimitiveType::Float32) ||
				    (IsVectorType(resolvedType) && std::get<VectorType>(resolvedType).type != PrimitiveType::Float32))
					throw AstError{ "pow only works with floating-point primitive or vectors" };

				node.cachedExpressionType = *type;
				break;
			}

			case IntrinsicType::SampleTexture:
			{
				const ExpressionType* type = GetExpressionType(*node.parameters.front());
				if (!type)
					return ValidationResult::Unresolved;

				const ExpressionType& resolvedType = ResolveAlias(*type);
				node.cachedExpressionType = VectorType{ 4, std::get<SamplerType>(resolvedType).sampledType };
				break;
			}
		}

		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(SwizzleExpression& node) -> ValidationResult
	{
		MandatoryExpr(node.expression);
		const ExpressionType* exprType = GetExpressionType(*node.expression);
		if (!exprType)
			return ValidationResult::Unresolved;

		const ExpressionType& resolvedExprType = ResolveAlias(*exprType);

		if (!IsPrimitiveType(resolvedExprType) && !IsVectorType(resolvedExprType))
			throw AstError{ "Cannot swizzle this type" };

		PrimitiveType baseType;
		std::size_t componentCount;
		if (IsPrimitiveType(resolvedExprType))
		{
			if (m_context->options.removeScalarSwizzling)
				throw AstError{ "internal error" }; //< scalar swizzling should have been removed by then

			baseType = std::get<PrimitiveType>(resolvedExprType);
			componentCount = 1;
		}
		else
		{
			const VectorType& vecType = std::get<VectorType>(resolvedExprType);
			baseType = vecType.type;
			componentCount = vecType.componentCount;
		}

		if (node.componentCount > 4)
			throw AstError{ "cannot swizzle more than four elements" };

		for (std::size_t i = 0; i < node.componentCount; ++i)
		{
			if (node.components[i] >= componentCount)
				throw AstError{ "invalid swizzle" };
		}

		if (node.componentCount > 1)
		{
			node.cachedExpressionType = VectorType{
				node.componentCount,
				baseType
			};
		}
		else
			node.cachedExpressionType = baseType;

		return ValidationResult::Validated;
	}
	
	auto SanitizeVisitor::Validate(UnaryExpression& node) -> ValidationResult
	{
		const ExpressionType* exprType = GetExpressionType(MandatoryExpr(node.expression));
		if (!exprType)
			return ValidationResult::Unresolved;

		const ExpressionType& resolvedExprType = ResolveAlias(*exprType);

		switch (node.op)
		{
			case UnaryType::LogicalNot:
			{
				if (resolvedExprType != ExpressionType(PrimitiveType::Boolean))
					throw AstError{ "logical not is only supported on booleans" };

				break;
			}

			case UnaryType::Minus:
			case UnaryType::Plus:
			{
				PrimitiveType basicType;
				if (IsPrimitiveType(resolvedExprType))
					basicType = std::get<PrimitiveType>(resolvedExprType);
				else if (IsVectorType(resolvedExprType))
					basicType = std::get<VectorType>(resolvedExprType).type;
				else
					throw AstError{ "plus and minus unary expressions are only supported on primitive/vectors types" };

				if (basicType != PrimitiveType::Float32 && basicType != PrimitiveType::Int32 && basicType != PrimitiveType::UInt32)
					throw AstError{ "plus and minus unary expressions are only supported on floating points and integers types" };

				break;
			}
		}

		node.cachedExpressionType = *exprType;
		return ValidationResult::Validated;
	}

	auto SanitizeVisitor::Validate(VariableValueExpression& node) -> ValidationResult
	{
		node.cachedExpressionType = m_context->variableTypes.Retrieve(node.variableId);
		return ValidationResult::Validated;
	}

	ExpressionType SanitizeVisitor::ValidateBinaryOp(BinaryType op, const ExpressionType& leftExprType, const ExpressionType& rightExprType)
	{
		if (!IsPrimitiveType(leftExprType) && !IsMatrixType(leftExprType) && !IsVectorType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		if (!IsPrimitiveType(rightExprType) && !IsMatrixType(rightExprType) && !IsVectorType(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		if (IsPrimitiveType(leftExprType))
		{
			PrimitiveType leftType = std::get<PrimitiveType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
					if (leftType == PrimitiveType::Boolean)
						throw AstError{ "this operation is not supported for booleans" };

					[[fallthrough]];
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				{
					TypeMustMatch(leftExprType, rightExprType);
					return PrimitiveType::Boolean;
				}

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExprType, rightExprType);
					return leftExprType;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					switch (leftType)
					{
						case PrimitiveType::Float32:
						case PrimitiveType::Int32:
						case PrimitiveType::UInt32:
						{
							if (IsMatrixType(rightExprType))
							{
								TypeMustMatch(leftType, std::get<MatrixType>(rightExprType).type);
								return rightExprType;
							}
							else if (IsPrimitiveType(rightExprType))
							{
								TypeMustMatch(leftType, rightExprType);
								return leftExprType;
							}
							else if (IsVectorType(rightExprType))
							{
								TypeMustMatch(leftType, std::get<VectorType>(rightExprType).type);
								return rightExprType;
							}
							else
								throw AstError{ "incompatible types" };

							break;
						}

						case PrimitiveType::Boolean:
							throw AstError{ "this operation is not supported for booleans" };

						default:
							throw AstError{ "incompatible types" };
					}
				}

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
				{
					if (leftType != PrimitiveType::Boolean)
						throw AstError{ "logical and/or are only supported on booleans" };

					TypeMustMatch(leftExprType, rightExprType);
					return PrimitiveType::Boolean;
				}
			}
		}
		else if (IsMatrixType(leftExprType))
		{
			const MatrixType& leftType = std::get<MatrixType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(leftExprType, rightExprType);
					return PrimitiveType::Boolean;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExprType, rightExprType);
					return leftExprType;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsMatrixType(rightExprType))
					{
						TypeMustMatch(leftExprType, rightExprType);
						return leftExprType; //< FIXME
					}
					else if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						return leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						const VectorType& rightType = std::get<VectorType>(rightExprType);
						TypeMustMatch(leftType.type, rightType.type);

						if (leftType.columnCount != rightType.componentCount)
							throw AstError{ "incompatible types" };

						return rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
					throw AstError{ "logical and/or are only supported on booleans" };
			}
		}
		else if (IsVectorType(leftExprType))
		{
			const VectorType& leftType = std::get<VectorType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(leftExprType, rightExprType);
					return PrimitiveType::Boolean;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExprType, rightExprType);
					return leftExprType;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						return leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						TypeMustMatch(leftType, rightExprType);
						return rightExprType;
					}
					else
						throw AstError{ "incompatible types" };

					break;
				}

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
					throw AstError{ "logical and/or are only supported on booleans" };
			}
		}

		throw AstError{ "internal error: unchecked operation" };
	}
}
