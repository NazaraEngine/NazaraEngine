// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
<<<<<<< HEAD
	class AbstractBuffer;
	class Buffer;
=======
	class Color;
	class Context;
	class GpuQuery;
	class IndexBuffer;
	class RenderTarget;
	struct RenderStates;
	class Shader;
	class Texture;
	class TextureSampler;
	class UniformBuffer;
	class VertexBuffer;
	class VertexDeclaration;
>>>>>>> ubo

	class NAZARA_RENDERER_API Renderer
	{
		public:
			Renderer() = delete;
			~Renderer() = delete;

<<<<<<< HEAD
			static inline RendererImpl* GetRendererImpl();
=======
			static void BeginCondition(const GpuQuery& query, GpuQueryCondition condition);

			static void BindUniformBuffer(unsigned int bindingPoint, const UniformBuffer* uniformBuffer);

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
>>>>>>> ubo

			static bool Initialize();

			static inline bool IsInitialized();

			static inline void SetParameters(const ParameterList& parameters);

			static void Uninitialize();

		private:
			static AbstractBuffer* CreateHardwareBufferImpl(Buffer* parent, BufferType type);

			static std::unique_ptr<RendererImpl> s_rendererImpl;
			static DynLib s_rendererLib;
			static ParameterList s_initializationParameters;
			static unsigned int s_moduleReferenceCounter;
	};
}

#include <Nazara/Renderer/Renderer.inl>

#endif // NAZARA_RENDERER_HPP
