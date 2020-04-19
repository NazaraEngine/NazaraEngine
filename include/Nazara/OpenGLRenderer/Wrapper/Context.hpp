// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP
#define NAZARA_OPENGLRENDERER_CONTEXTIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <array>
#include <string>
#include <unordered_set>

namespace Nz
{
	class OpenGLDevice;
}

namespace Nz::GL
{
	enum class ContextType
	{
		OpenGL,
		OpenGL_ES
	};

	enum class Extension
	{
		SpirV,

		Max = SpirV
	};

	enum class ExtensionStatus
	{
		NotSupported,

		ARB,
		EXT,
		KHR
	};

	enum class TextureTarget
	{
		Cubemap,
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

	class Loader;

	class Context
	{
		public:
			inline Context(const OpenGLDevice* device);
			virtual ~Context();

			void BindTexture(TextureTarget target, GLuint texture) const;

			virtual void EnableVerticalSync(bool enabled) = 0;

			inline const OpenGLDevice* GetDevice() const;
			inline ExtensionStatus GetExtensionStatus(Extension extension) const;
			inline const ContextParams& GetParams() const;

			inline bool IsExtensionSupported(Extension extension) const;
			inline bool IsExtensionSupported(const std::string& extension) const;

			bool Initialize(const ContextParams& params);

			inline void NotifyTextureDestruction(GLuint texture) const;

			virtual void SwapBuffers() = 0;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig) sig name = nullptr;
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC

			static const Context* GetCurrentContext();
			static bool SetCurrentContext(const Context* context);

		protected:
			virtual bool Activate() const = 0;
			virtual void Desactivate() const = 0;
			virtual const Loader& GetLoader() = 0;

			virtual bool ImplementFallback(const std::string_view& function);

			static void NotifyContextDestruction(Context* context);

			ContextParams m_params;

		private:
			void GL_APIENTRY HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;

			struct State
			{
				std::array<GLuint, UnderlyingCast(TextureTarget::Max) + 1> boundTextures;
			};

			std::array<ExtensionStatus, UnderlyingCast(Extension::Max) + 1> m_extensionStatus;
			std::unordered_set<std::string> m_supportedExtensions;
			const OpenGLDevice* m_device;
			mutable State m_state;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Context.inl>

#endif
