// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
#if 0
	/*!
	* \ingroup graphics
	* \class Nz::Material
	* \brief Graphics class that represents a material
	*/

	/*!
	* \brief Constructs a Material object with default states
	*
	* \see Reset
	*/
	MaterialPass::MaterialPass(Settings&& settings)
	{
		m_pipelineInfo.pipelineLayout = std::move(settings.pipelineLayout);

		m_shaders.resize(settings.shaders.size());
		for (std::size_t i = 0; i < m_shaders.size(); ++i)
		{
			auto& shaderData = m_pipelineInfo.shaders.emplace_back();
			shaderData.uberShader = std::move(settings.shaders[i].uberShader);
			shaderData.uberShader->UpdateConfigCallback([=](UberShader::Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)
			{
				if (vertexBuffers.empty())
					return;

				const VertexDeclaration& vertexDeclaration = *vertexBuffers.front().declaration;
				const auto& components = vertexDeclaration.GetComponents();

				Int32 locationIndex = 0;
				for (const auto& component : components)
				{
					switch (component.component)
					{
						case VertexComponent::Color:
							config.optionValues[CRC32("VertexColorLoc")] = locationIndex;
							break;

						case VertexComponent::Normal:
							config.optionValues[CRC32("VertexNormalLoc")] = locationIndex;
							break;

						case VertexComponent::Position:
							config.optionValues[CRC32("VertexPositionLoc")] = locationIndex;
							break;

						case VertexComponent::Tangent:
							config.optionValues[CRC32("VertexTangentLoc")] = locationIndex;
							break;

						case VertexComponent::TexCoord:
							config.optionValues[CRC32("VertexUvLoc")] = locationIndex;
							break;

						case VertexComponent::JointIndices:
							config.optionValues[CRC32("VertexJointIndicesLoc")] = locationIndex;
							break;

						case VertexComponent::JointWeights:
							config.optionValues[CRC32("VertexJointWeightsLoc")] = locationIndex;
							break;

						case VertexComponent::Unused:
						case VertexComponent::Userdata:
							break;
					}

					++locationIndex;
				}
			});

			// TODO: Ensure pipeline layout compatibility using ShaderReflection

			m_shaders[i].onShaderUpdated.Connect(shaderData.uberShader->OnShaderUpdated, [this](UberShader*)
			{
				InvalidatePipeline();
			});
		}
	}
#endif
}
