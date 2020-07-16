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
	void GlslWriter::DeclareVariables(const ShaderAst& shader, const std::vector<T>& variables, const std::string& keyword, const std::string& section)
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
				else if constexpr (std::is_same_v<T, ShaderAst::Uniform>)
				{
					if (var.bindingIndex || var.memoryLayout)
					{
						Append("layout(");

						bool first = true;
						if (var.bindingIndex)
						{
							if (!first)
								Append(", ");

							Append("binding = ");
							Append(*var.bindingIndex);

							first = false;
						}

						if (var.memoryLayout)
						{
							if (!first)
								Append(", ");

							Append(*var.memoryLayout);

							first = false;
						}

						Append(") ");
					}

					if (!keyword.empty())
					{
						Append(keyword);
						Append(" ");
					}

					std::visit([&](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;
						if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
						{
							Append(arg);
							Append(" ");
							Append(var.name);
						}
						else if constexpr (std::is_same_v<T, std::string>)
						{
							const auto& structs = shader.GetStructs();
							auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == arg; });
							if (it == structs.end())
								throw std::runtime_error("struct " + arg + " has not been defined");

							const auto& s = *it;

							AppendLine(var.name + "_interface");
							AppendLine("{");
							for (const auto& m : s.members)
							{
								Append("\t");
								Append(m.type);
								Append(" ");
								Append(m.name);
								AppendLine(";");
							}
							Append("} ");
							Append(var.name);
						}
						else
							static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

					}, var.type);

					AppendLine(";");
					AppendLine();
				}
			}

			AppendLine();
		}
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
