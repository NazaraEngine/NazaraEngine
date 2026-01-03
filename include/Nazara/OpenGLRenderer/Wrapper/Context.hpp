// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_CONTEXT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_CONTEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/OpenGL.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <array>
#include <string>
#include <unordered_set>

namespace Nz
{
	class OpenGLDevice;
	class OpenGLTexture;
}

namespace Nz::GL
{
	class Framebuffer;
	class Texture;

	enum class BufferTarget
	{
		Array,
		CopyRead,
		CopyWrite,
		ElementArray,
		PixelPack,
		PixelUnpack,
		Storage,
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
		ClipControl,
		ComputeShader,
		DebugOutput,
		DepthClamp,
		PolygonMode,
		ShaderImageLoadFormatted,
		ShaderImageLoadStore,
		SpirV,
		StorageBuffers,
		TextureCompressionS3tc,
		TextureFilterAnisotropic,
		TextureView,

		Max = TextureView
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
		RenderAPIValidationLevel validationLevel = RenderAPIValidationLevel::Warnings;
		bool doubleBuffering = true;
		bool wrapErrorHandling = false;
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
			Context(const OpenGLDevice* device);
			Context(const Context&) = delete;
			Context(Context&&) = delete;
			virtual ~Context();

			void BindBuffer(BufferTarget target, GLuint buffer, bool force = false) const;
			[[nodiscard]] GLenum BindFramebuffer(GLuint fbo) const;
			void BindFramebuffer(FramebufferTarget target, GLuint fbo) const;
			void BindImageTexture(GLuint imageUnit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) const;
			void BindProgram(GLuint program) const;
			void BindSampler(UInt32 textureUnit, GLuint sampler) const;
			void BindStorageBuffer(UInt32 storageUnit, GLuint buffer, GLintptr offset, GLsizeiptr size) const;
			void BindTexture(TextureTarget target, GLuint texture) const;
			void BindTexture(UInt32 textureUnit, TextureTarget target, GLuint texture) const;
			void BindUniformBuffer(UInt32 uboUnit, GLuint buffer, GLintptr offset, GLsizeiptr size) const;
			void BindVertexArray(GLuint vertexArray, bool force = false) const;

			bool BlitTexture(const OpenGLTexture& source, const OpenGLTexture& destination, const Boxui& srcBox, const Boxui& dstBox, SamplerFilter filter) const;
			bool BlitTextureToWindow(const OpenGLTexture& texture, const Boxui& srcBox, const Boxui& dstBox, SamplerFilter filter) const;

			bool ClearErrorStack() const;

			bool CopyTexture(const OpenGLTexture& source, const OpenGLTexture& destination, const Boxui& srcBox, const Vector3ui& dstPos) const;

			inline bool DidLastCallSucceed() const;

			inline bool GetBoolean(GLenum name) const;
			inline bool GetBoolean(GLenum name, GLuint index) const;
			inline const OpenGLDevice* GetDevice() const;
			inline ExtensionStatus GetExtensionStatus(Extension extension) const;
			inline float GetFloat(GLenum name) const;
			inline GLFunction GetFunctionByIndex(std::size_t funcIndex) const;
			template<typename T> T GetInteger(GLenum name) const;
			template<typename T> T GetInteger(GLenum name, GLuint index) const;
			inline const ContextParams& GetParams() const;
			virtual PresentModeFlags GetSupportedPresentModes() const = 0;
			inline const OpenGLVaoCache& GetVaoCache() const;

			inline bool IsExtensionSupported(Extension extension) const;
			inline bool IsExtensionSupported(std::string_view extension) const;

			inline bool HasZeroToOneDepth() const;

			bool Initialize(const ContextParams& params);

			inline void NotifyBufferDestruction(GLuint buffer) const;
			inline void NotifyFramebufferDestruction(GLuint fbo) const;
			inline void NotifyProgramDestruction(GLuint program) const;
			inline void NotifySamplerDestruction(GLuint sampler) const;
			inline void NotifyTextureDestruction(GLuint texture) const;
			inline void NotifyVertexArrayDestruction(GLuint vao) const;

			template<typename... Args> void PrintFunctionCall(std::size_t funcIndex, Args... args) const;
			bool ProcessErrorStack() const;

			inline void ResetColorWriteMasks() const;
			inline void ResetDepthWriteMasks() const;
			inline void ResetStencilWriteMasks() const;

			void SetCurrentTextureUnit(UInt32 textureUnit) const;
			virtual void SetPresentMode(PresentMode presentMode) = 0;
			void SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height) const;
			void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) const;

			virtual void SwapBuffers() = 0;

			void UpdateStates(const RenderStates& renderStates, bool isViewportFlipped) const;

#define NAZARA_OPENGLRENDERER_FUNCTION(name, sig) sig name = nullptr;
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>

			Context& operator=(const Context&) = delete;
			Context& operator=(Context&&) = delete;

			static const Context* GetCurrentContext();
			static bool SetCurrentContext(const Context* context);

			NazaraSignal(OnContextDestruction, Context* /*context*/);

		protected:
			virtual bool Activate() const = 0;
			virtual void Desactivate() const = 0;
			virtual const Loader& GetLoader() = 0;
			void OnContextRelease();

			virtual bool ImplementFallback(std::string_view function);

			static void NotifyContextDestruction(Context* context);

			ContextParams m_params;

		private:
			void HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
			bool InitializeBlitFramebuffers() const;
			static void BindTextureToFramebuffer(Framebuffer& framebuffer, const OpenGLTexture& texture);

			enum class FunctionIndex
			{
#define NAZARA_OPENGLRENDERER_FUNCTION(name, sig) name,
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>

				Count
			};

			struct BlitFramebuffers;

			struct State
			{
				struct Box
				{
					GLint x, y;
					GLsizei width, height;
				};

				struct BufferBinding
				{
					GLuint buffer = 0;
					GLintptr offset = 0;
					GLsizeiptr size = 0;
				};

				struct ImageUnits
				{
					GLboolean layered = GL_FALSE;
					GLenum access = GL_READ_ONLY;
					GLenum format = GL_RGBA8;
					GLint layer = 0;
					GLint level = 0;
					GLuint texture = 0;
				};

				struct TextureUnit
				{
					GLuint sampler = 0;
					EnumArray<TextureTarget, GLuint> textureTargets = { 0 };
				};

				EnumArray<BufferTarget, GLuint> bufferTargets = { 0 };
				std::vector<BufferBinding> storageUnits;
				std::vector<BufferBinding> uboUnits;
				std::vector<ImageUnits> imageUnits;
				std::vector<TextureUnit> textureUnits;
				Box scissorBox;
				Box viewport;
				GLuint boundProgram = 0;
				GLuint boundDrawFBO = 0;
				GLuint boundReadFBO = 0;
				GLuint boundVertexArray = 0;
				UInt32 currentTextureUnit = 0;
				RenderStates renderStates;
			};

			EnumArray<Extension, ExtensionStatus> m_extensionStatus;
			std::array<GLFunction, UnderlyingCast(FunctionIndex::Count)> m_originalFunctionPointer;
			mutable std::unique_ptr<BlitFramebuffers> m_blitFramebuffers;
			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_supportedExtensions;
			OpenGLVaoCache m_vaoCache;
			const OpenGLDevice* m_device;
			mutable State m_state;
			mutable bool m_didCollectErrors;
			mutable bool m_hadAnyError;
			bool m_hasZeroToOneDepth;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Context.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_CONTEXT_HPP
