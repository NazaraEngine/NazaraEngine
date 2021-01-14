// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTOPTIMISER_HPP
#define NAZARA_SHADERASTOPTIMISER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <vector>

namespace Nz
{
	class ShaderAst;

	class NAZARA_SHADER_API ShaderAstOptimizer : public ShaderAstCloner
	{
		public:
			ShaderAstOptimizer() = default;
			ShaderAstOptimizer(const ShaderAstOptimizer&) = delete;
			ShaderAstOptimizer(ShaderAstOptimizer&&) = delete;
			~ShaderAstOptimizer() = default;

			ShaderNodes::StatementPtr Optimise(const ShaderNodes::StatementPtr& statement);
			ShaderNodes::StatementPtr Optimise(const ShaderNodes::StatementPtr& statement, const ShaderAst& shader, UInt64 enabledConditions);

			ShaderAstOptimizer& operator=(const ShaderAstOptimizer&) = delete;
			ShaderAstOptimizer& operator=(ShaderAstOptimizer&&) = delete;

		protected:
			using ShaderAstCloner::Visit;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::ConditionalExpression& node) override;
			void Visit(ShaderNodes::ConditionalStatement& node) override;

			template<ShaderNodes::BinaryType Type> void PropagateConstant(const std::shared_ptr<ShaderNodes::Constant>& lhs, const std::shared_ptr<ShaderNodes::Constant>& rhs);

		private:
			const ShaderAst* m_shaderAst;
			UInt64 m_enabledConditions;
	};
}

#include <Nazara/Shader/ShaderAstOptimizer.inl>

#endif
