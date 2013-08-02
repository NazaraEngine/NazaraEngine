// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERPROGRAMMANAGERPARAMS_HPP
#define NAZARA_SHADERPROGRAMMANAGERPARAMS_HPP

#include <Nazara/Renderer/Enums.hpp>

struct NzShaderProgramManagerParams
{
	struct FullscreenQuad
	{
		bool alphaMapping;
		bool alphaTest;
		bool diffuseMapping;
	};

	struct Model
	{
		bool alphaMapping;
		bool alphaTest;
		bool diffuseMapping;
		bool emissiveMapping;
		bool lighting;
		bool normalMapping;
		bool parallaxMapping;
		bool specularMapping;
	};

	nzShaderTarget target;
	nzUInt32 flags;

	union
	{
		FullscreenQuad fullscreenQuad;
		Model model;
	};
};

#endif // NAZARA_SHADERPROGRAMMANAGERPARAMS_HPP
