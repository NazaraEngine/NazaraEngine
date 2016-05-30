// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERPASS_HPP
#define NAZARA_DEFERREDRENDERPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	class AbstractViewer;
	class DeferredRenderTechnique;
	class DeferredRenderQueue;
	class RenderBuffer;
	class RenderTexture;
	class Scene;
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
			RenderBuffer* m_depthStencilBuffer;
			RenderTexture* m_GBufferRTT;
			RenderTexture* m_workRTT;
			Texture* m_GBuffer[4];
			Texture* m_workTextures[2];

		private:
			bool m_enabled;
	};
}

#endif // NAZARA_DEFERREDRENDERPASS_HPP
