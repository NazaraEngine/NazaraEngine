// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz::GL
{
	template<typename F>
	VertexArray VertexArray::Build(const Context& context, F&& callback)
	{
		VertexArray vao;
		if (!vao.Create(context))
			throw std::runtime_error("failed to create vao");

		context.BindVertexArray(vao.GetObjectId(), true);
		callback();
		context.BindVertexArray(0, true);

		return vao;
	}

	inline GLuint VertexArray::CreateHelper(const Context& context)
	{
		GLuint vao = 0;
		context.glGenVertexArrays(1U, &vao);

		return vao;
	}

	inline void VertexArray::DestroyHelper(const Context& context, GLuint objectId)
	{
		context.glDeleteVertexArrays(1U, &objectId);

		context.NotifyVertexArrayDestruction(objectId);
	}
}
