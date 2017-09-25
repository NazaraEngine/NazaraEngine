// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class Color;
	class Context;
	class GpuQuery;
	class IndexBuffer;
	class RenderTarget;
	class RenderStates;
	class Shader;
	class Texture;
	class TextureSampler;
	class VertexBuffer;
	class VertexDeclaration;

	class NAZARA_RENDERER_API Renderer
	{
		friend Texture;

		public:
			using DrawCall = void (*)(PrimitiveMode, unsigned int, unsigned int);
			using DrawCallInstanced = void (*)(unsigned int, PrimitiveMode, unsigned int, unsigned int);

			Renderer() = delete;
			~Renderer() = delete;

			static void BeginCondition(const GpuQuery& query, GpuQueryCondition condition);

			static void Clear(UInt32 flags = RendererBuffer_Color | RendererBuffer_Depth);

			static void DrawFullscreenQuad();
			static void DrawIndexedPrimitives(PrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
			static void DrawIndexedPrimitivesInstanced(unsigned int instanceCount, PrimitiveMode mode, unsigned int firstIndex, unsigned int indexCount);
			static void DrawPrimitives(PrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);
			static void DrawPrimitivesInstanced(unsigned int instanceCount, PrimitiveMode mode, unsigned int firstVertex, unsigned int vertexCount);

			static void Enable(RendererParameter parameter, bool enable);

			static void EndCondition();

			static void Flush();

			static RendererComparison GetDepthFunc();
			static VertexBuffer* GetInstanceBuffer();
			static float GetLineWidth();
			static Matrix4f GetMatrix(MatrixType type);
			static UInt8 GetMaxAnisotropyLevel();
			static unsigned int GetMaxColorAttachments();
			static unsigned int GetMaxRenderTargets();
			static unsigned int GetMaxTextureSize();
			static unsigned int GetMaxTextureUnits();
			static unsigned int GetMaxVertexAttribs();
			static float GetPointSize();
			static const RenderStates& GetRenderStates();
			static Recti GetScissorRect();
			static const Shader* GetShader();
			static const RenderTarget* GetTarget();
			static Recti GetViewport();

			static bool HasCapability(RendererCap capability);

			static bool Initialize();

			static bool IsComponentTypeSupported(ComponentType type);
			static bool IsEnabled(RendererParameter parameter);
			static bool IsInitialized();

			static void SetBlendFunc(BlendFunc srcBlend, BlendFunc dstBlend);
			static void SetClearColor(const Color& color);
			static void SetClearColor(UInt8 r, UInt8 g, UInt8 b, UInt8 a = 255);
			static void SetClearDepth(double depth);
			static void SetClearStencil(unsigned int value);
			static void SetDepthFunc(RendererComparison compareFunc);
			static void SetFaceCulling(FaceSide faceSide);
			static void SetFaceFilling(FaceFilling fillingMode);
			static void SetIndexBuffer(const IndexBuffer* indexBuffer);
			static void SetLineWidth(float size);
			static void SetMatrix(MatrixType type, const Matrix4f& matrix);
			static void SetPointSize(float size);
			static void SetRenderStates(const RenderStates& states);
			static void SetScissorRect(const Recti& rect);
			static void SetShader(const Shader* shader);
			static void SetStencilCompareFunction(RendererComparison compareFunc, FaceSide faceSide = FaceSide_FrontAndBack);
			static void SetStencilFailOperation(StencilOperation failOperation, FaceSide faceSide = FaceSide_FrontAndBack);
			static void SetStencilMask(UInt32 mask, FaceSide faceSide = FaceSide_FrontAndBack);
			static void SetStencilPassOperation(StencilOperation passOperation, FaceSide faceSide = FaceSide_FrontAndBack);
			static void SetStencilReferenceValue(unsigned int refValue, FaceSide faceSide = FaceSide_FrontAndBack);
			static void SetStencilZFailOperation(StencilOperation zfailOperation, FaceSide faceSide = FaceSide_FrontAndBack);
			static bool SetTarget(const RenderTarget* target);
			static void SetTexture(unsigned int unit, const Texture* texture);
			static void SetTextureSampler(unsigned int textureUnit, const TextureSampler& sampler);
			static void SetVertexBuffer(const VertexBuffer* vertexBuffer);
			static void SetViewport(const Recti& viewport);

			static void Uninitialize();

		private:
			static void EnableInstancing(bool instancing);
			static bool EnsureStateUpdate();
			static bool GenerateDebugShader();
			static void OnContextRelease(const Context* context);
			static void OnIndexBufferRelease(const IndexBuffer* indexBuffer);
			static void OnShaderReleased(const Shader* shader);
			static void OnTextureReleased(const Texture* texture);
			static void OnVertexBufferRelease(const VertexBuffer* vertexBuffer);
			static void OnVertexDeclarationRelease(const VertexDeclaration* vertexDeclaration);
			static void UpdateMatrix(MatrixType type);

			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_RENDERER_HPP
