// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDFORWARDPASS_HPP
#define NAZARA_DEFERREDFORWARDPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>

namespace Nz
{
	class ForwardRenderTechnique;

	class NAZARA_GRAPHICS_API DeferredForwardPass : public DeferredRenderPass
	{
		public:
			DeferredForwardPass();
			virtual ~DeferredForwardPass();

			void Initialize(DeferredRenderTechnique* technique);
			bool Process(const SceneData& sceneData, unsigned int workTexture, unsigned int sceneTexture) const;

		protected:
			const ForwardRenderTechnique* m_forwardTechnique;
	};
}

#endif // NAZARA_DEFERREDFORWARDPASS_HPP
