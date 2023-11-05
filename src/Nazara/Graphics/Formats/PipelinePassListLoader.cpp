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
					auto result = ExpectKeyword("passlist");
					if (!result)
						return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

					Result<std::string, ResourceLoadingError> passListName = GetString();
					if (!passListName)
						return Err(std::move(passListName).GetError()); //< FIXME: why rvalue is needed

					m_current.emplace();
					m_current->passList = std::make_shared<PipelinePassList>();
					result = Block([this]() -> Result<void, ResourceLoadingError>
					{
						Result<std::string, ResourceLoadingError> kw = GetKeyword();
						if (!kw)
							return Err(std::move(kw).GetError()); //< FIXME: why rvalue is needed

						if (kw.GetValue() == "attachment")
						{
							auto result = HandleAttachment();
							if (!result)
								return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed
						}
						else if (kw.GetValue() == "pass")
						{
							auto result = HandlePass();
							if (!result)
								return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed
						}
						else if (kw.GetValue() == "output")
						{
							Result<std::string, ResourceLoadingError> attachmentName = GetString();
							if (!attachmentName)
								return Err(std::move(attachmentName).GetError()); //< FIXME: why rvalue is needed

							auto it = m_current->attachmentsByName.find(attachmentName.GetValue());
							if (it == m_current->attachmentsByName.end())
							{
								NazaraErrorFmt("unknown attachment {}", attachmentName.GetValue());
								return Err(ResourceLoadingError::DecodingError);
							}

							m_current->passList->SetFinalOutput(it->second);
						}
						else
						{
							NazaraErrorFmt("unexpected keyword {}", kw.GetValue());
							return Err(ResourceLoadingError::DecodingError);
						}

						return Ok();
					});
					if (!result)
						return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

					return Ok(std::move(m_current->passList));
				}

			private:
				Result<void, ResourceLoadingError> Block(const FunctionRef<Result<void, ResourceLoadingError>()>& callback)
				{
					auto beginBlock = GetKeyword();
					if (!beginBlock)
						return Err(std::move(beginBlock).GetError()); //< FIXME: why rvalue is needed

					if (beginBlock.GetValue() != "{")
					{
						NazaraErrorFmt("expected \"{{\" token, got {}", beginBlock.GetValue());
						return Err(ResourceLoadingError::DecodingError);
					}

					for (;;)
					{
						auto nextKw = GetKeyword(true);
						if (!nextKw)
							return Err(std::move(nextKw).GetError()); //< FIXME: why rvalue is needed

						if (nextKw.GetValue() == "}")
							break;

						auto result = callback();
						if (!result)
							return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed
					}

					auto endBlock = GetKeyword();
					if (!endBlock)
						return Err(std::move(endBlock).GetError()); //< FIXME: why rvalue is needed

					if (endBlock.GetValue() != "}")
					{
						NazaraErrorFmt("expected \"}}\" token, got {}", endBlock.GetValue());
						return Err(ResourceLoadingError::DecodingError);
					}

					return Ok();
				}

				Result<void, ResourceLoadingError> ExpectKeyword(std::string_view expectedKeyword)
				{
					Result<std::string, ResourceLoadingError> passListKw = GetKeyword();
					if (!passListKw)
						return Err(std::move(passListKw).GetError()); //< FIXME: why rvalue is needed

					if (passListKw.GetValue() != expectedKeyword)
					{
						NazaraErrorFmt("expected \"{}\" keyword, got {}", expectedKeyword, passListKw.GetValue());
						return Err(ResourceLoadingError::DecodingError);
					}

					return Ok();
				}

				Result<void, ResourceLoadingError> HandleAttachment()
				{
					Result<std::string, ResourceLoadingError> attachmentName = GetString();
					if (!attachmentName)
						return Err(std::move(attachmentName).GetError()); //< FIXME: why rvalue is needed

					std::string format;
					auto result = Block([&]() -> Result<void, ResourceLoadingError>
					{
						Result<std::string, ResourceLoadingError> kw = GetKeyword();
						if (!kw)
							return Err(std::move(kw).GetError()); //< FIXME: why rvalue is needed

						if (kw.GetValue() == "format")
						{
							Result<std::string, ResourceLoadingError> formatStr = GetString();
							if (!formatStr)
								return Err(std::move(formatStr).GetError()); //< FIXME: why rvalue is needed

							format = std::move(formatStr).GetValue();
						}
						else
						{
							NazaraErrorFmt("unexpected keyword {}", kw.GetValue());
							return Err(ResourceLoadingError::DecodingError);
						}

						return Ok();
					});
					if (!result)
						return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

					if (format.empty())
					{
						NazaraErrorFmt("missing mandatory format in attachment {}", attachmentName.GetValue());
						return Err(ResourceLoadingError::DecodingError);
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
						return Err(ResourceLoadingError::DecodingError);
					}

					if (m_current->attachmentsByName.find(attachmentName.GetValue()) != m_current->attachmentsByName.end())
					{
						NazaraErrorFmt("attachment {} already exists", attachmentName.GetValue());
						return Err(ResourceLoadingError::DecodingError);
					}

					std::size_t attachmentId = m_current->passList->AddAttachment({
						attachmentName.GetValue(),
						attachmentFormat
					});

					m_current->attachmentsByName.emplace(attachmentName.GetValue(), attachmentId);

					return Ok();
				}
				
				Result<void, ResourceLoadingError> HandlePass()
				{
					Result<std::string, ResourceLoadingError> passName = GetString();
					if (!passName)
						return Err(std::move(passName).GetError()); //< FIXME: why rvalue is needed

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

					auto result = Block([&]() -> Result<void, ResourceLoadingError>
					{
						Result<std::string, ResourceLoadingError> kw = GetKeyword();
						if (!kw)
							return Err(std::move(kw).GetError()); //< FIXME: why rvalue is needed

						if (kw.GetValue() == "impl")
						{
							Result<std::string, ResourceLoadingError> implStr = GetString();
							if (!implStr)
								return Err(std::move(implStr).GetError()); //< FIXME: why rvalue is needed

							impl = std::move(implStr).GetValue();

							auto nextKw = GetKeyword(true);
							if (!nextKw)
								return Err(std::move(nextKw).GetError()); //< FIXME: why rvalue is needed

							if (nextKw.GetValue() == "{")
							{
								Block([&]() -> Result<void, ResourceLoadingError>
								{
									Result<std::string, ResourceLoadingError> key = GetKeyword();
									if (!key)
										return Err(std::move(key).GetError()); //< FIXME: why rvalue is needed

									Result<std::string, ResourceLoadingError> value = GetString();
									if (!value)
										return Err(std::move(value).GetError()); //< FIXME: why rvalue is needed

									implConfig.SetParameter(key.GetValue(), value.GetValue());
									return Ok();
								});
							}
						}
						else if (kw.GetValue() == "depthstencilinput")
						{
							Result<std::string, ResourceLoadingError> attachmentStr = GetString();
							if (!attachmentStr)
								return Err(std::move(attachmentStr).GetError()); //< FIXME: why rvalue is needed

							depthstencilInput = std::move(attachmentStr).GetValue();
						}
						else if (kw.GetValue() == "depthstenciloutput")
						{
							Result<std::string, ResourceLoadingError> attachmentStr = GetString();
							if (!attachmentStr)
								return Err(std::move(attachmentStr).GetError()); //< FIXME: why rvalue is needed

							depthstencilOutput = std::move(attachmentStr).GetValue();
						}
						else if (kw.GetValue() == "flag")
						{
							Result<std::string, ResourceLoadingError> str = GetString();
							if (!str)
								return Err(std::move(str).GetError()); //< FIXME: why rvalue is needed

							flags.push_back(std::move(str).GetValue());
						}
						else if (kw.GetValue() == "input")
						{
							Result<std::string, ResourceLoadingError> name = GetString();
							if (!name)
								return Err(std::move(name).GetError()); //< FIXME: why rvalue is needed

							Result<std::string, ResourceLoadingError> attachment = GetString();
							if (!attachment)
								return Err(std::move(attachment).GetError()); //< FIXME: why rvalue is needed

							inputs.push_back({
								std::move(name).GetValue(),
								std::move(attachment).GetValue(),
							});
						}
						else if (kw.GetValue() == "output")
						{
							Result<std::string, ResourceLoadingError> name = GetString();
							if (!name)
								return Err(std::move(name).GetError()); //< FIXME: why rvalue is needed

							Result<std::string, ResourceLoadingError> attachment = GetString();
							if (!attachment)
								return Err(std::move(attachment).GetError()); //< FIXME: why rvalue is needed

							outputs.push_back({
								std::move(name).GetValue(),
								std::move(attachment).GetValue(),
							});
						}
						else
						{
							NazaraErrorFmt("unexpected keyword {}", kw.GetValue());
							return Err(ResourceLoadingError::DecodingError);
						}

						return Ok();
					});
					if (!result)
						return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

					FramePipelinePassRegistry& passRegistry = Graphics::Instance()->GetFramePipelinePassRegistry();

					std::size_t implIndex = passRegistry.GetPassIndex(impl);
					if (implIndex == FramePipelinePassRegistry::InvalidIndex)
					{
						NazaraErrorFmt("unknown pass {}", impl);
						return Err(ResourceLoadingError::DecodingError);
					}

					std::size_t passId = m_current->passList->AddPass(passName.GetValue(), implIndex, std::move(implConfig));

					for (auto&& [inputName, attachmentName] : inputs)
					{
						std::size_t inputIndex = passRegistry.GetPassInputIndex(implIndex, inputName);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraErrorFmt("pass {} has no input {}", impl, inputName);
							return Err(ResourceLoadingError::DecodingError);
						}

						auto it = m_current->attachmentsByName.find(attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", attachmentName);
							return Err(ResourceLoadingError::DecodingError);
						}

						m_current->passList->SetPassInput(passId, inputIndex, it->second);
					}

					for (auto&& [outputName, attachmentName] : outputs)
					{
						std::size_t inputIndex = passRegistry.GetPassOutputIndex(implIndex, outputName);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraErrorFmt("pass {} has no output {}", impl, outputName);
							return Err(ResourceLoadingError::DecodingError);
						}

						auto it = m_current->attachmentsByName.find(attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", attachmentName);
							return Err(ResourceLoadingError::DecodingError);
						}

						m_current->passList->SetPassOutput(passId, inputIndex, it->second);
					}

					if (!depthstencilInput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilInput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", depthstencilInput);
							return Err(ResourceLoadingError::DecodingError);
						}

						m_current->passList->SetPassDepthStencilInput(passId, it->second);
					}

					if (!depthstencilOutput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilOutput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraErrorFmt("unknown attachment {}", depthstencilOutput);
							return Err(ResourceLoadingError::DecodingError);
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
							return Err(ResourceLoadingError::DecodingError);
						}
					}

					return Ok();
				}

				Result<std::string, ResourceLoadingError> GetKeyword(bool peek = false)
				{
					std::size_t beginOffset;
					do
					{
						auto result = EnsureLine();
						if (result.IsErr())
							return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

						beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
					}
					while (beginOffset == m_currentLine.npos);

					if (m_currentLine[beginOffset] == '"')
					{
						NazaraError("expected a keyword, got a string");
						return Err(ResourceLoadingError::DecodingError);
					}

					std::size_t endOffset = m_currentLine.find_first_of(" \r\t\n", beginOffset + 1);
					if (endOffset == m_currentLine.npos)
						endOffset = m_currentLine.size();

					std::string currentToken = std::string(m_currentLine.substr(beginOffset, endOffset - beginOffset));
					if (!peek)
						m_currentLine.erase(m_currentLine.begin(), m_currentLine.begin() + endOffset);

					return currentToken;
				}

				
				Result<std::string, ResourceLoadingError> GetString()
				{
					std::size_t beginOffset;
					do
					{
						auto result = EnsureLine();
						if (result.IsErr())
							return Err(std::move(result).GetError()); //< FIXME: why rvalue is needed

						beginOffset = m_currentLine.find_first_not_of(" \r\t\n");
					}
					while (beginOffset == m_currentLine.npos);

					if (m_currentLine[beginOffset] != '"')
					{
						NazaraError("expected a string, got a keyword");
						return Err(ResourceLoadingError::DecodingError);
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
							return Err(ResourceLoadingError::DecodingError);

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
									return Err(ResourceLoadingError::DecodingError);
							}

							str.push_back(character);
							break;
						}

						default:
							str.push_back(m_currentLine[i]);
						}
					}

					NazaraError("unfinished string");
					return Err(ResourceLoadingError::DecodingError);
				}

				Result<void, ResourceLoadingError> EnsureLine()
				{
					while (m_currentLine.find_first_not_of(" \r\t\n") == m_currentLine.npos)
					{
						m_currentLine.clear();
						m_stream.ReadLine(m_currentLine);
						if (m_currentLine.empty())
							return Err(ResourceLoadingError::DecodingError);
					}

					return Ok();
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
