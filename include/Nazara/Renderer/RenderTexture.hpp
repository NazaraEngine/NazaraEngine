// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTEXTURE_HPP
#define NAZARA_RENDERTEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Texture.hpp>

///TODO: Faire fonctionner les RenderTexture indépendamment du contexte (un FBO par classe et par contexte l'utilisant)

class NzRenderBuffer;

struct NzRenderTextureImpl;

class NAZARA_API NzRenderTexture : public NzRenderTarget, NzResourceListener, NzNonCopyable
{
	public:
		NzRenderTexture() = default;
		~NzRenderTexture();

		bool AttachBuffer(nzAttachmentPoint attachmentPoint, nzUInt8 index, NzRenderBuffer* buffer);
		bool AttachBuffer(nzAttachmentPoint attachmentPoint, nzUInt8 index, nzPixelFormat format, unsigned int width, unsigned int height);
		bool AttachTexture(nzAttachmentPoint attachmentPoint, nzUInt8 index, NzTexture* texture, unsigned int z = 0);

		bool Create(bool lock = false);
		void Destroy();

		void Detach(nzAttachmentPoint attachmentPoint, nzUInt8 index);

		unsigned int GetHeight() const;
		NzRenderTargetParameters GetParameters() const;
		NzVector2ui GetSize() const;
		unsigned int GetWidth() const;

		bool IsComplete() const;
		bool IsRenderable() const;
		bool IsValid() const;

		bool Lock() const;

		void SetColorTarget(nzUInt8 target) const;
		void SetColorTargets(const nzUInt8* targets, unsigned int targetCount) const;
		void SetColorTargets(const std::initializer_list<nzUInt8>& targets) const;

		void Unlock() const;

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;
		bool HasContext() const override;

		static void Blit(NzRenderTexture* src, NzRenderTexture* dst, nzUInt32 buffers = nzRendererBuffer_Color | nzRendererBuffer_Depth | nzRendererBuffer_Stencil, bool bilinearFilter = false);
		static void Blit(NzRenderTexture* src, NzRectui srcRect, NzRenderTexture* dst, NzRectui dstRect, nzUInt32 buffers = nzRendererBuffer_Color | nzRendererBuffer_Depth | nzRendererBuffer_Stencil, bool bilinearFilter = false);
		static bool IsSupported();

	protected:
		bool Activate() const override;
		void Desactivate() const override;
		void EnsureTargetUpdated() const override;

	private:
		bool OnResourceDestroy(const NzResource* resource, int index) override;
		void UpdateDrawBuffers() const;
		void UpdateTargets() const;

		NzRenderTextureImpl* m_impl = nullptr;
};

#endif // NAZARA_RENDERTEXTURE_HPP
