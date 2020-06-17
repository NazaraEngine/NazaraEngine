// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GlslWriter.hpp>
#include <type_traits>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	template<typename T>
	void GlslWriter::Append(const T& param)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << param;
	}

	template<typename T>
	void GlslWriter::DeclareVariables(const std::vector<T>& variables, const std::string& keyword, const std::string& section)
	{
		if (!variables.empty())
		{
			if (!section.empty())
				AppendCommentSection(section);

			for (const auto& var : variables)
			{
				if constexpr (std::is_same_v<T, ShaderAst::InputOutput>)
				{
					if (var.locationIndex)
					{
						Append("layout(location = ");
						Append(*var.locationIndex);
						Append(") ");
					}
				}
				else if constexpr (std::is_same_v<T, ShaderAst::Uniform>)
				{
					if (var.bindingIndex)
					{
						Append("layout(binding = ");
						Append(*var.bindingIndex);
						Append(") ");
					}
				}

				if (!keyword.empty())
				{
					Append(keyword);
					Append(" ");
				}

				Append(var.type);
				Append(" ");
				Append(var.name);
				AppendLine(";");
			}

			AppendLine();
		}
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
