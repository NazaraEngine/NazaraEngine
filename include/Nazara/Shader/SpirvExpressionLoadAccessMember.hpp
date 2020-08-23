// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONLOADACCESSMEMBER_HPP
#define NAZARA_SPIRVEXPRESSIONLOADACCESSMEMBER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <Nazara/Shader/ShaderVarVisitorExcept.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <vector>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionLoadAccessMember : public ShaderAstVisitorExcept, public ShaderVarVisitorExcept
	{
		public:
			inline SpirvExpressionLoadAccessMember(SpirvWriter& writer);
			SpirvExpressionLoadAccessMember(const SpirvExpressionLoadAccessMember&) = delete;
			SpirvExpressionLoadAccessMember(SpirvExpressionLoadAccessMember&&) = delete;
			~SpirvExpressionLoadAccessMember() = default;

			UInt32 EvaluateExpression(ShaderNodes::AccessMember& expr);

			using ShaderAstVisitor::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::Identifier& node) override;

			using ShaderVarVisitor::Visit;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			SpirvExpressionLoadAccessMember& operator=(const SpirvExpressionLoadAccessMember&) = delete;
			SpirvExpressionLoadAccessMember& operator=(SpirvExpressionLoadAccessMember&&) = delete;

		private:
			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 resultId;
				UInt32 pointedTypeId;
			};

			struct Value
			{
				UInt32 resultId;
			};

			SpirvWriter& m_writer;
			std::variant<std::monostate, Pointer, Value> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionLoadAccessMember.inl>

#endif
