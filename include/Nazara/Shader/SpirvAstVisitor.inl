// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvAstVisitor::SpirvAstVisitor(SpirvWriter& writer, SpirvSection& instructions, std::vector<FuncData>& funcData) :
	m_extVarIndex(0),
	m_funcIndex(0),
	m_funcData(funcData),
	m_currentBlock(nullptr),
	m_instructions(instructions),
	m_writer(writer)
	{
	}

	void SpirvAstVisitor::RegisterExternalVariable(std::size_t varIndex, const ShaderAst::ExpressionType& type)
	{
		UInt32 pointerId = m_writer.GetExtVarPointerId(varIndex);
		SpirvStorageClass storageClass = (IsSamplerType(type)) ? SpirvStorageClass::UniformConstant : SpirvStorageClass::Uniform;

		RegisterVariable(varIndex, m_writer.GetTypeId(type), pointerId, storageClass);
	}

	inline void SpirvAstVisitor::RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* structDesc)
	{
		if (structIndex >= m_structs.size())
			m_structs.resize(structIndex + 1);

		m_structs[structIndex] = structDesc;
	}

	inline void SpirvAstVisitor::RegisterVariable(std::size_t varIndex, UInt32 typeId, UInt32 pointerId, SpirvStorageClass storageClass)
	{
		if (varIndex >= m_variables.size())
			m_variables.resize(varIndex + 1);

		m_variables[varIndex] = Variable{
			storageClass,
			pointerId,
			typeId
		};
	}
}

#include <Nazara/Shader/DebugOff.hpp>
