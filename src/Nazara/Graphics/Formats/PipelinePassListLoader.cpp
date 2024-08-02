// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Formats/PipelinePassListLoader.hpp>
#include <Nazara/Core/ParameterFile.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <fast_float/fast_float.h>
#include <optional>

namespace Nz::Loaders
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		class PassListLoader
		{
			public:
				PassListLoader(Stream& stream, const PipelinePassListParams& /*parameters*/) :
				m_paramFile(stream)
				{
				}

				Result<std::shared_ptr<PipelinePassList>, ResourceLoadingError> Parse()
				{
					try
					{
						std::string finalOutputAttachment;

						m_paramFile.Parse(
							"passlist", [&](ParameterFileSection section, std::string /*passListName*/)
							{
								m_current.emplace();
								m_current->passList = std::make_shared<PipelinePassList>();
								section.Block(
									"attachment", &PassListLoader::HandleAttachment, this,
									"attachmentproxy", &PassListLoader::HandleAttachmentProxy, this,
									"pass", &PassListLoader::HandlePass, this,
									"output", &finalOutputAttachment
								);
							}
						);

						if (finalOutputAttachment.empty())
						{
							NazaraError("missing passlist output attachment");
							return Err(ResourceLoadingError::DecodingError);
						}

						auto it = m_current->attachmentsByName.find(finalOutputAttachment);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraError("unknown attachment {}", finalOutputAttachment);
							return Err(ResourceLoadingError::DecodingError);
						}

						m_current->passList->SetFinalOutput(it->second);

						return Ok(std::move(m_current->passList));
					}
					catch (ResourceLoadingError error)
					{
						return Err(error);
					}
				}

			private:
				void HandleAttachment(ParameterFileSection section, std::string attachmentName)
				{
					std::string format;

					section.Block(
						"format", &format
					);

					if (format.empty())
					{
						NazaraError("missing mandatory format in attachment {}", attachmentName);
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
						NazaraError("unknown format {}", format);
						throw ResourceLoadingError::DecodingError;
					}

					if (m_current->attachmentsByName.find(attachmentName) != m_current->attachmentsByName.end())
					{
						NazaraError("attachment {} already exists", attachmentName);
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t attachmentId = m_current->passList->AddAttachment({
						attachmentName,
						attachmentFormat
					});

					m_current->attachmentsByName.emplace(std::move(attachmentName), attachmentId);
				}

				void HandleAttachmentProxy(std::string proxyName, std::string targetName)
				{
					auto it = m_current->attachmentsByName.find(targetName);
					if (it == m_current->attachmentsByName.end())
					{
						NazaraError("unknown attachment {}", targetName);
						throw ResourceLoadingError::DecodingError;
					}

					if (m_current->attachmentsByName.find(proxyName) != m_current->attachmentsByName.end())
					{
						NazaraError("attachment {} already exists", proxyName);
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t proxyId = m_current->passList->AddAttachmentProxy(proxyName, it->second);
					m_current->attachmentsByName.emplace(std::move(proxyName), proxyId);
				}

				void HandlePass(ParameterFileSection section, std::string passName)
				{
					struct InputOutput
					{
						std::string name;
						std::string attachmentName;
					};

					struct Output : InputOutput
					{
						std::string clearColor;
					};

					ParameterList implConfig;
					std::string impl;
					std::string depthstencilInput;
					std::string depthstencilOutput;
					std::vector<InputOutput> inputs;
					std::vector<Output> outputs;
					std::vector<std::string> flags;

					section.Block(
						"depthstencilinput", &depthstencilInput,
						"depthstenciloutput", &depthstencilOutput,
						"impl", [&](ParameterFileSection implSection, std::string passImpl)
						{
							impl = std::move(passImpl);
							implSection.Block(ParameterFile::OptionalBlock,
								ParameterFile::List, [&](ParameterFile::Identifier key, std::string value)
								{
									implConfig.SetParameter(std::move(key.value), std::move(value));
								}
							);
						},
						"input", [&](std::string name, std::string attachment)
						{
							inputs.push_back({
								std::move(name),
								std::move(attachment),
							});
						},
						"output", [&](ParameterFileSection implSection, std::string name, std::string attachment)
						{
							auto& outputData = outputs.emplace_back();
							outputData.name = std::move(name);
							outputData.attachmentName = std::move(attachment);

							implSection.Block(ParameterFile::OptionalBlock,
								"ClearColor", &outputData.clearColor
							);
						},
						"flag", [&](std::string flag)
						{
							flags.push_back(std::move(flag));
						}
					);

					FramePipelinePassRegistry& passRegistry = Graphics::Instance()->GetFramePipelinePassRegistry();

					std::size_t implIndex = passRegistry.GetPassIndex(impl);
					if (implIndex == FramePipelinePassRegistry::InvalidIndex)
					{
						NazaraError("unknown pass {}", impl);
						throw ResourceLoadingError::DecodingError;
					}

					std::size_t passId = m_current->passList->AddPass(std::move(passName), implIndex, std::move(implConfig));

					for (auto&& [inputName, attachmentName] : inputs)
					{
						std::size_t inputIndex = passRegistry.GetPassInputIndex(implIndex, inputName);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraError("pass {} has no input {}", impl, inputName);
							throw ResourceLoadingError::DecodingError;
						}

						auto it = m_current->attachmentsByName.find(attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraError("unknown attachment {}", attachmentName);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassInput(passId, inputIndex, it->second);
					}

					for (auto&& outputData : outputs)
					{
						std::size_t inputIndex = passRegistry.GetPassOutputIndex(implIndex, outputData.name);
						if (inputIndex == FramePipelinePassRegistry::InvalidIndex)
						{
							NazaraError("pass {} has no output {}", impl, outputData.name);
							throw ResourceLoadingError::DecodingError;
						}

						auto it = m_current->attachmentsByName.find(outputData.attachmentName);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraError("unknown attachment {}", outputData.attachmentName);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassOutput(passId, inputIndex, it->second);

						if (!outputData.clearColor.empty())
						{
							if (outputData.clearColor == "Viewer")
								m_current->passList->SetPassOutputClearColor(passId, inputIndex, FramePipelinePass::ViewerClearColor{});
							else
							{
								float color[4] = { 0.f, 0.f, 0.f, 1.f };
								const char* ptr = outputData.clearColor.c_str();
								const char* end = ptr + outputData.clearColor.size();
								for (std::size_t i = 0; i < 4; ++i)
								{
									fast_float::from_chars_result r = fast_float::from_chars(ptr, end, color[i]);
									if (r.ec != std::errc{})
									{
										NazaraErrorFmt("failed to decode clear color \"{}\" for output {}", outputData.clearColor, outputData.name);
										throw ResourceLoadingError::DecodingError;
									}

									if (r.ptr == end)
									{
										if (i < 3)
										{
											NazaraErrorFmt("failed to decode clear color \"{}\" for output {} (expected 3 or 4 floats)", outputData.clearColor, outputData.name);
											throw ResourceLoadingError::DecodingError;
										}
									}

									ptr = r.ptr;
									if (*ptr == ' ')
										ptr++;
								}

								if (ptr != end)
								{
									NazaraErrorFmt("failed to decode clear color \"{}\" for output {} (more than 4 floats)", outputData.clearColor, outputData.name);

									throw ResourceLoadingError::DecodingError;
								}

								m_current->passList->SetPassOutputClearColor(passId, inputIndex, Color(color[0], color[1], color[2], color[3]));
							}
						}
					}

					if (!depthstencilInput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilInput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraError("unknown attachment {}", depthstencilInput);
							throw ResourceLoadingError::DecodingError;
						}

						m_current->passList->SetPassDepthStencilInput(passId, it->second);
					}

					if (!depthstencilOutput.empty())
					{
						auto it = m_current->attachmentsByName.find(depthstencilOutput);
						if (it == m_current->attachmentsByName.end())
						{
							NazaraError("unknown attachment {}", depthstencilOutput);
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
							NazaraError("unknown pass flag {}", flagStr);
							throw ResourceLoadingError::DecodingError;
						}
					}
				}

				struct CurrentPassList
				{
					std::shared_ptr<PipelinePassList> passList;
					std::unordered_map<std::string /*attachmentName*/, std::size_t /*attachmentId*/, StringHash<>, std::equal_to<>> attachmentsByName;
				};

				std::optional<CurrentPassList> m_current;
				std::string m_currentLine;
				ParameterFile m_paramFile;
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
