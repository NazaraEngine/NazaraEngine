// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_REFLECT_HPP
#define NAZARA_SHADER_AST_REFLECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
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
				std::function<void(const std::string& optionName, const ExpressionType& optionType)> onOptionDeclaration;
			};

		private:
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareOptionStatement& node) override;

			const Callbacks* m_callbacks;
	};
}

#include <Nazara/Shader/Ast/AstReflect.inl>

#endif
