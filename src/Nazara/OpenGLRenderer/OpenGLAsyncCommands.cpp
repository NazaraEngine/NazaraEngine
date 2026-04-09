// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLAsyncCommands.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>

namespace Nz
{
	void OpenGLAsyncCommands::AddCommands(Nz::FunctionRef<void(CommandBufferBuilder& builder)> callback)
	{
		OpenGLCommandBufferBuilder builder(m_commandBuffer);
		callback(builder);
	}

	void OpenGLAsyncCommands::Execute()
	{
		m_commandBuffer.Execute();
	}
}
