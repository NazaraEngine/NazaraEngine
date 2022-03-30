// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTREFLECT_HPP
#define NAZARA_SHADER_AST_ASTREFLECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangSourceLocation.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstReflect : public AstRecursiveVisitor
	{
		public:
			struct Callbacks;

			AstReflect() = default;
			AstReflect(const AstReflect&) = delete;
			AstReflect(AstReflect&&) = delete;
			~AstReflect() = default;

			void Reflect(Statement& statement, const Callbacks& callbacks);

			AstReflect& operator=(const AstReflect&) = delete;
			AstReflect& operator=(AstReflect&&) = delete;

			struct Callbacks
			{
				std::function<void(ShaderStageType stageType, const std::string& functionName)> onEntryPointDeclaration;

				std::function<void(const DeclareAliasStatement& aliasDecl)> onAliasDeclaration;
				std::function<void(const DeclareConstStatement& constDecl)> onConstDeclaration;
				std::function<void(const DeclareExternalStatement& extDecl)> onExternalDeclaration;
				std::function<void(const DeclareFunctionStatement& funcDecl)> onFunctionDeclaration;
				std::function<void(const DeclareOptionStatement& optionDecl)> onOptionDeclaration;
				std::function<void(const DeclareStructStatement& structDecl)> onStructDeclaration;
				std::function<void(const DeclareVariableStatement& variableDecl)> onVariableDeclaration;

				std::function<void(const std::string& name, std::size_t aliasIndex, const ShaderLang::SourceLocation& sourceLocation)> onAliasIndex;
				std::function<void(const std::string& name, std::size_t constIndex, const ShaderLang::SourceLocation& sourceLocation)> onConstIndex;
				std::function<void(const std::string& name, std::size_t funcIndex, const ShaderLang::SourceLocation& sourceLocation)> onFunctionIndex;
				std::function<void(const std::string& name, std::size_t optIndex, const ShaderLang::SourceLocation& sourceLocation)> onOptionIndex;
				std::function<void(const std::string& name, std::size_t structIndex, const ShaderLang::SourceLocation& sourceLocation)> onStructIndex;
				std::function<void(const std::string& name, std::size_t varIndex, const ShaderLang::SourceLocation& sourceLocation)> onVariableIndex;
			};

		private:
			void Visit(DeclareAliasStatement& node) override;
			void Visit(DeclareConstStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareOptionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(ForStatement& node) override;
			void Visit(ForEachStatement& node) override;

			const Callbacks* m_callbacks;
	};
}

#include <Nazara/Shader/Ast/AstReflect.inl>

#endif // NAZARA_SHADER_AST_ASTREFLECT_HPP
