// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTECHNIQUES_HPP
#define NAZARA_RENDERTECHNIQUES_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/Enums.hpp>

class NzAbstractRenderTechnique;

class NAZARA_API NzRenderTechniques
{
	public:
		using RenderTechniqueFactory = NzAbstractRenderTechnique* (*)();

		NzRenderTechniques() = delete;
		~NzRenderTechniques() = delete;

		static NzAbstractRenderTechnique* GetByEnum(nzRenderTechniqueType renderTechnique, int* techniqueRanking = nullptr);
		static NzAbstractRenderTechnique* GetByIndex(unsigned int index, int* techniqueRanking = nullptr);
		static NzAbstractRenderTechnique* GetByName(const NzString& name, int* techniqueRanking = nullptr);
		static NzAbstractRenderTechnique* GetByRanking(int maxRanking, int* techniqueRanking = nullptr);

		static unsigned int GetCount();

		static void Register(const NzString& name, int ranking, RenderTechniqueFactory factory);

		static NzString ToString(nzRenderTechniqueType renderTechnique);

		static void Unregister(const NzString& name);
};

#endif // NAZARA_RENDERTECHNIQUES_HPP
