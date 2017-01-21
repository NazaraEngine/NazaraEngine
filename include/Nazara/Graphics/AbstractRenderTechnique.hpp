// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTRENDERTECHNIQUE_HPP
#define NAZARA_ABSTRACTRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/SceneData.hpp>

namespace Nz
{
	class AbstractViewer;
	class Background;
	struct SceneData;

	class NAZARA_GRAPHICS_API AbstractRenderTechnique
	{
		public:
			AbstractRenderTechnique();
			AbstractRenderTechnique(const AbstractRenderTechnique&) = delete;
			AbstractRenderTechnique(AbstractRenderTechnique&&) = default;
			virtual ~AbstractRenderTechnique();

			virtual void Clear(const SceneData& sceneData) const = 0;
			virtual bool Draw(const SceneData& sceneData) const = 0;

			virtual void EnableInstancing(bool instancing);

			virtual String GetName() const;
			virtual AbstractRenderQueue* GetRenderQueue() = 0;
			virtual RenderTechniqueType GetType() const = 0;

			virtual bool IsInstancingEnabled() const;

			AbstractRenderTechnique& operator=(const AbstractRenderTechnique&) = delete;
			AbstractRenderTechnique& operator=(AbstractRenderTechnique&&) = default;

		protected:
			bool m_instancingEnabled;
	};
}

#endif // NAZARA_ABSTRACTRENDERTECHNIQUE_HPP
