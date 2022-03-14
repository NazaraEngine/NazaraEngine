// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTEXPORTVISITOR_HPP
#define NAZARA_SHADER_AST_ASTEXPORTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstExportVisitor : public AstRecursiveVisitor
	{
		public:
			struct Callbacks;

			AstExportVisitor() = default;
			AstExportVisitor(const AstExportVisitor&) = delete;
			AstExportVisitor(AstExportVisitor&&) = delete;
			~AstExportVisitor() = default;

			void Visit(Statement& statement, const Callbacks& callbacks);

			AstExportVisitor& operator=(const AstExportVisitor&) = delete;
			AstExportVisitor& operator=(AstExportVisitor&&) = delete;

			struct Callbacks
			{
				std::function<void(DeclareFunctionStatement& funcNode)> onExportedFunc;
				std::function<void(DeclareStructStatement& structNode)> onExportedStruct;
			};

		private:
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;

			const Callbacks* m_callbacks;
	};
}

#include <Nazara/Shader/Ast/AstExportVisitor.inl>

#endif // NAZARA_SHADER_AST_ASTEXPORTVISITOR_HPP
