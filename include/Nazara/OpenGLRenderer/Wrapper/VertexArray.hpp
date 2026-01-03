// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_VERTEXARRAY_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_VERTEXARRAY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/ContextObject.hpp>

namespace Nz::GL
{
	class VertexArray : public ContextObject<VertexArray, GL_VERTEX_ARRAY>
	{
		friend ContextObject;

		public:
			VertexArray(const VertexArray&) = delete;
			VertexArray(VertexArray&&) = default;
			~VertexArray() = default;

			VertexArray& operator=(const VertexArray&) = delete;
			VertexArray& operator=(VertexArray&&) = delete;

			template<typename F> static VertexArray Build(const Context& context, F&& callback);

		private:
			VertexArray() = default;

			static inline GLuint CreateHelper(const Context& context);
			static inline void DestroyHelper(const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/VertexArray.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_VERTEXARRAY_HPP
