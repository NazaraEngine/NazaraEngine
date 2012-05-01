// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/IndexBuffer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/VertexBuffer.hpp>

#define NazaraRenderer NzRenderer::Instance()

enum nzRendererCap
{
	nzRendererCap_AnisotropicFilter,
	nzRendererCap_FP64,
	nzRendererCap_HardwareBuffer,
	nzRendererCap_MultipleRenderTargets,
	nzRendererCap_Texture3D,
	nzRendererCap_TextureCubemap,
	nzRendererCap_TextureMulti,
	nzRendererCap_TextureNPOT,

	nzRendererCap_Count
};

enum nzRendererClear
{
	nzRendererClear_Color = 0x01,
	nzRendererClear_Depth = 0x02,
	nzRendererClear_Stencil = 0x04
};

class NAZARA_API NzRenderer
{
	public:
		NzRenderer();
		~NzRenderer();

		void Clear(nzRendererClear flags);

		NzShader* GetShader() const;
		NzRenderTarget* GetTarget() const;

		bool HasCapability(nzRendererCap capability) const;
		bool Initialize();

		void SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a = 255);
		void SetClearDepth(double depth);
		void SetClearStencil(unsigned int value);
		bool SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		bool SetShader(NzShader* shader);
		bool SetTarget(NzRenderTarget* target);
		bool SetVertexBuffer(const NzVertexBuffer* vertexBuffer);

		void Uninitialize();

		#if NAZARA_RENDERER_SINGLETON
		static void Destroy();
		#endif
		static NzRenderer* Instance();

	private:
		static NzRenderer* s_instance;

		const NzIndexBuffer* m_indexBuffer;
		NzRenderTarget* m_target;
		NzShader* m_shader;
		const NzVertexBuffer* m_vertexBuffer;
		bool m_capabilities[nzRendererCap_Count];
		bool m_vertexBufferUpdated;
};

#endif // NAZARA_RENDERER_HPP
