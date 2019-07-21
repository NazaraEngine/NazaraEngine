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
	* \brief Constructs a Material object with default states
	*
	* \see Reset
	*/
	inline Material::Material(std::shared_ptr<const MaterialSettings> settings) :
	m_settings(std::move(settings)),
	m_reflectionMode(ReflectionMode_Skybox),
	m_pipelineUpdated(false),
	m_shadowCastingEnabled(true),
	m_reflectionSize(256)
	{
		m_pipelineInfo.settings = m_settings;
		SetShader("Basic");

		m_sharedUniformBuffers.resize(m_settings->GetSharedUniformBlocks().size());
		m_textures.resize(m_settings->GetTextures().size());

		m_uniformBuffers.reserve(m_settings->GetUniformBlocks().size());
		for (const auto& uniformBufferInfo : m_settings->GetUniformBlocks())
			m_uniformBuffers.emplace_back(UniformBuffer::New(static_cast<UInt32>(uniformBufferInfo.blockSize), DataStorage_Hardware, BufferUsage_Dynamic));
	}

	/*!
	* \brief Constructs a Material object by assignation
	*
	* \param material Material to copy into this
	*/
	inline Material::Material(const Material& material) :
	RefCounted(),
	Resource(material),
	m_reflectionMode(material.m_reflectionMode)
	{
		operator=(material);
	}

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
	inline void Material::Configure(const MaterialPipeline* pipeline)
	{
		NazaraAssert(pipeline, "Invalid material pipeline");

		m_pipeline = pipeline;
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

		// Temp and dirty fix for pipeline overriding has*Map
		m_pipelineInfo.hasAlphaMap = m_alphaMap.IsValid();
		m_pipelineInfo.hasDiffuseMap = m_diffuseMap.IsValid();
		m_pipelineInfo.hasEmissiveMap = m_emissiveMap.IsValid();
		m_pipelineInfo.hasHeightMap = m_heightMap.IsValid();
		m_pipelineInfo.hasNormalMap = m_normalMap.IsValid();
		m_pipelineInfo.hasSpecularMap = m_specularMap.IsValid();

		InvalidatePipeline();
	}

	/*!
	* \brief Reset material pipeline state
	*
	* Sets the material pipeline using a name to lookup in the MaterialPipelineLibrary
	*
	* \return True if the material pipeline was found in the library
	*
	* \see Configure
	*/
	inline bool Material::Configure(const String& pipelineName)
	{
		MaterialPipelineRef pipeline = MaterialPipelineLibrary::Query(pipelineName);
		if (!pipeline)
		{
			NazaraError("Failed to get pipeline \"" + pipelineName + "\"");
			return false;
		}

		Configure(std::move(pipeline));
		return true;
	}

	/*!
	* \brief Enable/Disable alpha test for this material
	*
	* When enabled, all objects using this material will be rendered using alpha testing,
	* rejecting pixels if their alpha component is under a defined threshold.
	* This allows some kind of transparency with a much cheaper cost as it doesn't prevent any optimization (as deferred rendering or batching).
	*
	* \param alphaTest Defines if this material will use alpha testing
	*
	* \remark Invalidates the pipeline
	*
	* \see IsAlphaTestEnabled
	* \see SetAlphaThreshold
	*/
	inline void Material::EnableAlphaTest(bool alphaTest)
	{
		m_pipelineInfo.alphaTest = alphaTest;

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
	inline RendererComparison Material::GetDepthFunc() const
	{
		return m_pipelineInfo.depthFunc;
	}

	/*!
	* \brief Gets the depth material
	*
	* \return Constant reference to the depth material
	*
	* \see EnableShadowCasting
	*/
	inline const MaterialRef& Material::GetDepthMaterial() const
	{
		return m_depthMaterial;
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
	inline const MaterialPipeline* Material::GetPipeline() const
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

	/*!
	* \brief Gets the reflection mode of the material
	*
	* \return Current reflection mode
	*
	* \see SetReflectionMode
	*/
	inline ReflectionMode Material::GetReflectionMode() const
	{
		return m_reflectionMode;
	}

	inline const std::shared_ptr<const MaterialSettings>& Material::GetSettings() const
	{
		return m_settings;
	}

	/*!
	* \brief Gets the über-shader used by this material
	* \return Constant pointer to the über-shader used
	*/
	inline const UberShader* Material::GetShader() const
	{
		return m_pipelineInfo.uberShader;
	}

	/*!
	* \brief Gets the src in blend
	* \return Function for src blending
	*/
	inline BlendFunc Material::GetSrcBlend() const
	{
		return m_pipelineInfo.srcBlend;
	}

	inline UniformBuffer* Material::GetSharedUniformBuffer(std::size_t bufferIndex) const
	{
		NazaraAssert(bufferIndex < m_sharedUniformBuffers.size(), "Invalid shared uniform buffer index");
		return m_sharedUniformBuffers[bufferIndex];
	}

	inline const TextureRef& Material::GetTexture(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].texture;
	}

	inline TextureSampler& Material::GetTextureSampler(std::size_t textureIndex)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].sampler;
	}

	inline const TextureSampler& Material::GetTextureSampler(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].sampler;
	}

	inline UniformBufferRef& Material::GetUniformBuffer(std::size_t bufferIndex)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex];
	}

	inline const UniformBufferRef& Material::GetUniformBuffer(std::size_t bufferIndex) const
	{
		NazaraAssert(bufferIndex < m_sharedUniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex];
	}

	inline bool Material::HasDepthMaterial() const
	{
		return m_depthMaterial.IsValid();
	}

	inline bool Material::HasTexture(std::size_t textureIndex) const
	{
		Texture* texture = GetTexture(textureIndex);
		return texture && texture->IsValid();
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
	* \brief Checks whether this material has alpha test enabled
	* \return true If it is the case
	*/
	inline bool Material::IsAlphaTestEnabled() const
	{
		return m_pipelineInfo.alphaTest;
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
	inline void Material::SetDepthFunc(RendererComparison depthFunc)
	{
		m_pipelineInfo.depthFunc = depthFunc;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the depth material
	* \return true If successful
	*
	* \param depthMaterial Material for depth
	*/
	inline void Material::SetDepthMaterial(MaterialRef depthMaterial)
	{
		m_depthMaterial = std::move(depthMaterial);
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

	/*!
	* \brief Changes reflection mode of the material
	*
	* When reflections are enabled, the material will render reflections from the object environment according to the reflection mode.
	* This function does change the reflection mode used by the material.
	*
	* Skyboxes reflections are the cheapest but are static and thus can't reflect other objects.
	* Probes reflections are cheap, depending on probes reflection mode, but require regular probe finding from objects using it.
	* Real-time reflections are expensive but provide the most accurate reflection map (and can reflect other objects around).
	*
	* \param reflectionMode The new reflection mode this material should use
	*
	* \remark May invalidates the pipeline
	*
	* \see EnableReflectionMapping
	* \see IsReflectionMappingEnabled
	* \see SetReflectionSize
	*/
	inline void Material::SetReflectionMode(ReflectionMode reflectionMode)
	{
		if (m_reflectionMode != reflectionMode)
		{
			OnMaterialReflectionModeChange(this, reflectionMode);

			m_reflectionMode = reflectionMode;
		}
	}

	/*!
	* \brief Sets the shader with a constant reference to a ubershader
	*
	* \param uberShader Uber shader to apply
	*
	* \remark Invalidates the pipeline
	*
	* \see GetShader
	*/
	inline void Material::SetShader(UberShaderConstRef uberShader)
	{
		m_pipelineInfo.uberShader = std::move(uberShader);

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the shader by name
	* \return true If successful
	*
	* \param uberShaderName Named shader
	*/
	inline bool Material::SetShader(const String& uberShaderName)
	{
		UberShaderConstRef uberShader = UberShaderLibrary::Get(uberShaderName);
		if (!uberShader)
			return false;

		SetShader(std::move(uberShader));
		return true;
	}

	inline void Material::SetSharedUniformBuffer(std::size_t bufferIndex, UniformBuffer* uniformBuffer)
	{
		NazaraAssert(bufferIndex < m_sharedUniformBuffers.size(), "Invalid shared uniform buffer index");
		m_sharedUniformBuffers[bufferIndex] = uniformBuffer;
	}

	inline void Material::SetTexture(std::size_t textureIndex, Texture* texture)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		m_textures[textureIndex].texture = texture;

		if (texture)
			m_pipelineInfo.textures |= UInt64(1) << UInt64(textureIndex);
		else
			m_pipelineInfo.textures &= ~(UInt64(1) << UInt64(textureIndex));

		InvalidatePipeline();
	}

	inline void Material::SetTextureSampler(std::size_t textureIndex, const TextureSampler& sampler)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		m_textures[textureIndex].sampler = sampler;
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

	/*!
	* \brief Sets the current material with the content of the other one
	* \return A reference to this
	*
	* \param material The other Material
	*/
	inline Material& Material::operator=(const Material& material)
	{
		Resource::operator=(material);

		m_settings = material.m_settings;
		m_textures = material.m_textures;
		m_sharedUniformBuffers = material.m_sharedUniformBuffers;
		m_depthMaterial = material.m_depthMaterial;
		m_pipeline = material.m_pipeline;
		m_pipelineInfo = material.m_pipelineInfo;
		m_pipelineUpdated = material.m_pipelineUpdated;
		m_shadowCastingEnabled = material.m_shadowCastingEnabled;
		m_reflectionSize = material.m_reflectionSize;

		m_pipelineInfo.settings = m_settings;

		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const UniformBuffer* sourceBuffer = material.GetUniformBuffer(i);
			UniformBuffer* targetBuffer = m_uniformBuffers[i] = UniformBuffer::New(sourceBuffer->GetEndOffset() - sourceBuffer->GetStartOffset(), DataStorage_Hardware, BufferUsage_Dynamic);
			if (!targetBuffer->CopyContent(sourceBuffer))
				NazaraError("Failed to copy uniform buffer content");
		}

		SetReflectionMode(material.GetReflectionMode());
		return *this;
	}

	/*!
	* \brief Gets the default material
	*
	* \return Reference to the default material
	*
	* \remark This material should NOT be modified as it would affect all objects using it
	*/
	inline MaterialRef Material::GetDefault()
	{
		return s_defaultMaterial;
	}

	inline int Material::GetTextureUnit(TextureMap textureMap)
	{
		return s_textureUnits[textureMap];
	}

	/*!
	* \brief Loads the material from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the material
	*/
	inline MaterialRef Material::LoadFromFile(const String& filePath, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromFile(filePath, params);
	}

	/*!
	* \brief Loads the material from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the material
	*/
	inline MaterialRef Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromMemory(data, size, params);
	}

	/*!
	* \brief Loads the material from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the material
	* \param params Parameters for the material
	*/
	inline MaterialRef Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromStream(stream, params);
	}

	inline void Material::InvalidatePipeline()
	{
		m_pipelineUpdated = false;
	}

	inline void Material::UpdatePipeline() const
	{
		m_pipeline = MaterialPipeline::GetPipeline(m_pipelineInfo);
		m_pipelineUpdated = true;
	}

	/*!
	* \brief Creates a new material from the arguments
	* \return A reference to the newly created material
	*
	* \param args Arguments for the material
	*/
	template<typename... Args>
	MaterialRef Material::New(Args&&... args)
	{
		std::unique_ptr<Material> object(new Material(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
