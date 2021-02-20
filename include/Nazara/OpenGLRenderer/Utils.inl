// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline std::optional<GLTextureFormat> DescribeTextureFormat(PixelFormat pixelFormat)
	{
		// TODO: Fill this switch
		switch (pixelFormat)
		{
			case PixelFormat_A8:              return GLTextureFormat{ GL_R8,               GL_RED,           GL_UNSIGNED_BYTE,     GL_ZERO,  GL_ZERO,  GL_ZERO, GL_RED };
			case PixelFormat_BGR8:            return GLTextureFormat{ GL_RGB8,              GL_RGB,           GL_UNSIGNED_BYTE,     GL_BLUE,  GL_GREEN, GL_RED,  GL_ONE };
			case PixelFormat_BGR8_SRGB:       return GLTextureFormat{ GL_SRGB8,             GL_RGB,           GL_UNSIGNED_BYTE,     GL_BLUE,  GL_GREEN, GL_RED,  GL_ONE };
			case PixelFormat_BGRA8:           return GLTextureFormat{ GL_SRGB8_ALPHA8,      GL_RGBA,          GL_UNSIGNED_BYTE,     GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat_BGRA8_SRGB:      return GLTextureFormat{ GL_SRGB8_ALPHA8,      GL_RGBA,          GL_UNSIGNED_BYTE,     GL_BLUE,  GL_GREEN, GL_RED,  GL_ALPHA };
			case PixelFormat_Depth24Stencil8: return GLTextureFormat{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_RED,   GL_GREEN, GL_ZERO, GL_ZERO };
			case PixelFormat_RGB8:            return GLTextureFormat{ GL_RGB8,              GL_RGB,           GL_UNSIGNED_BYTE,     GL_RED,   GL_GREEN, GL_BLUE, GL_ONE };
			case PixelFormat_RGB8_SRGB:       return GLTextureFormat{ GL_SRGB8,             GL_RGB,           GL_UNSIGNED_BYTE,     GL_RED,   GL_GREEN, GL_BLUE, GL_ONE };
			case PixelFormat_RGBA8:           return GLTextureFormat{ GL_RGBA8,             GL_RGBA,          GL_UNSIGNED_BYTE,     GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			case PixelFormat_RGBA8_SRGB:      return GLTextureFormat{ GL_SRGB8_ALPHA8,      GL_RGBA,          GL_UNSIGNED_BYTE,     GL_RED,   GL_GREEN, GL_BLUE, GL_ALPHA };
			default: break;
		}

		NazaraError("Unhandled PixelFormat 0x" + NumberToString(UnderlyingCast(pixelFormat), 16));
		return {};
	}

	inline GLenum ToOpenGL(BlendFunc blendFunc)
	{
		switch (blendFunc)
		{
			case BlendFunc_DestAlpha:    return GL_DST_ALPHA;
			case BlendFunc_DestColor:    return GL_DST_COLOR;
			case BlendFunc_SrcAlpha:     return GL_SRC_ALPHA;
			case BlendFunc_SrcColor:     return GL_SRC_COLOR;
			case BlendFunc_InvDestAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case BlendFunc_InvDestColor: return GL_ONE_MINUS_DST_COLOR;
			case BlendFunc_InvSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFunc_InvSrcColor:  return GL_ONE_MINUS_SRC_COLOR;
			case BlendFunc_One:          return GL_ONE;
			case BlendFunc_Zero:         return GL_ZERO;
		}

		NazaraError("Unhandled BlendFunc 0x" + NumberToString(UnderlyingCast(blendFunc), 16));
		return {};
	}

	inline GLenum ToOpenGL(FaceSide filter)
	{
		switch (filter)
		{
			case FaceSide_None:
				break;

			case FaceSide_Back:         return GL_BACK;
			case FaceSide_Front:        return GL_FRONT;
			case FaceSide_FrontAndBack: return GL_FRONT_AND_BACK;
		}

		NazaraError("Unhandled FaceSide 0x" + NumberToString(UnderlyingCast(filter), 16));
		return {};
	}

	GLenum ToOpenGL(PrimitiveMode primitiveMode)
	{
		switch (primitiveMode)
		{
			case PrimitiveMode_LineList:      return GL_LINES;
			case PrimitiveMode_LineStrip:     return GL_LINE_STRIP;
			case PrimitiveMode_PointList:     return GL_POINTS;
			case PrimitiveMode_TriangleList:  return GL_TRIANGLES;
			case PrimitiveMode_TriangleStrip: return GL_TRIANGLE_STRIP;
			case PrimitiveMode_TriangleFan:   return GL_TRIANGLE_FAN;
		}

		NazaraError("Unhandled PrimitiveMode 0x" + NumberToString(UnderlyingCast(primitiveMode), 16));
		return {};
	}

	inline GLenum ToOpenGL(RendererComparison comparison)
	{
		switch (comparison)
		{
			case RendererComparison_Always:         return GL_ALWAYS;
			case RendererComparison_Equal:          return GL_EQUAL;
			case RendererComparison_Greater:        return GL_GREATER;
			case RendererComparison_GreaterOrEqual: return GL_GEQUAL;
			case RendererComparison_Less:           return GL_LESS;
			case RendererComparison_LessOrEqual:    return GL_LEQUAL;
			case RendererComparison_Never:          return GL_NEVER;
			case RendererComparison_NotEqual:       return GL_NOTEQUAL;
		}

		NazaraError("Unhandled RendererComparison 0x" + NumberToString(UnderlyingCast(comparison), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerFilter filter)
	{
		switch (filter)
		{
			case SamplerFilter::SamplerFilter_Linear:  return GL_LINEAR;
			case SamplerFilter::SamplerFilter_Nearest: return GL_NEAREST;
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(filter), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerFilter minFilter, SamplerMipmapMode mipmapFilter)
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

				NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(mipmapFilter), 16));
				return {};
			}

			case SamplerFilter::SamplerFilter_Nearest:
			{
				switch (mipmapFilter)
				{
					case SamplerMipmapMode_Linear: return GL_NEAREST_MIPMAP_LINEAR;
					case SamplerMipmapMode_Nearest: return GL_NEAREST_MIPMAP_NEAREST;
				}

				NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(mipmapFilter), 16));
				return {};
			}
		}

		NazaraError("Unhandled SamplerFilter 0x" + NumberToString(UnderlyingCast(minFilter), 16));
		return {};
	}

	inline GLenum ToOpenGL(SamplerWrap wrapMode)
	{
		switch (wrapMode)
		{
			case SamplerWrap::SamplerWrap_Clamp:          return GL_CLAMP_TO_EDGE;
			case SamplerWrap::SamplerWrap_MirroredRepeat: return GL_MIRRORED_REPEAT;
			case SamplerWrap::SamplerWrap_Repeat:         return GL_REPEAT;
		}

		NazaraError("Unhandled SamplerWrap 0x" + NumberToString(UnderlyingCast(wrapMode), 16));
		return {};
	}

	inline GLenum ToOpenGL(ShaderStageType stageType)
	{
		switch (stageType)
		{
			case ShaderStageType::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderStageType::Vertex:   return GL_VERTEX_SHADER;
		}

		NazaraError("Unhandled ShaderStageType 0x" + NumberToString(UnderlyingCast(stageType), 16));
		return {};
	}

	inline GLenum ToOpenGL(StencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case StencilOperation_Decrement:        return GL_DECR;
			case StencilOperation_DecrementNoClamp: return GL_DECR_WRAP;
			case StencilOperation_Increment:        return GL_INCR;
			case StencilOperation_IncrementNoClamp: return GL_INCR_WRAP;
			case StencilOperation_Invert:           return GL_INVERT;
			case StencilOperation_Keep:             return GL_KEEP;
			case StencilOperation_Replace:          return GL_REPLACE;
			case StencilOperation_Zero:             return GL_ZERO;
		}

		NazaraError("Unhandled StencilOperation 0x" + NumberToString(UnderlyingCast(stencilOp), 16));
		return {};
	}

	inline GLenum ToOpenGL(GL::BufferTarget bufferTarget)
	{
		switch (bufferTarget)
		{
			case GL::BufferTarget::Array:             return GL_ARRAY_BUFFER;
			case GL::BufferTarget::CopyRead:          return GL_COPY_READ_BUFFER;
			case GL::BufferTarget::CopyWrite:         return GL_COPY_WRITE_BUFFER;
			case GL::BufferTarget::ElementArray:      return GL_ELEMENT_ARRAY_BUFFER;
			case GL::BufferTarget::PixelPack:         return GL_PIXEL_PACK_BUFFER;
			case GL::BufferTarget::PixelUnpack:       return GL_PIXEL_UNPACK_BUFFER;
			case GL::BufferTarget::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
			case GL::BufferTarget::Uniform:           return GL_UNIFORM_BUFFER;
		}

		NazaraError("Unhandled GL::BufferTarget 0x" + NumberToString(UnderlyingCast(bufferTarget), 16));
		return {};
	}

	inline GLenum ToOpenGL(GL::TextureTarget textureTarget)
	{
		switch (textureTarget)
		{
			case GL::TextureTarget::Cubemap:        return GL_TEXTURE_CUBE_MAP;
			case GL::TextureTarget::Target2D:       return GL_TEXTURE_2D;
			case GL::TextureTarget::Target2D_Array: return GL_TEXTURE_2D_ARRAY;
			case GL::TextureTarget::Target3D:       return GL_TEXTURE_3D;
		}

		NazaraError("Unhandled GL::TextureTarget 0x" + NumberToString(UnderlyingCast(textureTarget), 16));
		return {};
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
