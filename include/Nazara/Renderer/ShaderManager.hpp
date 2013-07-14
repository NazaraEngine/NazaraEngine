// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERMANAGER_HPP
#define NAZARA_SHADERMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderManagerParams.hpp>

class NAZARA_API NzShaderManager
{
	friend class NzRenderer;

	public:
		NzShaderManager() = delete;
		~NzShaderManager() = delete;

		static const NzShader* Get(const NzShaderManagerParams& params);

	private:
		static NzString BuildFragmentCode(const NzShaderManagerParams& params);
		static NzString BuildVertexCode(const NzShaderManagerParams& params);
		static NzShader* GenerateShader(const NzShaderManagerParams& params);

		static bool Initialize();
		static void Uninitialize();
};

#endif // NAZARA_SHADERMANAGER_HPP
