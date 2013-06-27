// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzColor;
class NzContext;
class NzIndexBuffer;
class NzMaterial;
class NzRenderTarget;
class NzShader;
class NzVertexBuffer;

class NAZARA_API NzRenderer
{
	public:
		struct InstancingData
		{
			NzMatrix4f worldMatrix;
		};

		NzRenderer() = delete;
		~NzRenderer() = delete;

		static void Clear(unsigned long flags = nzRendererClear_Color | nzRendererClear_Depth);

		static void DrawIndexedPrimitives(nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
		static void DrawIndexedPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
		static void DrawPrimitives(nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);
		static void DrawPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);
		static void DrawFullscreenQuad();

		static void Enable(nzRendererParameter parameter, bool enable);

		static void Flush();

		static float GetLineWidth();
		//static NzMatrix4f GetMatrix(nzMatrixCombination combination);
		static NzMatrix4f GetMatrix(nzMatrixType type);
		static nzUInt8 GetMaxAnisotropyLevel();
		static unsigned int GetMaxRenderTargets();
		static unsigned int GetMaxTextureUnits();
		static float GetPointSize();
		static const NzRenderStates& GetRenderStates();
		static NzRectui GetScissorRect();
		static const NzShader* GetShader();
		static const NzRenderTarget* GetTarget();
		static NzRectui GetViewport();

		static bool HasCapability(nzRendererCap capability);

		static bool Initialize();

		static bool IsEnabled(nzRendererParameter parameter);
		static bool IsInitialized();

		static void SetBlendFunc(nzBlendFunc srcBlend, nzBlendFunc dstBlend);
		static void SetClearColor(const NzColor& color);
		static void SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a = 255);
		static void SetClearDepth(double depth);
		static void SetClearStencil(unsigned int value);
		static void SetDepthFunc(nzRendererComparison compareFunc);
		static void SetFaceCulling(nzFaceCulling cullingMode);
		static void SetFaceFilling(nzFaceFilling fillingMode);
		static void SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		static void SetInstancingData(const InstancingData* instancingData, unsigned int instanceCount);
		static void SetLineWidth(float size);
		static void SetMatrix(nzMatrixType type, const NzMatrix4f& matrix);
		static void SetPointSize(float size);
		static void SetRenderStates(const NzRenderStates& states);
		static void SetScissorRect(const NzRectui& viewport);
		static void SetShader(const NzShader* shader);
		static void SetStencilCompareFunction(nzRendererComparison compareFunc);
		static void SetStencilFailOperation(nzStencilOperation failOperation);
		static void SetStencilMask(nzUInt32 mask);
		static void SetStencilPassOperation(nzStencilOperation passOperation);
		static void SetStencilReferenceValue(unsigned int refValue);
		static void SetStencilZFailOperation(nzStencilOperation zfailOperation);
		static bool SetTarget(const NzRenderTarget* target);
		static void SetTexture(nzUInt8 unit, const NzTexture* texture);
		static void SetTextureSampler(nzUInt8 textureUnit, const NzTextureSampler& sampler);
		static void SetVertexBuffer(const NzVertexBuffer* vertexBuffer);
		static void SetViewport(const NzRectui& viewport);

		static void Uninitialize();

	private:
		static void EnableInstancing(bool instancing);
		static bool EnsureStateUpdate();

		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_RENDERER_HPP
