// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONLOAD_HPP
#define NAZARA_SPIRVEXPRESSIONLOAD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <Nazara/Shader/ShaderVarVisitorExcept.hpp>
#include <vector>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionLoad : public ShaderAstVisitorExcept, public ShaderVarVisitorExcept
	{
		public:
			inline SpirvExpressionLoad(SpirvWriter& writer);
			SpirvExpressionLoad(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad(SpirvExpressionLoad&&) = delete;
			~SpirvExpressionLoad() = default;

			UInt32 EvaluateExpression(const ShaderNodes::ExpressionPtr& expr);

			using ShaderAstVisitorExcept::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			using ShaderVarVisitorExcept::Visit;
			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::ParameterVariable& var) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			SpirvExpressionLoad& operator=(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad& operator=(SpirvExpressionLoad&&) = delete;

		private:
			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			std::vector<UInt32> m_resultIds;
			SpirvWriter& m_writer;
	};
}

#include <Nazara/Shader/SpirvExpressionLoad.inl>

#endif
