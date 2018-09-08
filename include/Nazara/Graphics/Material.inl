// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
	inline Material::Material()
	{
		Reset();
	}

	/*!
	* \brief Constructs a Material object using a MaterialPipeline
	*
	* Calls Configure with the pipeline parameter
	*
	* \see Configure
	*/
	inline Material::Material(const MaterialPipeline* pipeline)
	{
		ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		Reset();
		Configure(pipeline);
	}

	/*!
	* \brief Constructs a Material object using a MaterialPipelineInfo
	*
	* Calls Configure with the pipelineInfo parameter
	*
	* \see Configure
	*/
	inline Material::Material(const MaterialPipelineInfo& pipelineInfo)
	{
		ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		Reset();
		Configure(pipelineInfo);
	}

	/*!
	* \brief Constructs a Material object using a MaterialPipeline name
	*
	* Calls Configure with the pipelineName parameter
	*
	* \remark In case of error (ie. named pipeline is not registered), throw an exception
	*
	* \see Configure
	*/
	inline Material::Material(const String& pipelineName)
	{
		ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		Reset();
		Configure(pipelineName);
	}

	/*!
	* \brief Constructs a Material object by assignation
	*
	* \param material Material to copy into this
	*/
	inline Material::Material(const Material& material) :
	RefCounted(),
	Resource(material),
	m_reflectionMode(ReflectionMode_Skybox)
	{
		Copy(material);
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
	* \brief Gets the alpha map
	*
	* \return Constant reference to the current texture
	*
	* \see SetAlphaMap
	*/
	inline const TextureRef& Material::GetAlphaMap() const
	{
		return m_alphaMap;
	}

	/*!
	* \brief Gets the alpha test threshold
	*
	* \return The threshold value for the alpha test
	*
	* \see EnableAlphaTest
	* \see SetAlphaThreshold
	*/
	inline float Material::GetAlphaThreshold() const
	{
		return m_alphaThreshold;
	}

	/*!
	* \brief Gets the ambient color
	*
	* \return Ambient color
	*
	* \see SetAmbientColor
	*/
	inline Color Material::GetAmbientColor() const
	{
		return m_ambientColor;
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
	* \brief Gets the diffuse color
	*
	* \return Diffuse color
	*
	* \see SetDiffuseColor
	*/
	inline Color Material::GetDiffuseColor() const
	{
		return m_diffuseColor;
	}

	/*!
	* \brief Gets the diffuse sampler
	*
	* \return Reference to the current texture sampler for the diffuse
	*
	* \see SetDiffuseSampler
	*/
	inline TextureSampler& Material::GetDiffuseSampler()
	{
		return m_diffuseSampler;
	}

	/*!
	* \brief Gets the diffuse sampler
	*
	* \return Constant reference to the current texture sampler for the diffuse
	*
	* \see SetDiffuseSampler
	*/
	inline const TextureSampler& Material::GetDiffuseSampler() const
	{
		return m_diffuseSampler;
	}

	/*!
	* \brief Gets the diffuse map
	*
	* \return Constant reference to the texture
	*
	* \see SetDiffuseMap
	*/
	const TextureRef& Material::GetDiffuseMap() const
	{
		return m_diffuseMap;
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
	* \brief Gets the emissive map
	*
	* \return Constant reference to the texture
	*
	* \see SetEmissiveMap
	*/
	inline const TextureRef& Material::GetEmissiveMap() const
	{
		return m_emissiveMap;
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
	* \brief Gets the height map
	* \return Constant reference to the texture
	*/
	inline const TextureRef& Material::GetHeightMap() const
	{
		return m_heightMap;
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
	* \brief Gets the normal map
	* \return Constant reference to the texture
	*/

	inline const TextureRef& Material::GetNormalMap() const
	{
		return m_normalMap;
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

	/*!
	* \brief Gets the über-shader used by this material
	* \return Constant pointer to the über-shader used
	*/
	inline const UberShader* Material::GetShader() const
	{
		return m_pipelineInfo.uberShader;
	}

	/*!
	* \brief Gets the shininess
	* \return Current shininess
	*/
	inline float Material::GetShininess() const
	{
		return m_shininess;
	}

	/*!
	* \brief Gets the specular color
	* \return Specular color
	*/
	inline Color Material::GetSpecularColor() const
	{
		return m_specularColor;
	}

	/*!
	* \brief Gets the specular map
	* \return Constant reference to the texture
	*/
	inline const TextureRef& Material::GetSpecularMap() const
	{
		return m_specularMap;
	}

	/*!
	* \brief Gets the specular sampler
	* \return Reference to the current texture sampler for the specular
	*/
	inline TextureSampler& Material::GetSpecularSampler()
	{
		return m_specularSampler;
	}

	/*!
	* \brief Gets the specular sampler
	* \return Constant reference to the current texture sampler for the specular
	*/
	inline const TextureSampler& Material::GetSpecularSampler() const
	{
		return m_specularSampler;
	}

	/*!
	* \brief Gets the src in blend
	* \return Function for src blending
	*/
	inline BlendFunc Material::GetSrcBlend() const
	{
		return m_pipelineInfo.srcBlend;
	}

	/*!
	* \brief Checks whether this material has an alpha map
	* \return true If it is the case
	*/
	inline bool Material::HasAlphaMap() const
	{
		return m_alphaMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a depth material
	* \return true If it is the case
	*/
	inline bool Material::HasDepthMaterial() const
	{
		return m_depthMaterial.IsValid();
	}

	/*!
	* \brief Checks whether this material has a diffuse map
	* \return true If it is the case
	*/
	inline bool Material::HasDiffuseMap() const
	{
		return m_diffuseMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a emissive map
	* \return true If it is the case
	*/
	inline bool Material::HasEmissiveMap() const
	{
		return m_emissiveMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a height map
	* \return true If it is the case
	*/
	inline bool Material::HasHeightMap() const
	{
		return m_heightMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a normal map
	* \return true If it is the case
	*/
	inline bool Material::HasNormalMap() const
	{
		return m_normalMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a specular map
	* \return true If it is the case
	*/
	inline bool Material::HasSpecularMap() const
	{
		return m_specularMap.IsValid();
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
	* \brief Loads the material from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the material
	*/
	inline bool Material::LoadFromFile(const String& filePath, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromFile(this, filePath, params);
	}

	/*!
	* \brief Loads the material from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the material
	*/
	inline bool Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromMemory(this, data, size, params);
	}

	/*!
	* \brief Loads the material from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the material
	* \param params Parameters for the material
	*/
	inline bool Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromStream(this, stream, params);
	}

	/*!
	* \brief Sets the alpha map by name
	* \return true If successful
	*
	* \param textureName Named texture
	*/
	inline bool Material::SetAlphaMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get alpha map \"" + textureName + "\"");
				return false;
			}
		}

		SetAlphaMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the alpha map with a reference to a texture
	* \return true If successful
	*
	* \param alphaMap Texture
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetAlphaMap(TextureRef alphaMap)
	{
		m_alphaMap = std::move(alphaMap);
		m_pipelineInfo.hasAlphaMap = m_alphaMap.IsValid();

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the alpha threshold
	*
	* \param alphaThreshold Threshold for the alpha
	*/
	inline void Material::SetAlphaThreshold(float alphaThreshold)
	{
		m_alphaThreshold = alphaThreshold;
	}

	/*!
	* \brief Sets the color for ambient
	*
	* \param ambient Color for ambient
	*/
	inline void Material::SetAmbientColor(const Color& ambient)
	{
		m_ambientColor = ambient;
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
	* \brief Sets the color for diffuse
	*
	* \param diffuse Color for diffuse
	*/
	inline void Material::SetDiffuseColor(const Color& diffuse)
	{
		m_diffuseColor = diffuse;
	}

	/*!
	* \brief Sets the diffuse map by name
	* \return true If successful
	*
	* \param textureName Named texture
	*
	* \remark Invalidates the pipeline
	*/
	inline bool Material::SetDiffuseMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get diffuse map \"" + textureName + "\"");
				return false;
			}
		}

		SetDiffuseMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the diffuse map with a reference to a texture
	* \return true If successful
	*
	* \param diffuseMap Texture
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetDiffuseMap(TextureRef diffuseMap)
	{
		m_diffuseMap = std::move(diffuseMap);
		m_pipelineInfo.hasDiffuseMap = m_diffuseMap.IsValid();

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the diffuse sampler
	*
	* \param sampler Diffuse sample
	*/

	inline void Material::SetDiffuseSampler(const TextureSampler& sampler)
	{
		m_diffuseSampler = sampler;
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
	* \brief Sets the emissive map by name
	* \return true If successful
	*
	* \param textureName Named texture
	*
	* \see GetEmissiveMap
	*/
	inline bool Material::SetEmissiveMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get emissive map \"" + textureName + "\"");
				return false;
			}
		}

		SetEmissiveMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the emissive map with a reference to a texture
	* \return true If successful
	*
	* \param emissiveMap Texture
	*
	* \remark Invalidates the pipeline
	*/
	inline void Material::SetEmissiveMap(TextureRef emissiveMap)
	{
		m_emissiveMap = std::move(emissiveMap);
		m_pipelineInfo.hasEmissiveMap = m_emissiveMap.IsValid();

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
	* \brief Sets the height map by path or name
	* \return true If successful
	*
	* \param textureName Named texture
	*
	* \see GetHeightMap
	*/
	inline bool Material::SetHeightMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get height map \"" + textureName + "\"");
				return false;
			}
		}

		SetHeightMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the height map with a reference to a texture
	*
	* \param heightMap Texture
	*
	* \remark Invalidates the pipeline
	*
	* \see GetHeightMap
	*/
	inline void Material::SetHeightMap(TextureRef heightMap)
	{
		m_heightMap = std::move(heightMap);
		m_pipelineInfo.hasHeightMap = m_heightMap.IsValid();

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
	* \brief Sets the normal map by path or name
	* \return true If successful
	*
	* \param textureName Named texture
	*
	* \remark Invalidates the pipeline
	*
	* \see GetNormalMap
	*/
	inline bool Material::SetNormalMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get normal map \"" + textureName + "\"");
				return false;
			}
		}

		SetNormalMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the normal map with a reference to a texture
	* \return true If successful
	*
	* \param normalMap Texture
	*
	* \remark Invalidates the pipeline
	*
	* \see GetNormalMap
	*/
	inline void Material::SetNormalMap(TextureRef normalMap)
	{
		m_normalMap = std::move(normalMap);
		m_pipelineInfo.hasNormalMap = m_normalMap.IsValid();

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

	/*!
	* \brief Sets the shininess of the material
	*
	* \param shininess Value of the shininess
	*/
	inline void Material::SetShininess(float shininess)
	{
		m_shininess = shininess;
	}

	/*!
	* \brief Sets the color for specular
	*
	* \param specular Color
	*/
	inline void Material::SetSpecularColor(const Color& specular)
	{
		m_specularColor = specular;
	}

	/*!
	* \brief Sets the specular map by name
	* \return true If successful
	*
	* \param textureName Named texture
	*
	* \remark Invalidates the pipeline
	*/
	inline bool Material::SetSpecularMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get specular map \"" + textureName + "\"");
				return false;
			}
		}

		SetSpecularMap(std::move(texture));
		return true;
	}

	/*!
	* \brief Sets the specular map with a reference to a texture
	* \return true If successful
	*
	* \param specularMap Texture
	*
	* \remark Invalidates the pipeline
	*
	* \see GetSpecularMap
	*/
	inline void Material::SetSpecularMap(TextureRef specularMap)
	{
		m_specularMap = std::move(specularMap);
		m_pipelineInfo.hasSpecularMap = m_specularMap.IsValid();

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the specular sampler
	*
	* \param sampler Specular sample
	*
	* \see GetSpecularSampler
	*/
	inline void Material::SetSpecularSampler(const TextureSampler& sampler)
	{
		m_specularSampler = sampler;
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

		Copy(material);
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
