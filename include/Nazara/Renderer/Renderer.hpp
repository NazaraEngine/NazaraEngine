// Copyright (C) 2014 Jérôme Leclercq
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
#include <Nazara/Renderer/GpuQuery.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NzColor;
class NzContext;
class NzIndexBuffer;
class NzMaterial;
class NzRenderTarget;
class NzShaderProgram;
class NzTexture;
class NzVertexBuffer;

class NAZARA_API NzRenderer
{
	friend NzShaderProgram;
	friend NzTexture;

	public:
		NzRenderer() = delete;
		~NzRenderer() = delete;

		static void BeginCondition(const NzGpuQuery& query, nzGpuQueryCondition condition);

		static void Clear(nzUInt32 flags = nzRendererBuffer_Color | nzRendererBuffer_Depth);

		static void DrawFullscreenQuad();
		static void DrawIndexedPrimitives(nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
		static void DrawIndexedPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
		static void DrawPrimitives(nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);
		static void DrawPrimitivesInstanced(unsigned int instanceCount, nzPrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);

		static void Enable(nzRendererParameter parameter, bool enable);

		static void EndCondition();

		static void Flush();

		static nzRendererComparison GetDepthFunc();
		static NzVertexBuffer* GetInstanceBuffer();
		static float GetLineWidth();
		static NzMatrix4f GetMatrix(nzMatrixType type);
		static nzUInt8 GetMaxAnisotropyLevel();
		static unsigned int GetMaxColorAttachments();
		static unsigned int GetMaxRenderTargets();
		static unsigned int GetMaxTextureUnits();
		static unsigned int GetMaxVertexAttribs();
		static float GetPointSize();
		static const NzRenderStates& GetRenderStates();
		static NzRecti GetScissorRect();
		static const NzShaderProgram* GetShaderProgram();
		static const NzRenderTarget* GetTarget();
		static NzRecti GetViewport();

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
		static void SetFaceCulling(nzFaceSide faceSide);
		static void SetFaceFilling(nzFaceFilling fillingMode);
		static void SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		static void SetLineWidth(float size);
		static void SetMatrix(nzMatrixType type, const NzMatrix4f& matrix);
		static void SetPointSize(float size);
		static void SetRenderStates(const NzRenderStates& states);
		static void SetScissorRect(const NzRecti& rect);
		static void SetShaderProgram(const NzShaderProgram* shader);
		static void SetStencilCompareFunction(nzRendererComparison compareFunc, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static void SetStencilFailOperation(nzStencilOperation failOperation, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static void SetStencilMask(nzUInt32 mask, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static void SetStencilPassOperation(nzStencilOperation passOperation, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static void SetStencilReferenceValue(unsigned int refValue, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static void SetStencilZFailOperation(nzStencilOperation zfailOperation, nzFaceSide faceSide = nzFaceSide_FrontAndBack);
		static bool SetTarget(const NzRenderTarget* target);
		static void SetTexture(nzUInt8 unit, const NzTexture* texture);
		static void SetTextureSampler(nzUInt8 textureUnit, const NzTextureSampler& sampler);
		static void SetVertexBuffer(const NzVertexBuffer* vertexBuffer);
		static void SetViewport(const NzRecti& viewport);

		static void Uninitialize();

	private:
		static void EnableInstancing(bool instancing);
		static bool EnsureStateUpdate();
		static void OnProgramReleased(const NzShaderProgram* program);
		static void OnTextureReleased(const NzTexture* texture);
		static void UpdateMatrix(nzMatrixType type);

		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_RENDERER_HPP
