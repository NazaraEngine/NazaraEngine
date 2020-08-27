// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONSTORE_HPP
#define NAZARA_SPIRVEXPRESSIONSTORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <Nazara/Shader/ShaderVarVisitorExcept.hpp>
#include <Nazara/Shader/SpirvData.hpp>

namespace Nz
{
	class SpirvSection;
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionStore : public ShaderAstVisitorExcept, public ShaderVarVisitorExcept
	{
		public:
			inline SpirvExpressionStore(SpirvWriter& writer);
			SpirvExpressionStore(const SpirvExpressionStore&) = delete;
			SpirvExpressionStore(SpirvExpressionStore&&) = delete;
			~SpirvExpressionStore() = default;

			void Store(const ShaderNodes::ExpressionPtr& node, UInt32 resultId);

			using ShaderAstVisitorExcept::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			using ShaderVarVisitorExcept::Visit;
			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::OutputVariable& var) override;

			SpirvExpressionStore& operator=(const SpirvExpressionStore&) = delete;
			SpirvExpressionStore& operator=(SpirvExpressionStore&&) = delete;

		private:
			struct LocalVar
			{
				std::string varName;
			};

			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 resultId;
			};

			SpirvWriter& m_writer;
			std::variant<std::monostate, LocalVar, Pointer> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionStore.inl>

#endif
