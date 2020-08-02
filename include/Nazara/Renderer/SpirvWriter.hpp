// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVWRITER_HPP
#define NAZARA_SPIRVWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <Nazara/Renderer/ShaderVisitor.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nz
{
	class NAZARA_RENDERER_API SpirvWriter : public ShaderVisitor
	{
		public:
			struct Environment;

			SpirvWriter();
			SpirvWriter(const SpirvWriter&) = delete;
			SpirvWriter(SpirvWriter&&) = delete;
			~SpirvWriter() = default;

			std::vector<UInt32> Generate(const ShaderAst& shader);

			void SetEnv(Environment environment);

			struct Environment
			{
				UInt32 spvMajorVersion = 1;
				UInt32 spvMinorVersion = 0;
			};

		private:
			struct Opcode;

			inline std::size_t Append(const char* str);
			inline std::size_t Append(const std::string_view& str);
			inline std::size_t Append(const std::string& str);
			std::size_t Append(UInt32 value);
			std::size_t Append(const Opcode& opcode, unsigned int wordCount);
			inline std::size_t Append(std::initializer_list<UInt32> codepoints);
			template<typename... Args> std::size_t Append(Opcode opcode, const Args&... args);
			template<typename T> std::size_t Append(T value);

			UInt32 AllocateResultId();

			void AppendHeader();
			void AppendTypes();

			inline unsigned int CountWord(const char* str);
			unsigned int CountWord(const std::string_view& str);
			inline unsigned int CountWord(const std::string& str);
			template<typename T> unsigned int CountWord(const T& value);
			template<typename T1, typename T2, typename... Args> unsigned int CountWord(const T1& value, const T2& value2, const Args&... rest);

			std::size_t GetOutputOffset() const;

			UInt32 ProcessType(ShaderExpressionType type);

			using ShaderVisitor::Visit;
			void Visit(const ShaderNodes::ExpressionPtr& expr, bool encloseIfRequired = false);
			void Visit(const ShaderNodes::AccessMember& node) override;
			void Visit(const ShaderNodes::AssignOp& node) override;
			void Visit(const ShaderNodes::Branch& node) override;
			void Visit(const ShaderNodes::BinaryOp& node) override;
			void Visit(const ShaderNodes::Cast& node) override;
			void Visit(const ShaderNodes::Constant& node) override;
			void Visit(const ShaderNodes::DeclareVariable& node) override;
			void Visit(const ShaderNodes::ExpressionStatement& node) override;
			void Visit(const ShaderNodes::Identifier& node) override;
			void Visit(const ShaderNodes::IntrinsicCall& node) override;
			void Visit(const ShaderNodes::Sample2D& node) override;
			void Visit(const ShaderNodes::StatementBlock& node) override;
			void Visit(const ShaderNodes::SwizzleOp& node) override;

			static void MergeBlocks(std::vector<UInt32>& output, const std::vector<UInt32>& from);

			struct Context
			{
				const ShaderAst* shader = nullptr;
				const ShaderAst::Function* currentFunction = nullptr;
			};

			struct State;

			Context m_context;
			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Renderer/SpirvWriter.inl>

#endif
