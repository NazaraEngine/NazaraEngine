// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTEXTURE_HPP
#define NAZARA_RENDERTEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/PixelFormat.hpp>

///TODO: Faire fonctionner les RenderTexture indépendamment du contexte (un FBO par instance et par contexte l'utilisant)

namespace Nz
{

	class Context;
	class RenderBuffer;
	class Texture;

	struct RenderTextureImpl;

	class NAZARA_RENDERER_API RenderTexture : public RenderTarget
	{
		public:
			inline RenderTexture();
			RenderTexture(const RenderTexture&) = delete;
			RenderTexture(RenderTexture&&) noexcept = default;
			inline ~RenderTexture();

			bool AttachBuffer(AttachmentPoint attachmentPoint, UInt8 index, RenderBuffer* buffer);
			bool AttachBuffer(AttachmentPoint attachmentPoint, UInt8 index, PixelFormatType format, unsigned int width, unsigned int height);
			bool AttachTexture(AttachmentPoint attachmentPoint, UInt8 index, Texture* texture, unsigned int z = 0);

			bool Create(bool lock = false);
			void Destroy();

			void Detach(AttachmentPoint attachmentPoint, UInt8 index);

			RenderTargetParameters GetParameters() const override;
			Vector2ui GetSize() const override;

			bool IsComplete() const;
			bool IsRenderable() const override;
			inline bool IsValid() const;

			bool Lock() const;

			inline void SetColorTarget(UInt8 target) const;
			void SetColorTargets(const UInt8* targets, unsigned int targetCount) const;
			void SetColorTargets(const std::initializer_list<UInt8>& targets) const;

			void Unlock() const;

			// Fonctions OpenGL
			unsigned int GetOpenGLID() const;
			bool HasContext() const override;

			RenderTexture& operator=(const RenderTexture&) = delete;
			RenderTexture& operator=(RenderTexture&&) noexcept = default;

			static inline void Blit(RenderTexture* src, RenderTexture* dst, UInt32 buffers = RendererBuffer_Color | RendererBuffer_Depth | RendererBuffer_Stencil, bool bilinearFilter = false);
			static void Blit(RenderTexture* src, Rectui srcRect, RenderTexture* dst, Rectui dstRect, UInt32 buffers = RendererBuffer_Color | RendererBuffer_Depth | RendererBuffer_Stencil, bool bilinearFilter = false);

		protected:
			bool Activate() const override;
			void Desactivate() const override;
			void EnsureTargetUpdated() const override;

		private:
			inline void InvalidateDrawBuffers() const;
			inline void InvalidateSize() const;
			inline void InvalidateTargets() const;
			void OnContextDestroy(const Context* context);
			void OnRenderBufferDestroy(const RenderBuffer* renderBuffer, unsigned int attachmentIndex);
			void OnTextureDestroy(const Texture* texture, unsigned int attachmentIndex);
			void UpdateDrawBuffers() const;
			void UpdateSize() const;
			void UpdateTargets() const;

			MovablePtr<RenderTextureImpl> m_impl;
			mutable bool m_checked ;
			mutable bool m_drawBuffersUpdated;
			mutable bool m_sizeUpdated;
			mutable bool m_targetsUpdated;
	};
}

#include <Nazara/Renderer/RenderTexture.inl>

#endif // NAZARA_RENDERTEXTURE_HPP
