// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLVAOCACHE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLVAOCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
#include <array>
#include <memory>
#include <optional>
#include <unordered_map>

namespace Nz::GL
{
	struct OpenGLVaoSetup
	{
		struct Attribs
		{
			GLuint vertexBuffer;
			GLint size;
			GLenum type;
			GLboolean normalized;
			GLsizei stride;
			const void* pointer;
		};

		GLuint indexBuffer = 0;
		std::array<std::optional<Attribs>, 16> vertexAttribs;

		inline friend bool operator==(const OpenGLVaoSetup& lhs, const OpenGLVaoSetup& rhs);
	};

	struct OpenGLVaoSetupHasher
	{
		inline std::size_t operator()(const OpenGLVaoSetup& setup) const;
	};

	class Context;
	class VertexArray;

	class NAZARA_OPENGLRENDERER_API OpenGLVaoCache
	{
		friend Context;

		public:
			OpenGLVaoCache(Context& owner);
			OpenGLVaoCache(const OpenGLVaoCache&) = delete;
			OpenGLVaoCache(OpenGLVaoCache&&) = delete;
			~OpenGLVaoCache();

			void Clear();

			const VertexArray& Get(const OpenGLVaoSetup& setup) const;

			OpenGLVaoCache& operator=(const OpenGLVaoCache&) = delete;
			OpenGLVaoCache& operator=(OpenGLVaoCache&&) = delete;

		private:
			void NotifyBufferDestruction(GLuint buffer) const;

			mutable std::unordered_map<OpenGLVaoSetup, std::unique_ptr<VertexArray>, OpenGLVaoSetupHasher> m_vertexArrays;
			Context& m_context;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLVaoCache.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLVAOCACHE_HPP
