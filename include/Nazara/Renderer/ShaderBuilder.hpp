// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERBUILDER_HPP
#define NAZARA_SHADERBUILDER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>

class NAZARA_API NzShaderBuilder
{
	friend class NzRenderer;

	public:
		NzShaderBuilder() = delete;
		~NzShaderBuilder() = delete;

		static const NzShader* Get(nzUInt32 flags);

	private:
		static bool Initialize();
		static void Uninitialize();
};

#endif // NAZARA_SHADERBUILDER_HPP
