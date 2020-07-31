// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/ShaderValidator.hpp>
#include <SpirV/spirv.h>
#include <SpirV/GLSL.std.450.h>
#include <cassert>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	struct SpirvWriter::Opcode
	{
		SpvOp op;
	};

	SpirvWriter::SpirvWriter() :
	m_currentState(nullptr)
	{
	}

	std::vector<UInt32> SpirvWriter::Generate(const ShaderAst& shader)
	{
		std::string error;
		if (!ValidateShader(shader, &error))
			throw std::runtime_error("Invalid shader AST: " + error);

		m_context.shader = &shader;

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		AppendHeader();

		std::vector<UInt32> ret = std::move(state.output);
		return ret;
	}

	void SpirvWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	void SpirvWriter::Append(const std::string_view& str)
	{
		std::size_t size4 = CountWord(str);
		for (std::size_t i = 0; i < size4; ++i)
		{
			UInt32 codepoint = 0;
			for (std::size_t j = 0; j < 4; ++j)
			{
				std::size_t pos = i * 4 + j;
				if (pos < str.size())
					codepoint |= UInt32(str[pos]) << (j * 8);
			}

			Append(codepoint);
		}
	}

	void SpirvWriter::Append(const Opcode& opcode, unsigned int wordCount)
	{
		Append(UInt32(opcode.op) | UInt32(wordCount) << 16);
	}

	void SpirvWriter::Append(UInt32 codepoint)
	{
		assert(m_currentState);
		m_currentState->output.push_back(codepoint);
	}

	void SpirvWriter::Append(std::initializer_list<UInt32> codepoints)
	{
		for (UInt32 cp : codepoints)
			Append(cp);
	}

	void SpirvWriter::AppendHeader()
	{
		Append(SpvMagicNumber); //< Spir-V magic number
		Append(0x00010000); //< Spir-V version number (1.0 for compatibility)
		Append(0); //< Generator magic number (TODO: Register generator to Khronos)
		Append(1); //< Bound (ID count)
		Append(0); //< Instruction schema (required to be 0 for now)
		Append(Opcode{ SpvOpCapability }, SpvCapabilityShader);
		Append(Opcode{ SpvOpExtInstImport }, 1, "GLSL.std.450");
		Append(Opcode{ SpvOpMemoryModel }, SpvAddressingModelLogical, SpvMemoryModelGLSL450);

		assert(m_context.shader);
		switch (m_context.shader->GetStage())
		{
			case ShaderStageType::Fragment:
				break;
			case ShaderStageType::Vertex:
				break;

			default:
				break;
		}
	}

	void SpirvWriter::Visit(const ShaderNodes::ExpressionPtr& expr, bool encloseIfRequired)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::AccessMember& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::AssignOp& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Branch& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::BinaryOp& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::BuiltinVariable& var)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Cast& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Constant& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::DeclareVariable& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::ExpressionStatement& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Identifier& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::InputVariable& var)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::IntrinsicCall& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::LocalVariable& var)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::ParameterVariable& var)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::OutputVariable& var)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Sample2D& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::StatementBlock& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::SwizzleOp& node)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::UniformVariable& var)
	{
	}
}
