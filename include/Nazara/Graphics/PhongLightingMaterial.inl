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
	inline PhongLightingMaterial::PhongLightingMaterial()
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
	inline PhongLightingMaterial::PhongLightingMaterial(const MaterialPipeline* pipeline)
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
	inline PhongLightingMaterial::PhongLightingMaterial(const MaterialPipelineInfo& pipelineInfo)
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
	inline PhongLightingMaterial::PhongLightingMaterial(const String& pipelineName)
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
	inline PhongLightingMaterial::PhongLightingMaterial(const PhongLightingMaterial& material) :
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
	inline PhongLightingMaterial::~PhongLightingMaterial()
	{
		OnMaterialRelease(this);
	}

	/*!
	* \brief Gets the alpha map
	*
	* \return Constant reference to the current texture
	*
	* \see SetAlphaMap
	*/
	inline const TextureRef& PhongLightingMaterial::GetAlphaMap() const
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
	inline float PhongLightingMaterial::GetAlphaThreshold() const
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
	inline Color PhongLightingMaterial::GetAmbientColor() const
	{
		return m_ambientColor;
	}

	/*!
	* \brief Gets the diffuse color
	*
	* \return Diffuse color
	*
	* \see SetDiffuseColor
	*/
	inline Color PhongLightingMaterial::GetDiffuseColor() const
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
	inline TextureSampler& PhongLightingMaterial::GetDiffuseSampler()
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
	inline const TextureSampler& PhongLightingMaterial::GetDiffuseSampler() const
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
	const TextureRef& PhongLightingMaterial::GetDiffuseMap() const
	{
		return m_diffuseMap;
	}

	/*!
	* \brief Gets the emissive map
	*
	* \return Constant reference to the texture
	*
	* \see SetEmissiveMap
	*/
	inline const TextureRef& PhongLightingMaterial::GetEmissiveMap() const
	{
		return m_emissiveMap;
	}

	/*!
	* \brief Gets the height map
	* \return Constant reference to the texture
	*/
	inline const TextureRef& PhongLightingMaterial::GetHeightMap() const
	{
		return m_heightMap;
	}

	/*!
	* \brief Gets the normal map
	* \return Constant reference to the texture
	*/

	inline const TextureRef& PhongLightingMaterial::GetNormalMap() const
	{
		return m_normalMap;
	}

	/*!
	* \brief Gets the point size of this material
	* \return Point size
	*/
	inline float PhongLightingMaterial::GetPointSize() const
	{
		return m_pipelineInfo.pointSize;
	}

	/*!
	* \brief Gets the shininess
	* \return Current shininess
	*/
	inline float PhongLightingMaterial::GetShininess() const
	{
		return m_shininess;
	}

	/*!
	* \brief Gets the specular color
	* \return Specular color
	*/
	inline Color PhongLightingMaterial::GetSpecularColor() const
	{
		return m_specularColor;
	}

	/*!
	* \brief Gets the specular map
	* \return Constant reference to the texture
	*/
	inline const TextureRef& PhongLightingMaterial::GetSpecularMap() const
	{
		return m_specularMap;
	}

	/*!
	* \brief Gets the specular sampler
	* \return Reference to the current texture sampler for the specular
	*/
	inline TextureSampler& PhongLightingMaterial::GetSpecularSampler()
	{
		return m_specularSampler;
	}

	/*!
	* \brief Gets the specular sampler
	* \return Constant reference to the current texture sampler for the specular
	*/
	inline const TextureSampler& PhongLightingMaterial::GetSpecularSampler() const
	{
		return m_specularSampler;
	}

	/*!
	* \brief Checks whether this material has an alpha map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasAlphaMap() const
	{
		return m_alphaMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a diffuse map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasDiffuseMap() const
	{
		return m_diffuseMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a emissive map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasEmissiveMap() const
	{
		return m_emissiveMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a height map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasHeightMap() const
	{
		return m_heightMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a normal map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasNormalMap() const
	{
		return m_normalMap.IsValid();
	}

	/*!
	* \brief Checks whether this material has a specular map
	* \return true If it is the case
	*/
	inline bool PhongLightingMaterial::HasSpecularMap() const
	{
		return m_specularMap.IsValid();
	}

	/*!
	* \brief Sets the alpha map by name
	* \return true If successful
	*
	* \param textureName Named texture
	*/
	inline bool PhongLightingMaterial::SetAlphaMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetAlphaMap(TextureRef alphaMap)
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
	inline void PhongLightingMaterial::SetAlphaThreshold(float alphaThreshold)
	{
		m_alphaThreshold = alphaThreshold;
	}

	/*!
	* \brief Sets the color for ambient
	*
	* \param ambient Color for ambient
	*/
	inline void PhongLightingMaterial::SetAmbientColor(const Color& ambient)
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
	inline void PhongLightingMaterial::SetDepthFunc(RendererComparison depthFunc)
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
	inline void PhongLightingMaterial::SetDepthMaterial(MaterialRef depthMaterial)
	{
		m_depthMaterial = std::move(depthMaterial);
	}

	/*!
	* \brief Sets the color for diffuse
	*
	* \param diffuse Color for diffuse
	*/
	inline void PhongLightingMaterial::SetDiffuseColor(const Color& diffuse)
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
	inline bool PhongLightingMaterial::SetDiffuseMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetDiffuseMap(TextureRef diffuseMap)
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

	inline void PhongLightingMaterial::SetDiffuseSampler(const TextureSampler& sampler)
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
	inline void PhongLightingMaterial::SetDstBlend(BlendFunc func)
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
	inline bool PhongLightingMaterial::SetEmissiveMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetEmissiveMap(TextureRef emissiveMap)
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
	inline void PhongLightingMaterial::SetFaceCulling(FaceSide faceSide)
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
	inline void PhongLightingMaterial::SetFaceFilling(FaceFilling filling)
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
	inline bool PhongLightingMaterial::SetHeightMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetHeightMap(TextureRef heightMap)
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
	inline void PhongLightingMaterial::SetLineWidth(float lineWidth)
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
	inline bool PhongLightingMaterial::SetNormalMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetNormalMap(TextureRef normalMap)
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
	inline void PhongLightingMaterial::SetPointSize(float pointSize)
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
	inline void PhongLightingMaterial::SetReflectionMode(ReflectionMode reflectionMode)
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
	inline void PhongLightingMaterial::SetShader(UberShaderConstRef uberShader)
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
	inline bool PhongLightingMaterial::SetShader(const String& uberShaderName)
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
	inline void PhongLightingMaterial::SetShininess(float shininess)
	{
		m_shininess = shininess;
	}

	/*!
	* \brief Sets the color for specular
	*
	* \param specular Color
	*/
	inline void PhongLightingMaterial::SetSpecularColor(const Color& specular)
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
	inline bool PhongLightingMaterial::SetSpecularMap(const String& textureName)
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
	inline void PhongLightingMaterial::SetSpecularMap(TextureRef specularMap)
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
	inline void PhongLightingMaterial::SetSpecularSampler(const TextureSampler& sampler)
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
	inline void PhongLightingMaterial::SetSrcBlend(BlendFunc func)
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
	inline PhongLightingMaterial& PhongLightingMaterial::operator=(const PhongLightingMaterial& material)
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
	inline MaterialRef PhongLightingMaterial::GetDefault()
	{
		return s_defaultMaterial;
	}

	inline int PhongLightingMaterial::GetTextureUnit(TextureMap textureMap)
	{
		return s_textureUnits[textureMap];
	}

	inline void PhongLightingMaterial::InvalidatePipeline()
	{
		m_pipelineUpdated = false;
	}

	inline void PhongLightingMaterial::UpdatePipeline() const
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
	MaterialRef PhongLightingMaterial::New(Args&&... args)
	{
		std::unique_ptr<PhongLightingMaterial> object(new PhongLightingMaterial(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
