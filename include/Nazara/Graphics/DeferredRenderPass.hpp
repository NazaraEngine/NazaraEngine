// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERPASS_HPP
#define NAZARA_DEFERREDRENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	class DeferredRenderTechnique;
	class DeferredRenderQueue;
	struct SceneData;
	class RenderTexture;
	class Texture;

	class NAZARA_GRAPHICS_API DeferredRenderPass
	{
		friend DeferredRenderTechnique;

		public:
			DeferredRenderPass();
			DeferredRenderPass(const DeferredRenderPass&) = delete;
			virtual ~DeferredRenderPass();

			void Enable(bool enable);

			virtual void Initialize(DeferredRenderTechnique* technique);

			bool IsEnabled() const;

			virtual bool Process(const SceneData& sceneData, unsigned int workTexture, unsigned int sceneTexture) const = 0;
			virtual bool Resize(const Vector2ui& GBufferSize);

			DeferredRenderPass& operator=(const DeferredRenderPass&) = delete;

		protected:
			Vector2ui m_dimensions;
			DeferredRenderTechnique* m_deferredTechnique;
			DeferredRenderQueue* m_renderQueue;
			RenderTexture* m_GBufferRTT;
			RenderTexture* m_workRTT;
			Texture* m_depthStencilTexture;
			Texture* m_GBuffer[4];
			Texture* m_workTextures[2];

		private:
			bool m_enabled;
	};
}

#endif // NAZARA_DEFERREDRENDERPASS_HPP
