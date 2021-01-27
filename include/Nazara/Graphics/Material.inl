// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Destructs the object and calls OnMaterialRelease
	*
	* \see OnMaterialRelease
	*/
	inline Material::~Material()
	{
		OnMaterialRelease(this);
	}

	/*!
	* \brief Reset material pipeline state
	*
	* Sets the material pipeline
	*
	* \remark pipeline must be valid
	*
	* \see Configure
	*/
	inline void Material::Configure(std::shared_ptr<MaterialPipeline> pipeline)
	{
		NazaraAssert(pipeline, "Invalid material pipeline");

		m_pipeline = std::move(pipeline);
		m_pipelineInfo = m_pipeline->GetInfo();
		m_pipelineUpdated = true;
	}

	/*!
	* \brief Reset material pipeline state
	*
	* Sets the material pipeline using pipeline info
	*
	* \remark pipeline must be valid
	*
	* \see Configure
	*/
	inline void Material::Configure(const MaterialPipelineInfo& pipelineInfo)
	{
		m_pipelineInfo = pipelineInfo;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable blending for this material
	*
	* When enabled, all objects using this material will be rendered using blending, obeying the dstBlend and srcBlend parameters
	* This is useful with translucent objects, but will reduces performance as it prevents some optimizations (as deferred rendering)
	*
	* \param blending Defines if this material will use blending
	*
	* \remark Invalidates the pipeline
	*
	* \see IsBlendingEnabled
	* \see SetDstBlend
	* \see SetSrcBlend
	*/
	inline void Material::EnableBlending(bool blending)
	{
		m_pipelineInfo.blending = blending;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable color writing for this material
	*
	* \param colorWrite Defines if this material will use color writing
	*
	* \remark Invalidates the pipeline
	*
	* \see IsColorWritingEnabled
	*/
	inline void Material::EnableColorWrite(bool colorWrite)
	{
		m_pipelineInfo.colorWrite = colorWrite;

		InvalidatePipeline();
	}

	inline void Material::EnableCondition(std::size_t conditionIndex, bool enable)
	{
		if (TestBit<UInt64>(m_enabledConditions, conditionIndex) != enable)
		{
			m_enabledConditions = SetBit<UInt64>(m_enabledConditions, conditionIndex);
			InvalidatePipeline();
		}
	}

	/*!
	* \brief Enable/Disable depth buffer for this material
	*
	* When enabled, all objects using this material will be rendered using a depth buffer, if the RenderTarget has one.
	* This will enable Depth Test, preventing further fragments to render on top of closer ones.
	*
	* This parameter is required for depth writing.
	*
	* In order to enable depth writing without enabling depth test, set the depth comparison function to RendererComparison_Never
	*
	* \param depthBuffer Defines if this material will use depth buffer
	*
	* \remark Invalidates the pipeline
	*
	* \see EnableDepthWrite
	* \see IsDepthBufferEnabled
	* \see SetDepthFunc
	*/
	inline void Material::EnableDepthBuffer(bool depthBuffer)
	{
		m_pipelineInfo.depthBuffer = depthBuffer;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable depth sorting for this material
	*
	* When enabled, all objects using this material will be rendered far from near
	* This is useful with translucent objects, but will reduces performance as it breaks batching
	*
	* \param depthSorting Defines if this material will use depth sorting
	*
	* \remark Depth sorting may not be perfect (may be object-sorting instead of triangle-sorting)
	* \remark Invalidates the pipeline
	*
	* \see IsDepthSortingEnabled
	*/
	inline void Material::EnableDepthSorting(bool depthSorting)
	{
		m_pipelineInfo.depthSorting = depthSorting;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable depth writing for this material
	*
	* When enabled, and if depth buffer is enabled and present, all fragments generated with this material will write
	* to the depth buffer if they pass depth test.
	*
	* This is usually disabled with translucent objects, as depth test is wanted to prevent them from rendering on top of opaque objects but
	* not depth writing (which could make other translucent fragments to fail depth test)
	*
	* \param depthBuffer Defines if this material will use depth write
	*
	* \remark Invalidates the pipeline
	*
	* \see EnableDepthBuffer
	* \see IsDepthWriteEnabled
	*/
	inline void Material::EnableDepthWrite(bool depthWrite)
	{
		m_pipelineInfo.depthWrite = depthWrite;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable face culling for this material
	*
	* When enabled, the material prevents front and/or back faces from rendering.
	* This is commonly used as an optimization to prevent processing of hidden faces by the rendering device.
	*
	* Use SetFaceCulling to control which side will be eliminated.
	*
	* \param faceCulling Defines if this material will use face culling
	*
	* \remark Invalidates the pipeline
	*
	* \see IsFaceCullingEnabled
	* \see SetFaceCulling
	*/
	inline void Material::EnableFaceCulling(bool faceCulling)
	{
		m_pipelineInfo.faceCulling = faceCulling;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable reflection mapping for this material
	*
	* When enabled, the material will render reflections from the object environment according to the reflection mode.
	* Whether or not this is expensive depends of the reflection mode and size.
	*
	* Please note this is only a hint for the render technique, and reflections can be forcefully enabled or disabled depending on the material shader.
	*
	* Use SetReflectionMode and SetReflectionSize to control reflection quality.
	*
	* \param reflection Defines if this material should use reflection mapping
	*
	* \remark May invalidates the pipeline
	*
	* \see IsReflectionMappingEnabled
	* \see SetReflectionMode
	* \see SetReflectionSize
	*/
	inline void Material::EnableReflectionMapping(bool reflection)
	{
		m_pipelineInfo.reflectionMapping = reflection;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable scissor test for this material
	*
	* When enabled, the material prevents fragments out of the scissor box to be rendered.
	* This can be useful with GUI, where widgets must not be rendered outside of their parent rendering area.
	*
	* \param scissorTest Defines if this material will use scissor test
	*
	* \remark Invalidates the pipeline
	*
	* \see IsScissorTestEnabled
	*/
	inline void Material::EnableScissorTest(bool scissorTest)
	{
		m_pipelineInfo.scissorTest = scissorTest;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable shadow casting for this material
	*
	* When enabled, all objects using this material will be allowed to cast shadows upon any objects using a material with shadow receiving enabled.
	* The depth material replaces this one when rendering shadows.
	*
	* \param castShadows Defines if this material will be allowed to cast shadows
	*
	* \remark Does not invalidate the pipeline
	*
	* \see EnableShadowReceive
	* \see IsShadowCastingEnabled
	* \see SetDepthMaterial
	*/
	inline void Material::EnableShadowCasting(bool castShadows)
	{
		// Has no influence on pipeline
		m_shadowCastingEnabled = castShadows;
	}

	/*!
	* \brief Enable/Disable shadow receiving for this material
	*
	* When enabled, all objects using this material will be allowed to be casted shadows upon themselves
	* Disabling this can be helpful to prevent some rendering artifacts (especially with translucent objects)
	*
	* \param receiveShadows Defines if this material will be able to receive shadows
	*
	* \remark Invalidates the pipeline
	*
	* \see IsShadowReceiveEnabled
	*/
	inline void Material::EnableShadowReceive(bool receiveShadows)
	{
		m_pipelineInfo.shadowReceive = receiveShadows;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable stencil test for this material
	*
	* When enabled, all fragments must pass the stencil test to be rendered.
	*
	* \param scissorTest Defines if this material will use stencil test
	*
	* \remark Invalidates the pipeline
	*
	* \see IsStencilTestEnabled
	*/
	inline void Material::EnableStencilTest(bool stencilTest)
	{
		m_pipelineInfo.stencilTest = stencilTest;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable vertex coloring on this material
	*
	* This is a temporary option, until the new material pipeline system is ready, allowing to enable vertex coloring.
	* This option only works with meshes using vertex colors.
	*
	* \param vertexColor Defines if this material will use vertex color or not
	*
	* \remark Invalidates the pipeline
	*
	* \see HasVertexColor
	*/
	inline void Material::EnableVertexColor(bool vertexColor)
	{
		m_pipelineInfo.hasVertexColor = vertexColor;

		InvalidatePipeline();
	}

	/*!
	* \brief Ensures the pipeline gets updated
	*
	* When the pipeline gets invalidated, it's not updated until required (per example by calling GetPipeline).
	* Using this function forces the pipeline update, making GetPipeline thread-safe as long as the pipeline does not get invalidated.
	*
	* \see GetPipeline
	*/
	inline void Material::EnsurePipelineUpdate() const
	{
		if (!m_pipelineUpdated)
			UpdatePipeline();
	}

	/*!
	* \brief Gets the function to compare depth
	*
	* \return Function comparing the depth of two materials
	*
	* \see EnableDepthTest
	* \see SetAmbientColor
	*/
	inline RendererComparison Material::GetDepthCompareFunc() const
	{
		return m_pipelineInfo.depthCompare;
	}

	/*!
	* \brief Gets the dst in blend
	*
	* \return Function for dst blending
	*
	* \see SetDstBlend
	*/
	inline BlendFunc Material::GetDstBlend() const
	{
		return m_pipelineInfo.dstBlend;
	}

	/*!
	* \brief Gets the face culling
	*
	* \return Current face culling side
	*
	* \see SetFaceCulling
	*/
	inline FaceSide Material::GetFaceCulling() const
	{
		return m_pipelineInfo.cullingSide;
	}

	/*!
	* \brief Gets the face filling
	* \return Current face filling
	*/
	inline FaceFilling Material::GetFaceFilling() const
	{
		return m_pipelineInfo.faceFilling;
	}

	/*!
	* \brief Gets the line width of this material
	* \return Line width
	*/
	inline float Material::GetLineWidth() const
	{
		return m_pipelineInfo.lineWidth;
	}

	/*!
	* \brief Gets the render states
	* \return Constant reference to the render states
	*/
	inline const std::shared_ptr<MaterialPipeline>& Material::GetPipeline() const
	{
		EnsurePipelineUpdate();

		return m_pipeline;
	}

	/*!
	* \brief Gets the pipeline informations
	* \return Constant reference to the pipeline info
	*/
	inline const MaterialPipelineInfo& Material::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}

	/*!
	* \brief Gets the point size of this material
	* \return Point size
	*/
	inline float Material::GetPointSize() const
	{
		return m_pipelineInfo.pointSize;
	}

	inline const std::shared_ptr<const MaterialSettings>& Material::GetSettings() const
	{
		return m_settings;
	}

	/*!
	* \brief Gets the über-shader used by this material
	* \return Constant pointer to the über-shader used
	*/
	inline const std::shared_ptr<UberShader>& Material::GetShader(ShaderStageType shaderStage) const
	{
		return m_pipelineInfo.shaders[UnderlyingCast(shaderStage)].uberShader;
	}

	/*!
	* \brief Gets the src in blend
	* \return Function for src blending
	*/
	inline BlendFunc Material::GetSrcBlend() const
	{
		return m_pipelineInfo.srcBlend;
	}

	inline const std::shared_ptr<Texture>& Material::GetTexture(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].texture;
	}

	inline const TextureSamplerInfo& Material::GetTextureSampler(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].samplerInfo;
	}

	inline const std::shared_ptr<AbstractBuffer>& Material::GetUniformBuffer(std::size_t bufferIndex) const
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex].buffer;
	}

	inline std::vector<UInt8>& Material::GetUniformBufferData(std::size_t bufferIndex)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		UniformBuffer& uboEntry = m_uniformBuffers[bufferIndex];
		uboEntry.dataInvalidated = true;
		return uboEntry.data;
	}

	inline const std::vector<UInt8>& Material::GetUniformBufferConstData(std::size_t bufferIndex)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex].data;
	}

	inline bool Material::HasTexture(std::size_t textureIndex) const
	{
		return GetTexture(textureIndex) != nullptr;
	}

	/*!
	* \brief Checks whether this material uses vertex coloring
	* \return true If it is the case
	*/
	inline bool Material::HasVertexColor() const
	{
		return m_pipelineInfo.hasVertexColor;
	}

	/*!
	* \brief Checks whether this material has blending enabled
	* \return true If it is the case
	*/
	inline bool Material::IsBlendingEnabled() const
	{
		return m_pipelineInfo.blending;
	}

	/*!
	* \brief Checks whether this material has color write enabled
	* \return true If it is the case
	*/
	inline bool Material::IsColorWriteEnabled() const
	{
		return m_pipelineInfo.colorWrite;
	}

	inline bool Material::IsConditionEnabled(std::size_t conditionIndex) const
	{
		return TestBit<UInt64>(m_enabledConditions, conditionIndex);
	}

	/*!
	* \brief Checks whether this material has depth buffer enabled
	* \return true If it is the case
	*/
	inline bool Material::IsDepthBufferEnabled() const
	{
		return m_pipelineInfo.depthBuffer;
	}

	/*!
	* \brief Checks whether this material has depth sorting enabled
	* \return true If it is the case
	*/
	inline bool Material::IsDepthSortingEnabled() const
	{
		return m_pipelineInfo.depthSorting;
	}

	/*!
	* \brief Checks whether this material has depth writing enabled
	* \return true If it is the case
	*/
	inline bool Material::IsDepthWriteEnabled() const
	{
		return m_pipelineInfo.depthWrite;
	}

	/*!
	* \brief Checks whether this material has face culling enabled
	* \return true If it is the case
	*/
	inline bool Material::IsFaceCullingEnabled() const
	{
		return m_pipelineInfo.faceCulling;
	}

	/*!
	* \brief Checks whether this material has reflection mapping enabled
	* \return true If it is the case
	*
	* \see EnableReflectionMapping
	*/
	inline bool Material::IsReflectionMappingEnabled() const
	{
		return m_pipelineInfo.reflectionMapping;
	}

	/*!
	* \brief Checks whether this material has scissor test enabled
	* \return true If it is the case
	*/
	inline bool Material::IsScissorTestEnabled() const
	{
		return m_pipelineInfo.scissorTest;
	}

	/*!
	* \brief Checks whether this material has stencil test enabled
	* \return true If it is the case
	*/
	inline bool Material::IsStencilTestEnabled() const
	{
		return m_pipelineInfo.stencilTest;
	}

	/*!
	* \brief Checks whether this material cast shadow
	* \return true If it is the case
	*/
	inline bool Material::IsShadowCastingEnabled() const
	{
		return m_shadowCastingEnabled;
	}

	/*!
	* \brief Checks whether this material receive shadow
	* \return true If it is the case
	*/
	inline bool Material::IsShadowReceiveEnabled() const
	{
		return m_pipelineInfo.shadowReceive;
	}

	/*!
	* \brief Sets the depth functor
	*
	* \param depthFunc
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetDepthCompareFunc(RendererComparison depthFunc)
	{
		m_pipelineInfo.depthCompare = depthFunc;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the dst in blend
	*
	* \param func Function for dst blending
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetDstBlend(BlendFunc func)
	{
		m_pipelineInfo.dstBlend = func;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the face culling
	*
	* \param faceSide Face to cull
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetFaceCulling(FaceSide faceSide)
	{
		m_pipelineInfo.cullingSide = faceSide;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the face filling
	*
	* \param filling Face to fill
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetFaceFilling(FaceFilling filling)
	{
		m_pipelineInfo.faceFilling = filling;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the line width for this material
	*
	* This parameter is used when rendering lines, to define the width (in pixels) the line will take on the framebuffer
	*
	* \param lineWidth Width of the line
	*
	* \remark Invalidates the pipeline
	*
	* \see GetLineWidth
	*/
	inline void Material::SetLineWidth(float lineWidth)
	{
		m_pipelineInfo.lineWidth = lineWidth;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the point size for this material
	*
	* This parameter is used when rendering points, to define the size (in pixels) the point will take on the framebuffer
	*
	* \param pointSize Size of the point
	*
	* \remark Invalidates the pipeline
	*
	* \see GetPointSize
	*/
	inline void Material::SetPointSize(float pointSize)
	{
		m_pipelineInfo.pointSize = pointSize;

		InvalidatePipeline();
	}

	inline void Material::SetUniformBuffer(std::size_t bufferIndex, std::shared_ptr<AbstractBuffer> uniformBuffer)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid shared uniform buffer index");
		if (m_uniformBuffers[bufferIndex].buffer != uniformBuffer)
		{
			m_uniformBuffers[bufferIndex].buffer = std::move(uniformBuffer);
			m_uniformBuffers[bufferIndex].dataInvalidated = true;
			InvalidateShaderBinding();
		}
	}

	inline void Material::SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		if (m_textures[textureIndex].texture != texture)
		{
			m_textures[textureIndex].texture = std::move(texture);
			InvalidateShaderBinding();
		}
	}

	inline void Material::SetTextureSampler(std::size_t textureIndex, TextureSamplerInfo samplerInfo)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		if (m_textures[textureIndex].samplerInfo != samplerInfo)
		{
			m_textures[textureIndex].samplerInfo = std::move(samplerInfo);
			InvalidateTextureSampler(textureIndex);
		}
	}

	/*!
	* \brief Sets the src in blend
	*
	* \param func Function for src blending
	*
	* \remark Invalidates the pipeline
	*
	* \see GetSrcBlend
	*/
	inline void Material::SetSrcBlend(BlendFunc func)
	{
		m_pipelineInfo.srcBlend = func;

		InvalidatePipeline();
	}

	inline void Material::InvalidatePipeline()
	{
		m_pipelineUpdated = false;
	}

	inline void Material::InvalidateShaderBinding()
	{
		//TODO
	}

	inline void Material::InvalidateTextureSampler(std::size_t textureIndex)
	{
		assert(textureIndex < m_textures.size());
		m_textures[textureIndex].sampler.reset();

		InvalidateShaderBinding();
	}

	inline void Material::UpdatePipeline() const
	{
		for (auto& shader : m_pipelineInfo.shaders)
			shader.enabledConditions = 0;

		const auto& conditions = m_settings->GetConditions();
		for (std::size_t conditionIndex = 0; conditionIndex < conditions.size(); ++conditionIndex)
		{
			if (TestBit<UInt64>(m_enabledConditions, conditionIndex))
			{
				for (std::size_t shaderStage = 0; shaderStage < ShaderStageTypeCount; ++shaderStage)
					m_pipelineInfo.shaders[shaderStage].enabledConditions |= conditions[conditionIndex].enabledConditions[shaderStage];
			}
		}

		m_pipeline = MaterialPipeline::Get(m_pipelineInfo);
		m_pipelineUpdated = true;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
