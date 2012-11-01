// Copyright (C) 2012 Jérôme Leclercq
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
#include <Nazara/Utility/Enums.hpp>
#include <map>

class NzColor;
class NzContext;
class NzIndexBuffer;
class NzRenderTarget;
class NzShader;
class NzVertexBuffer;
class NzVertexDeclaration;

class NAZARA_API NzRenderer
{
	public:
		NzRenderer() = delete;
		~NzRenderer() = delete;

		static void Clear(unsigned long flags = nzRendererClear_Color | nzRendererClear_Depth);

		static void DrawIndexedPrimitives(nzPrimitiveType primitive, unsigned int firstIndex, unsigned int indexCount);
		static void DrawPrimitives(nzPrimitiveType primitive, unsigned int firstVertex, unsigned int vertexCount);

		static void Enable(nzRendererParameter parameter, bool enable);

		//static NzMatrix4f GetMatrix(nzMatrixCombination combination);
		static NzMatrix4f GetMatrix(nzMatrixType type);
		static unsigned int GetMaxAnisotropyLevel();
		static unsigned int GetMaxRenderTargets();
		static unsigned int GetMaxTextureUnits();
		static float GetPointSize();
		static NzShader* GetShader();
		static NzRenderTarget* GetTarget();
		static NzRectui GetViewport();

		static bool HasCapability(nzRendererCap capability);

		static bool Initialize();

		static bool IsInitialized();

		static void SetBlendFunc(nzBlendFunc src, nzBlendFunc dest);
		static void SetClearColor(const NzColor& color);
		static void SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a = 255);
		static void SetClearDepth(double depth);
		static void SetClearStencil(unsigned int value);
		static void SetFaceCulling(nzFaceCulling cullingMode);
		static void SetFaceFilling(nzFaceFilling fillingMode);
		static bool SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		static void SetMatrix(nzMatrixType type, const NzMatrix4f& matrix);
		static void SetPointSize(float size);
		static bool SetShader(NzShader* shader);
		static void SetStencilCompareFunction(nzRendererComparison compareFunc);
		static void SetStencilFailOperation(nzStencilOperation failOperation);
		static void SetStencilMask(nzUInt32 mask);
		static void SetStencilPassOperation(nzStencilOperation passOperation);
		static void SetStencilReferenceValue(unsigned int refValue);
		static void SetStencilZFailOperation(nzStencilOperation zfailOperation);
		static bool SetTarget(NzRenderTarget* target);
		static bool SetVertexBuffer(const NzVertexBuffer* vertexBuffer);
		static bool SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration);
		static void SetViewport(const NzRectui& viewport);

		static void Uninitialize();

	private:
		static bool EnsureStateUpdate();

		static unsigned int s_moduleReferenceCouter;
};

#endif // NAZARA_RENDERER_HPP
