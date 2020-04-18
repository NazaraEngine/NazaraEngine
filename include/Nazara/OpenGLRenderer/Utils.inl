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
