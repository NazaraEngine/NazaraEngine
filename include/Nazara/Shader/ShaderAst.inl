// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline ShaderAst::ShaderAst(ShaderStageType shaderStage) :
	m_stage(shaderStage)
	{
	}

	inline std::size_t ShaderAst::FindConditionByName(const std::string_view& conditionName) const
	{
		for (std::size_t i = 0; i < m_conditions.size(); ++i)
		{
			if (m_conditions[i].name == conditionName)
				return i;
		}

		return InvalidCondition;
	}

	inline auto Nz::ShaderAst::GetCondition(std::size_t i) const -> const Condition&
	{
		assert(i < m_functions.size());
		return m_conditions[i];
	}

	inline std::size_t ShaderAst::GetConditionCount() const
	{
		return m_conditions.size();
	}

	inline auto ShaderAst::GetConditions() const -> const std::vector<Condition>&
	{
		return m_conditions;
	}

	inline auto ShaderAst::GetFunction(std::size_t i) const -> const Function&
	{
		assert(i < m_functions.size());
		return m_functions[i];
	}

	inline std::size_t ShaderAst::GetFunctionCount() const
	{
		return m_functions.size();
	}

	inline auto ShaderAst::GetFunctions() const -> const std::vector<Function>&
	{
		return m_functions;
	}

	inline auto ShaderAst::GetInput(std::size_t i) const -> const InputOutput&
	{
		assert(i < m_inputs.size());
		return m_inputs[i];
	}

	inline std::size_t ShaderAst::GetInputCount() const
	{
		return m_inputs.size();
	}

	inline auto ShaderAst::GetInputs() const -> const std::vector<InputOutput>&
	{
		return m_inputs;
	}

	inline auto ShaderAst::GetOutput(std::size_t i) const -> const InputOutput&
	{
		assert(i < m_outputs.size());
		return m_outputs[i];
	}

	inline std::size_t ShaderAst::GetOutputCount() const
	{
		return m_outputs.size();
	}

	inline auto ShaderAst::GetOutputs() const -> const std::vector<InputOutput>&
	{
		return m_outputs;
	}

	inline ShaderStageType ShaderAst::GetStage() const
	{
		return m_stage;
	}

	inline auto ShaderAst::GetStruct(std::size_t i) const -> const Struct&
	{
		assert(i < m_structs.size());
		return m_structs[i];
	}

	inline std::size_t ShaderAst::GetStructCount() const
	{
		return m_structs.size();
	}

	inline auto ShaderAst::GetStructs() const -> const std::vector<Struct>&
	{
		return m_structs;
	}

	inline auto ShaderAst::GetUniform(std::size_t i) const -> const Uniform&
	{
		assert(i < m_uniforms.size());
		return m_uniforms[i];
	}

	inline std::size_t ShaderAst::GetUniformCount() const
	{
		return m_uniforms.size();
	}

	inline auto ShaderAst::GetUniforms() const -> const std::vector<Uniform>&
	{
		return m_uniforms;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
