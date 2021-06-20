// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLVERTEXARRAY_HPP
#define NAZARA_OPENGLRENDERER_GLVERTEXARRAY_HPP

#include <Nazara/Prerequisites.hpp>
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

#endif
