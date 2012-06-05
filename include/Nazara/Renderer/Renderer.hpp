// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <map>
#include <tuple>

#define NazaraRenderer NzRenderer::Instance()

enum nzBlendFunc
{
	nzBlendFunc_DestAlpha,
	nzBlendFunc_DestColor,
	nzBlendFunc_SrcAlpha,
	nzBlendFunc_SrcColor,
	nzBlendFunc_InvDestAlpha,
	nzBlendFunc_InvDestColor,
	nzBlendFunc_InvSrcAlpha,
	nzBlendFunc_InvSrcColor,
	nzBlendFunc_One,
	nzBlendFunc_Zero
};

enum nzFaceCulling
{
	nzFaceCulling_Back,
	nzFaceCulling_Front,
	nzFaceCulling_FrontAndBack
};

enum nzFaceFilling
{
	nzFaceFilling_Point,
	nzFaceFilling_Line,
	nzFaceFilling_Fill
};

enum nzPrimitiveType
{
	nzPrimitiveType_LineList,
	nzPrimitiveType_LineStrip,
	nzPrimitiveType_PointList,
	nzPrimitiveType_TriangleList,
	nzPrimitiveType_TriangleStrip,
	nzPrimitiveType_TriangleFan
};

enum nzRendererCap
{
	nzRendererCap_AnisotropicFilter,
	nzRendererCap_FP64,
	nzRendererCap_HardwareBuffer,
	nzRendererCap_MultipleRenderTargets,
	nzRendererCap_OcclusionQuery,
	nzRendererCap_SoftwareBuffer,
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

enum nzRendererComparison
{
	nzRendererComparison_Always,
	nzRendererComparison_Equal,
	nzRendererComparison_Greater,
	nzRendererComparison_GreaterOrEqual,
	nzRendererComparison_Less,
	nzRendererComparison_LessOrEqual,
	nzRendererComparison_Never
};

enum nzRendererParameter
{
	nzRendererParameter_AlphaTest,
	nzRendererParameter_Blend,
	nzRendererParameter_ColorWrite,
	nzRendererParameter_DepthTest,
	nzRendererParameter_DepthWrite,
	nzRendererParameter_FaceCulling,
	nzRendererParameter_Stencil
};

enum nzStencilOperation
{
	nzStencilOperation_Decrement,
	nzStencilOperation_DecrementToSaturation,
	nzStencilOperation_Increment,
	nzStencilOperation_IncrementToSaturation,
	nzStencilOperation_Invert,
	nzStencilOperation_Keep,
	nzStencilOperation_Replace,
	nzStencilOperation_Zero
};

class NzColor;
class NzContext;
class NzIndexBuffer;
class NzRenderTarget;
class NzShader;
class NzUtility;
class NzVertexBuffer;
class NzVertexDeclaration;

class NAZARA_API NzRenderer
{
	public:
		NzRenderer();
		~NzRenderer();

		void Clear(unsigned long flags = nzRendererClear_Color | nzRendererClear_Depth);

		void DrawIndexedPrimitives(nzPrimitiveType primitive, unsigned int firstIndex, unsigned int indexCount);
		void DrawPrimitives(nzPrimitiveType primitive, unsigned int firstVertex, unsigned int vertexCount);

		void Enable(nzRendererParameter parameter, bool enable);

		unsigned int GetMaxAnisotropyLevel() const;
		unsigned int GetMaxRenderTargets() const;
		unsigned int GetMaxTextureUnits() const;
		NzShader* GetShader() const;
		NzRenderTarget* GetTarget() const;

		bool HasCapability(nzRendererCap capability) const;
		bool Initialize();

		void SetClearColor(const NzColor& color);
		void SetClearColor(nzUInt8 r, nzUInt8 g, nzUInt8 b, nzUInt8 a = 255);
		void SetClearDepth(double depth);
		void SetClearStencil(unsigned int value);
		void SetFaceCulling(nzFaceCulling cullingMode);
		void SetFaceFilling(nzFaceFilling fillingMode);
		bool SetIndexBuffer(const NzIndexBuffer* indexBuffer);
		bool SetShader(NzShader* shader);
		void SetStencilCompareFunction(nzRendererComparison compareFunc);
		void SetStencilFailOperation(nzStencilOperation failOperation);
		void SetStencilMask(nzUInt32 mask);
		void SetStencilPassOperation(nzStencilOperation passOperation);
		void SetStencilReferenceValue(unsigned int refValue);
		void SetStencilZFailOperation(nzStencilOperation zfailOperation);
		bool SetTarget(NzRenderTarget* target);
		bool SetVertexBuffer(const NzVertexBuffer* vertexBuffer);
		bool SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration);

		void Uninitialize();

		static NzRenderer* Instance();
		static bool IsInitialized();

	private:
		bool EnsureStateUpdate();

		typedef std::tuple<const NzContext*, const NzIndexBuffer*, const NzVertexBuffer*, const NzVertexDeclaration*> VAO_Key;

		std::map<VAO_Key, unsigned int> m_vaos;
		nzRendererComparison m_stencilCompare;
		nzStencilOperation m_stencilFail;
		nzStencilOperation m_stencilPass;
		nzStencilOperation m_stencilZFail;
		nzUInt32 m_stencilMask;
		const NzIndexBuffer* m_indexBuffer;
		NzRenderTarget* m_target;
		NzShader* m_shader;
		NzUtility* m_utilityModule;
		const NzVertexBuffer* m_vertexBuffer;
		const NzVertexDeclaration* m_vertexDeclaration;
		bool m_vaoUpdated;
		bool m_capabilities[nzRendererCap_Count];
		bool m_stencilFuncUpdated;
		bool m_stencilOpUpdated;
		unsigned int m_maxAnisotropyLevel;
		unsigned int m_maxRenderTarget;
		unsigned int m_maxTextureUnit;
		unsigned int m_stencilReference;

		static NzRenderer* s_instance;
		static bool s_initialized;
};

#endif // NAZARA_RENDERER_HPP
