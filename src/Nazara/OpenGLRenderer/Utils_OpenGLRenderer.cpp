// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Format.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	std::string TranslateOpenGLError(GLenum code)
	{
		switch (code)
		{
			// OpenGL/OpenGL ES error codes
			case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM: an unacceptable value is specified for an enumerated argument";
			case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE: a numeric argument is out of range";
			case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION: the specified operation is not allowed in the current state";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION: the framebuffer object is not complete";
			case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY: there is not enough memory left to execute the command";

			// OpenGL error codes
			case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW: an attempt has been made to perform an operation that would cause an internal stack to underflow";
			case GL_STACK_OVERFLOW:  return "GL_STACK_OVERFLOW: an attempt has been made to perform an operation that would cause an internal stack to overflow";

			// Framebuffer error codes
			case GL_FRAMEBUFFER_UNDEFINED:                     return "GL_FRAMEBUFFER_UNDEFINED: default framebuffer does not exist";
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: some framebuffer attachment points are incomplete";
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: framebuffer has no image attached to it";
			case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED: framebuffer internal formats violates an implementation-dependent set of restrictions";
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: framebuffer mixes multiple samples size for attachements";
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: a framebuffer attachment is layered and a populated attachment is not (or color attachements are not from textures of the same target)";
		}

		return Format("unknown OpenGL error ({0:#x})", code);
	}
}
