// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Hash.hpp>

namespace Nz::GL
{
	inline bool operator==(const OpenGLVaoSetup& lhs, const OpenGLVaoSetup& rhs)
	{
		if (lhs.indexBuffer != rhs.indexBuffer)
			return false;

		const auto& compareAttribs = [](const auto& lAttribOpt, const auto& rAttribOpt)
		{
			if (lAttribOpt.has_value() != rAttribOpt.has_value())
				return false;

			if (lAttribOpt)
			{
				const auto& lAttrib = *lAttribOpt;
				const auto& rAttrib = *rAttribOpt;

				if (lAttrib.vertexBuffer != rAttrib.vertexBuffer)
					return false;

				if (lAttrib.size != rAttrib.size)
					return false;

				if (lAttrib.type != rAttrib.type)
					return false;

				if (lAttrib.normalized != rAttrib.normalized)
					return false;

				if (lAttrib.stride != rAttrib.stride)
					return false;

				if (lAttrib.pointer != rAttrib.pointer)
					return false;
			}


			return true;
		};

		return std::equal(lhs.vertexAttribs.begin(), lhs.vertexAttribs.end(), rhs.vertexAttribs.begin(), compareAttribs);
	}

	inline std::size_t OpenGLVaoSetupHasher::operator()(const OpenGLVaoSetup& setup) const
	{
		std::size_t seed = std::hash<GLuint>{}(setup.indexBuffer);

		std::size_t bindingIndex = 0;
		for (const auto& attribOpt : setup.vertexAttribs)
		{
			if (attribOpt)
			{
				const auto& attrib = attribOpt.value();

				HashCombine(seed, bindingIndex);
				HashCombine(seed, attrib.vertexBuffer);
				HashCombine(seed, attrib.size);
				HashCombine(seed, attrib.type);
				HashCombine(seed, attrib.normalized);
				HashCombine(seed, attrib.stride);
				HashCombine(seed, attrib.pointer);
			}

			bindingIndex++;
		}

		return seed;
	}
}
