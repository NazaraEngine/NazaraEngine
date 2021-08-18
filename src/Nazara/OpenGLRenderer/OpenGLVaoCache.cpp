// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/VertexArray.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

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
						m_context.glVertexAttribPointer(bindingIndex, attrib.size, attrib.type, attrib.normalized, attrib.stride, attrib.pointer);
					}

					bindingIndex++;
				}
			});

			it = m_vertexArrays.emplace(setup, std::make_unique<VertexArray>(std::move(vao))).first;
		}

		return *(it->second);
	}

	void OpenGLVaoCache::NotifyBufferDestruction(GLuint buffer)
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
