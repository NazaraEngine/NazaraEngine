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
#include <Nazara/Renderer/ShaderAstVisitor.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nz
{
	class NAZARA_RENDERER_API SpirvWriter : public ShaderAstVisitor
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
			struct Raw;
			struct WordCount;

			struct Section
			{
				inline std::size_t Append(const char* str);
				inline std::size_t Append(const std::string_view& str);
				inline std::size_t Append(const std::string& str);
				inline std::size_t Append(UInt32 value);
				std::size_t Append(const Opcode& opcode, const WordCount& wordCount);
				std::size_t Append(const Raw& raw);
				inline std::size_t Append(std::initializer_list<UInt32> codepoints);
				template<typename... Args> std::size_t Append(Opcode opcode, const Args&... args);
				template<typename T> std::size_t Append(T value);

				inline unsigned int CountWord(const char* str);
				inline unsigned int CountWord(const std::string_view& str);
				inline unsigned int CountWord(const std::string& str);
				unsigned int CountWord(const Raw& raw);
				template<typename T> unsigned int CountWord(const T& value);
				template<typename T1, typename T2, typename... Args> unsigned int CountWord(const T1& value, const T2& value2, const Args&... rest);

				inline std::size_t GetOutputOffset() const;

				std::vector<UInt32> data;
			};

			UInt32 AllocateResultId();

			void AppendConstants();
			void AppendHeader();
			void AppendStructType(std::size_t structIndex, UInt32 resultId);
			void AppendTypes();

			UInt32 GetConstantId(const ShaderNodes::Constant::Variant& value) const;
			UInt32 GetTypeId(const ShaderExpressionType& type) const;

			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			UInt32 RegisterType(ShaderExpressionType type);

			using ShaderAstVisitor::Visit;
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

			static void MergeBlocks(std::vector<UInt32>& output, const Section& from);

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
