// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTECHNIQUES_HPP
#define NAZARA_RENDERTECHNIQUES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>

namespace Nz
{
	class AbstractRenderTechnique;

	class NAZARA_GRAPHICS_API RenderTechniques
	{
		public:
			using RenderTechniqueFactory = AbstractRenderTechnique* (*)();

			RenderTechniques() = delete;
			~RenderTechniques() = delete;

			static AbstractRenderTechnique* GetByEnum(RenderTechniqueType renderTechnique, int* techniqueRanking = nullptr);
			static AbstractRenderTechnique* GetByIndex(unsigned int index, int* techniqueRanking = nullptr);
			static AbstractRenderTechnique* GetByName(const String& name, int* techniqueRanking = nullptr);
			static AbstractRenderTechnique* GetByRanking(int maxRanking, int* techniqueRanking = nullptr);

			static std::size_t GetCount();

			static void Register(const String& name, int ranking, RenderTechniqueFactory factory);

			static String ToString(RenderTechniqueType renderTechnique);

			static void Unregister(const String& name);
	};
}

#endif // NAZARA_RENDERTECHNIQUES_HPP
