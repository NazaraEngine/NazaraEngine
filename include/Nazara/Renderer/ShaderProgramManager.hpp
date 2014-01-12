// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERPROGRAMMANAGER_HPP
#define NAZARA_SHADERPROGRAMMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/ShaderProgramManagerParams.hpp>

class NAZARA_API NzShaderProgramManager
{
	friend class NzRenderer;

	public:
		NzShaderProgramManager() = delete;
		~NzShaderProgramManager() = delete;

		static const NzShaderProgram* Get(const NzShaderProgramManagerParams& params);

	private:
		static NzString BuildFragmentCode(const NzShaderProgramManagerParams& params);
		static NzString BuildVertexCode(const NzShaderProgramManagerParams& params);
		static NzShaderProgram* GenerateProgram(const NzShaderProgramManagerParams& params);

		static bool Initialize();
		static void Uninitialize();
};

#endif // NAZARA_SHADERPROGRAMMANAGER_HPP
