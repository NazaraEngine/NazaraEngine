// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP
#define NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <array>
#include <string>
#include <unordered_set>

namespace Nz
{
	class OpenGLDevice;
}

namespace Nz::GL
{
	enum class BufferTarget
	{
		Array,
		CopyRead,
		CopyWrite,
		ElementArray,
		PixelPack,
		PixelUnpack,
		TransformFeedback,
		Uniform,

		Max = Uniform
	};

	enum class ContextType
	{
		OpenGL,
		OpenGL_ES
	};

	enum class Extension
	{
		DepthClamp,
		SpirV,
		TextureCompressionS3tc,
		TextureFilterAnisotropic,

		Max = TextureFilterAnisotropic
	};

	enum class ExtensionStatus
	{
		NotSupported = 0,

		ARB = 3,
		Core = 5,
		EXT = 2,
		KHR = 4,
		Vendor = 1
	};

	enum class FramebufferTarget
	{
		Draw,
		Read
	};

	enum class TextureTarget
	{
		Cubemap,
		CubemapNegativeX,
		CubemapNegativeY,
		CubemapNegativeZ,
		CubemapPositiveX,
		CubemapPositiveY,
		CubemapPositiveZ,
		Target2D,
		Target2D_Array,
		Target3D,

		Max = Target3D
	};

	struct ContextParams
	{
		ContextType type = ContextType::OpenGL_ES;
		bool doubleBuffering = true;
		unsigned int bitsPerPixel = 32;
		unsigned int depthBits = 24;
		unsigned int glMajorVersion = 0;
		unsigned int glMinorVersion = 0;
		unsigned int sampleCount = 1;
		unsigned int stencilBits = 8;
	};

	class NAZARA_OPENGLRENDERER_API Context
	{
		struct SymbolLoader;
		friend SymbolLoader;

		public:
			inline Context(const OpenGLDevice* device);
			virtual ~Context();

			void BindBuffer(BufferTarget target, GLuint buffer, bool force = false) const;
			void BindFramebuffer(GLuint fbo) const;
			void BindFramebuffer(FramebufferTarget target, GLuint fbo) const;
			void BindProgram(GLuint program) const;
			void BindSampler(UInt32 textureUnit, GLuint sampler) const;
			void BindTexture(TextureTarget target, GLuint texture) const;
			void BindTexture(UInt32 textureUnit, TextureTarget target, GLuint texture) const;
			void BindUniformBuffer(UInt32 uboUnit, GLuint buffer, GLintptr offset, GLsizeiptr size) const;
			void BindVertexArray(GLuint vertexArray, bool force = false) const;

			bool ClearErrorStack() const;

			virtual void EnableVerticalSync(bool enabled) = 0;

			inline const OpenGLDevice* GetDevice() const;
			inline ExtensionStatus GetExtensionStatus(Extension extension) const;
			inline GLFunction GetFunctionByIndex(std::size_t funcIndex) const;
			inline const OpenGLVaoCache& GetVaoCache() const;
			inline const ContextParams& GetParams() const;

			inline bool IsExtensionSupported(Extension extension) const;
			inline bool IsExtensionSupported(const std::string& extension) const;

			bool Initialize(const ContextParams& params);

			inline void NotifyBufferDestruction(GLuint buffer) const;
			inline void NotifyFramebufferDestruction(GLuint fbo) const;
			inline void NotifyProgramDestruction(GLuint program) const;
			inline void NotifySamplerDestruction(GLuint sampler) const;
			inline void NotifyTextureDestruction(GLuint texture) const;
			inline void NotifyVertexArrayDestruction(GLuint vao) const;

			bool ProcessErrorStack() const;

			inline void ResetColorWriteMasks() const;
			inline void ResetDepthWriteMasks() const;
			inline void ResetStencilWriteMasks() const;

			void SetCurrentTextureUnit(UInt32 textureUnit) const;
			void SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height) const;
			void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) const;

			virtual void SwapBuffers() = 0;

			void UpdateStates(const RenderStates& renderStates, bool isViewportFlipped) const;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

			static const Context* GetCurrentContext();
			static bool SetCurrentContext(const Context* context);

		protected:
			virtual bool Activate() const = 0;
			virtual void Desactivate() const = 0;
			virtual const Loader& GetLoader() = 0;
			void OnContextRelease();

			virtual bool ImplementFallback(const std::string_view& function);

			static void NotifyContextDestruction(Context* context);

			ContextParams m_params;

		private:
			void GL_APIENTRY HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;

			enum class FunctionIndex
			{
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) name,
				NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

				Count
			};

			struct State
			{
				struct Box
				{
					GLint x, y;
					GLsizei width, height;
				};

				struct TextureUnit
				{
					GLuint sampler = 0;
					std::array<GLuint, UnderlyingCast(TextureTarget::Max) + 1> textureTargets = { 0 };
				};

				struct UniformBufferUnit
				{
					GLuint buffer = 0;
					GLintptr offset = 0;
					GLsizeiptr size = 0;
				};

				std::array<GLuint, UnderlyingCast(BufferTarget::Max) + 1> bufferTargets = { 0 };
				std::vector<TextureUnit> textureUnits;
				std::vector<UniformBufferUnit> uboUnits;
				Box scissorBox;
				Box viewport;
				GLuint boundProgram = 0;
				GLuint boundDrawFBO = 0;
				GLuint boundReadFBO = 0;
				GLuint boundVertexArray = 0;
				UInt32 currentTextureUnit = 0;
				RenderStates renderStates;
			};

			std::array<ExtensionStatus, UnderlyingCast(Extension::Max) + 1> m_extensionStatus;
			std::array<GLFunction, UnderlyingCast(FunctionIndex::Count)> m_originalFunctionPointer;
			std::unordered_set<std::string> m_supportedExtensions;
			OpenGLVaoCache m_vaoCache;
			const OpenGLDevice* m_device;
			mutable State m_state;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Context.inl>

#endif
