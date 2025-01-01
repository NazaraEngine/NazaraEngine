// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/VertexArray.hpp>

namespace Nz::GL
{
	OpenGLVaoCache::OpenGLVaoCache(Context& context) :
	m_context(context)
	{
	}

	OpenGLVaoCache::~OpenGLVaoCache() = default;

	void OpenGLVaoCache::Clear()
	{
		m_vertexArrays.clear();
	}

	const VertexArray& OpenGLVaoCache::Get(const OpenGLVaoSetup& setup) const
	{
		auto it = m_vertexArrays.find(setup);
		if (it == m_vertexArrays.end())
		{
			VertexArray vao = VertexArray::Build(m_context, [&]
			{
				if (setup.indexBuffer != 0)
					m_context.BindBuffer(BufferTarget::ElementArray, setup.indexBuffer, true);

				GLuint bindingIndex = 0;
				for (const auto& attribOpt : setup.vertexAttribs)
				{
					if (attribOpt)
					{
						const auto& attrib = attribOpt.value();

						assert(attrib.vertexBuffer != 0);
						m_context.BindBuffer(BufferTarget::Array, attrib.vertexBuffer, true);

						m_context.glEnableVertexAttribArray(bindingIndex);

						switch (attrib.type)
						{
							case GL_BYTE:
							case GL_UNSIGNED_BYTE:
							case GL_SHORT:
							case GL_UNSIGNED_SHORT:
							case GL_INT:
							case GL_UNSIGNED_INT:
								if (!attrib.normalized)
								{
									m_context.glVertexAttribIPointer(bindingIndex, attrib.size, attrib.type, attrib.stride, attrib.pointer);
									break;
								}
								else
									[[fallthrough]];

							default:
								m_context.glVertexAttribPointer(bindingIndex, attrib.size, attrib.type, attrib.normalized, attrib.stride, attrib.pointer);
								break;
						}
					}

					bindingIndex++;
				}
			});

			it = m_vertexArrays.emplace(setup, std::make_unique<VertexArray>(std::move(vao))).first;
		}

		return *(it->second);
	}

	void OpenGLVaoCache::NotifyBufferDestruction(GLuint buffer) const
	{
		for (auto it = m_vertexArrays.begin(); it != m_vertexArrays.end();)
		{
			const OpenGLVaoSetup& setup = it->first;

			// If a VAO is based on at least one of these buffers, delete it
			auto FindVertexBuffer = [&](const auto& attribOpt)
			{
				if (!attribOpt)
					return false;

				return attribOpt->vertexBuffer == buffer;
			};

			if (setup.indexBuffer == buffer || std::any_of(setup.vertexAttribs.begin(), setup.vertexAttribs.end(), FindVertexBuffer))
				it = m_vertexArrays.erase(it);
			else
				++it;
		}
	}
}
