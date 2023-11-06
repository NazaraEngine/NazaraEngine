// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/PipelinePassListLoader.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <optional>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz::Loaders
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		class PassListLoader
		{
			public:
				PassListLoader(Stream& stream, const PipelinePassListParams& /*parameters*/) :
				m_stream(stream)
				{
				}

				Result<std::shared_ptr<PipelinePassList>, ResourceLoadingError> Parse()
				{
					try
					{
						ExpectKeyword("passlist");

						std::string passListName = ReadString();

						m_current.emplace();
						m_current->passList = std::make_shared<PipelinePassList>();
						Block([this]
						{
							std::string kw = ReadKeyword();
							if (kw == "attachment")
								HandleAttachment();
							else if (kw == "pass")
								HandlePass();
							else if (kw == "output")
							{
								std::string attachmentName = ReadString();

								auto it = m_current->attachmentsByName.find(attachmentName);
								if (it == m_current->attachmentsByName.end())
								{
									NazaraErrorFmt("unknown attachment {}", attachmentName);
									throw ResourceLoadingError::DecodingError;
								}

								m_current->passList->SetFinalOutput(it->second);
							}
							else
							{
								NazaraErrorFmt("unexpected keyword {}", kw);
								throw ResourceLoadingError::DecodingError;
							}
						});

						return Ok(std::move(m_current->passList));
					}
					catch (ResourceLoadingError error)
					{
						return Err(error);
					}
				}

			private:
				void Block(const FunctionRef<void()>& callback)
				{
					std::string beginToken = ReadKeyword();
					if (beginToken != "{")
					{
						NazaraErrorFmt("expected \"{{\" token, got {}", beginToken);
						throw ResourceLoadingError::DecodingError;
					}

					for (;;)
					{
						std::string nextKeyword = ReadKeyword(true);
						if (nextKeyword == "}")
							break;

						callback();
					}

					std::string endToken = ReadKeyword();
					if (endToken != "}")
					{
						NazaraErrorFmt("expected \"}}\" token, got {}", endToken);
						throw ResourceLoadingError::DecodingError;
					}
				}

				void ExpectKeyword(std::string_view expectedKeyword)
				{
					std::string keyword = ReadKeyword();
					if (keyword != expectedKeyword)
					{
						NazaraErrorFmt("expected \"{}\" keyword, got {}", expectedKeyword, keyword);
						throw ResourceLoadingError::DecodingError;
					}
				}

				void HandleAttachment()
				{
					std::string attachmentName = ReadString();

					std::string format;
					Block([&]
					{
						std::string kw = ReadKeyword();
						if (kw == "format")
							format = ReadString();
						else
						{
							NazaraErrorFmt("unexpected keyword {}", kw);
							throw ResourceLoadingError::DecodingError;
						}
					});

					if (format.empty())
					{
						NazaraErrorFmt("missing mandatory format in attachment {}", attachmentName);
						throw ResourceLoadingError::DecodingError;
					}

					PixelFormat attachmentFormat = PixelFormat::Undefined;
					if (format == "PreferredDepth")
						attachmentFormat = Graphics::Instance()->GetPreferredDepthFormat();
					else if (format == "PreferredDepthStencil")
						attachmentFormat = Graphics::Instance()->GetPreferredDepthStencilFormat();
					else
						attachmentFormat = PixelFormatInfo::IdentifyFormat(format);

					if (attachmentFormat == PixelFormat::Undefined)
					{
						NazaraErrorFmt("unknown format {}", format);
						throw ResourceLoadingError::DecodingError;
					}

					if (m_current->attachmentsByName.find(attachmentName) != m_current->attachmentsByName.end())
					{
						NazaraErrorFmt("attachment {} already exists", attachmentName);
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t attachmentId = m_current->passList->AddAttachment({
						attachmentName,
						attachmentFormat
					});

					m_current->attachmentsByName.emplace(attachmentName, attachmentId);
				}
				
				void HandlePass()
				{
					std::string passName = ReadString();

					struct InputOutput
					{
						std::string name;
						std::string attachmentName;
					};

					ParameterList implConfig;
					std::string impl;
					std::string depthstencilInput;
					std::string depthstencilOutput;
					std::vector<InputOutput> inputs;
					std::vector<InputOutput> outputs;
					std::vector<std::string> flags;

					Block([&]
					{
						std::string kw = ReadKeyword();

						if (kw == "impl")
						{
							impl = ReadString();

							std::string nextKeyword = ReadKeyword(true);
							if (nextKeyword == "{")
							{
								Block([&]
								{
									std::string key = ReadKeyword();
									std::string value = ReadString();

									implConfig.SetParameter(std::move(key), std::move(value));
								});
							}
						}
						else if (kw == "depthstencilinput")
							depthstencilInput = ReadString();
						else if (kw == "depthstenciloutput")
							depthstencilOutput = ReadString();
						else if (kw == "flag")
							flags.push_back(ReadString());
						else if (kw == "input")
						{
							std::string name = ReadString();
							std::string attachment = ReadString();

							inputs.push_back({
								std::move(name),
								std::move(attachment),
							});
						}
						else if (kw == "output")
						{
							std::string name = ReadString();
							std::string attachment = ReadString();

							outputs.push_back({
								std::move(name),
								std::move(attachment),
							});
						}
						else
						{
							NazaraErrorFmt("unexpected keyword {}", kw);
							throw ResourceLoadingError::DecodingError;
						}
					});

					FramePipelinePassRegistry& passRegistry = Graphics::Instance()->GetFramePipelinePassRegistry();

					std::size_t implIndex = passRegistry.GetPassIndex(impl);
					if (implIndex == FramePipelinePassRegistry::InvalidIndex)
					{
						NazaraErrorFmt("unknown pass {}", impl);
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t passId = m_current->passList->AddPass(passName, implIndex, std::move(implConfig));

					for (auto&& [inputName, attachmentName] : inputs)
					{
						std::size_t inputIndex = passRegistry.GetPassInputIndex(implIndex, inputName);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraErrorFmt("pass {} has no input {}", impl, inputName);
							throw ResourceLoadingError::DecodingError;
						}

						auto it = m_current->attachmentsByName.find(attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", attachmentName);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassInput(passId, inputIndex, it->second);
					}

					for (auto&& [outputName, attachmentName] : outputs)
					{
						std::size_t inputIndex = passRegistry.GetPassOutputIndex(implIndex, outputName);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraErrorFmt("pass {} has no output {}", impl, outputName);
							throw ResourceLoadingError::DecodingError;
						}

						auto it = m_current->attachmentsByName.find(attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", attachmentName);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassOutput(passId, inputIndex, it->second);
					}

					if (!depthstencilInput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilInput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", depthstencilInput);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassDepthStencilInput(passId, it->second);
					}

					if (!depthstencilOutput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilOutput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", depthstencilOutput);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassDepthStencilOutput(passId, it->second);
					}

					for (const auto& flagStr : flags)
					{
						if (flagStr == "LightShadowing")
							m_current->passList->EnablePassFlags(passId, FramePipelinePassFlag::LightShadowing);
						else
						{
							NazaraErrorFmt("unknown pass flag {}", flagStr);
							throw ResourceLoadingError::DecodingError;
						}
					}
				}

				std::string ReadKeyword(bool peek = false)
				{
					std::size_t beginOffset;
					do
					{
						EnsureLine();
						beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
					}
					while (beginOffset == m_currentLine.npos);

					if (m_currentLine[beginOffset] == '"')
					{
						NazaraError("expected a keyword, got a string");
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t endOffset = m_currentLine.find_first_of(" \r\t\n", beginOffset + 1);
					if (endOffset == m_currentLine.npos)
						endOffset = m_currentLine.size();

					std::string currentToken = std::string(m_currentLine.substr(beginOffset, endOffset - beginOffset));
					if (!peek)
						m_currentLine.erase(m_currentLine.begin(), m_currentLine.begin() + endOffset);

					return currentToken;
				}

				std::string ReadString()
				{
					std::size_t beginOffset;
					do
					{
						EnsureLine();
						beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
					}
					while (beginOffset == m_currentLine.npos);

					if (m_currentLine[beginOffset] != '"')
					{
						NazaraError("expected a string, got a keyword");
						throw ResourceLoadingError::DecodingError;
					}

					std::string str;
					for (std::size_t i = beginOffset + 1; i < m_currentLine.size(); ++i)
					{
						switch (m_currentLine[i])
						{
						case '\0':
						case '\n':
						case '\r':
							NazaraError("unfinished string");
							throw ResourceLoadingError::DecodingError;

						case '"':
						{
							m_currentLine.erase(m_currentLine.begin(), m_currentLine.begin() + beginOffset + i);

							return str;
						}

						case '\\':
						{
							i++;
							char character;
							switch (m_currentLine[i])
							{
								case 'n': character = '\n'; break;
								case 'r': character = '\r'; break;
								case 't': character = '\t'; break;
								case '"': character = '"'; break;
								case '\\': character = '\\'; break;
								default:
									NazaraErrorFmt("unrecognized character {}", character);
									throw ResourceLoadingError::DecodingError;
							}

							str.push_back(character);
							break;
						}

						default:
							str.push_back(m_currentLine[i]);
						}
					}

					NazaraError("unfinished string");
					throw ResourceLoadingError::DecodingError;
				}

				void EnsureLine()
				{
					while (m_currentLine.find_first_not_of(" \r\t\n") == m_currentLine.npos)
					{
						m_currentLine.clear();
						m_stream.ReadLine(m_currentLine);
						if (m_currentLine.empty())
							throw ResourceLoadingError::DecodingError;
					}
				}

				struct CurrentPassList
				{
					std::shared_ptr<PipelinePassList> passList;
					std::unordered_map<std::string /*attachmentName*/, std::size_t /*attachmentId*/> attachmentsByName;
				};

				std::optional<CurrentPassList> m_current;
				std::string m_currentLine;

				Stream& m_stream;
		};
	}

	PipelinePassListLoader::Entry GetPipelinePassListLoader()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		PipelinePassListLoader::Entry entry;
		entry.extensionSupport = [](std::string_view ext) { return ext == ".passlist"; };
		entry.streamLoader = [](Stream& stream, const PipelinePassListParams& parameters)
		{
			PassListLoader passListLoader(stream, parameters);
			return passListLoader.Parse();
		};

		return entry;
	}
}
