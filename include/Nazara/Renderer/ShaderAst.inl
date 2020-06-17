// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
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

#include <Nazara/Renderer/DebugOff.hpp>
