// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	GLenum ToOpenGL(SamplerFilter filter)
	{
		switch (filter)
		{
			case SamplerFilter::SamplerFilter_Linear:  return GL_LINEAR;
			case SamplerFilter::SamplerFilter_Nearest: return GL_NEAREST;
		}

		NazaraError("Unhandled SamplerFilter 0x" + String::Number(UnderlyingCast(filter), 16));
		return {};
	}

	GLenum ToOpenGL(SamplerFilter minFilter, SamplerMipmapMode mipmapFilter)
	{
		switch (minFilter)
		{
			case SamplerFilter::SamplerFilter_Linear:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode_Linear: return GL_LINEAR_MIPMAP_LINEAR;
					case SamplerMipmapMode_Nearest: return GL_LINEAR_MIPMAP_NEAREST;
				}

				NazaraError("Unhandled SamplerFilter 0x" + String::Number(UnderlyingCast(mipmapFilter), 16));
				return {};
			}

			case SamplerFilter::SamplerFilter_Nearest:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode_Linear: return GL_NEAREST_MIPMAP_LINEAR;
					case SamplerMipmapMode_Nearest: return GL_NEAREST_MIPMAP_NEAREST;
				}

				NazaraError("Unhandled SamplerFilter 0x" + String::Number(UnderlyingCast(mipmapFilter), 16));
				return {};
			}
		}

		NazaraError("Unhandled SamplerFilter 0x" + String::Number(UnderlyingCast(minFilter), 16));
		return {};
	}

	GLenum ToOpenGL(SamplerWrap wrapMode)
	{
		switch (wrapMode)
		{
			case SamplerWrap::SamplerWrap_Clamp:          return GL_CLAMP_TO_EDGE;
			case SamplerWrap::SamplerWrap_MirroredRepeat: return GL_MIRRORED_REPEAT;
			case SamplerWrap::SamplerWrap_Repeat:         return GL_REPEAT;
		}

		NazaraError("Unhandled SamplerWrap 0x" + String::Number(UnderlyingCast(wrapMode), 16));
		return {};
	}

	GLenum ToOpenGL(ShaderStageType stageType)
	{
		switch (stageType)
		{
			case ShaderStageType::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderStageType::Vertex:   return GL_VERTEX_SHADER;
		}

		NazaraError("Unhandled ShaderStageType 0x" + String::Number(UnderlyingCast(stageType), 16));
		return {};
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
