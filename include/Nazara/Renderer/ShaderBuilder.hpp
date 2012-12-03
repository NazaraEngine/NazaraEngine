// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERBUILDER_HPP
#define NAZARA_SHADERBUILDER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>

class NAZARA_API NzShaderBuilder
{
	public:
		NzShaderBuilder() = default;
		NzShaderBuilder(nzShaderLanguage language);
		~NzShaderBuilder();

		bool Create(nzShaderLanguage language);
		void Destroy();

		bool IsValid();

		void SetDeferred(bool deferredRendering);
		void SetDiffuseMapping(bool diffuseMapping);
		void SetInstancing(bool instancing);
		void SetLighting(bool lighting);
		void SetNormalMapping(bool normalMapping);
		void SetParallaxMapping(bool parallaxMapping);

	private:
		NzShaderImpl* m_impl = nullptr;
};

#endif // NAZARA_SHADER_HPP
