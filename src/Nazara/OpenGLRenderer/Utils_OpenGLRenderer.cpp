// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	std::string TranslateOpenGLError(GLenum code)
	{
		switch (code)
		{
			// OpenGL/OpenGL ES error codes
			case GL_INVALID_ENUM:
				return "an unacceptable value is specified for an enumerated argument";
				break;

			case GL_INVALID_VALUE:
				return "a numeric argument is out of range";
				break;

			case GL_INVALID_OPERATION:
				return "the specified operation is not allowed in the current state";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "the framebuffer object is not complete";
				break;

			case GL_OUT_OF_MEMORY:
				return "there is not enough memory left to execute the command";
				break;

				// OpenGL error codes
			case GL_STACK_UNDERFLOW:
				return "an attempt has been made to perform an operation that would cause an internal stack to underflow.";
				break;

			case GL_STACK_OVERFLOW:
				return "an attempt has been made to perform an operation that would cause an internal stack to overflow.";
				break;

		}

		return "Unknown OpenGL error (0x" + NumberToString(code, 16) + ')';
	}
}
