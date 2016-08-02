// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Material object by default
	*/

	inline Material::Material()
	{
		Reset();
	}

	/*!
	* \brief Constructs a Material object by assignation
	*
	* \param material Material to copy into this
	*/

	inline Material::Material(const Material& material) :
	RefCounted(),
	Resource(material)
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
	* \brief Enables a renderer parameter
	*
	* \param renderParameter Parameter for the rendering
	* \param enable Should the parameter be enabled
	*
	* \remark Produces a NazaraAssert if enumeration is invalid
	*/

	inline void Material::Enable(RendererParameter renderParameter, bool enable)
	{
		NazaraAssert(renderParameter <= RendererParameter_Max, "Renderer parameter out of enum");

		switch (renderParameter)
		{
			case RendererParameter_Blend:
				m_states.blending = enable;
				return;

			case RendererParameter_ColorWrite:
				m_states.colorWrite = enable;
				return;

			case RendererParameter_DepthBuffer:
				m_states.depthBuffer = enable;
				return;

			case RendererParameter_DepthWrite:
				m_states.depthWrite = enable;
				return;

			case RendererParameter_FaceCulling:
				m_states.faceCulling = enable;
				return;

			case RendererParameter_ScissorTest:
				m_states.scissorTest = enable;
				return;

			case RendererParameter_StencilTest:
				m_states.stencilTest = enable;
				return;
		}
	}

	/*!
	* \brief Enables the alpha test
	*
	* \param alphaTest Should the parameter be enabled
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::EnableAlphaTest(bool alphaTest)
	{
		m_alphaTestEnabled = alphaTest;

		InvalidateShaders();
	}

	/*!
	* \brief Enables the depth sorting
	*
	* \param depthSorting Should the parameter be enabled
	*/

	inline void Material::EnableDepthSorting(bool depthSorting)
	{
		// Has no influence on shaders
		m_depthSortingEnabled = depthSorting;
	}

	/*!
	* \brief Enables the lighting
	*
	* \param lighting Should the parameter be enabled
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::EnableLighting(bool lighting)
	{
		m_lightingEnabled = lighting;

		InvalidateShaders();
	}

	/*!
	* \brief Enables the shadow casting
	*
	* \param castShadows Should shadow casting be enabled
	*/

	inline void Material::EnableShadowCasting(bool castShadows)
	{
		// Has no influence on shaders
		m_shadowCastingEnabled = castShadows;
	}

	/*!
	* \brief Enables the shadow on receiving object
	*
	* \param receiveShadow Should receiving object have shadows  enabled
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::EnableShadowReceive(bool receiveShadows)
	{
		m_shadowReceiveEnabled = receiveShadows;

		InvalidateShaders();
	}

	/*!
	* \brief Enables the transformation
	*
	* \param transform Should the parameter be enabled
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::EnableTransform(bool transform)
	{
		m_transformEnabled = transform;

		InvalidateShaders();
	}

	/*!
	* \brief Gets the alpha map
	* \return Constant reference to the current texture
	*/

	inline const TextureRef& Material::GetAlphaMap() const
	{
		return m_alphaMap;
	}

	/*!
	* \brief Gets the alpha threshold
	* \return The threshold value for the alpha
	*/

	inline float Material::GetAlphaThreshold() const
	{
		return m_alphaThreshold;
	}

	/*!
	* \brief Gets the ambient color
	* \return Ambient color
	*/

	inline Color Material::GetAmbientColor() const
	{
		return m_ambientColor;
	}

	/*!
	* \brief Gets the function to compare depth
	* \return Function comparing the depth of two materials
	*/

	inline RendererComparison Material::GetDepthFunc() const
	{
		return m_states.depthFunc;
	}

	/*!
	* \brief Gets the depth material
	* \return Constant reference to the depth material
	*/

	inline const MaterialRef& Material::GetDepthMaterial() const
	{
		return m_depthMaterial;
	}

	/*!
	* \brief Gets the diffuse color
	* \return Diffuse color
	*/

	inline Color Material::GetDiffuseColor() const
	{
		return m_diffuseColor;
	}

	/*!
	* \brief Gets the diffuse sampler
	* \return Reference to the current texture sampler for the diffuse
	*/

	inline TextureSampler& Material::GetDiffuseSampler()
	{
		return m_diffuseSampler;
	}

	/*!
	* \brief Gets the diffuse sampler
	* \return Constant reference to the current texture sampler for the diffuse
	*/

	inline const TextureSampler& Material::GetDiffuseSampler() const
	{
		return m_diffuseSampler;
	}

	/*!
	* \brief Gets the diffuse map
	* \return Constant reference to the texture
	*/

	const TextureRef& Material::GetDiffuseMap() const
	{
		return m_diffuseMap;
	}

	/*!
	* \brief Gets the dst in blend
	* \return Function for dst blending
	*/

	inline BlendFunc Material::GetDstBlend() const
	{
		return m_states.dstBlend;
	}

	/*!
	* \brief Gets the emissive map
	* \return Constant reference to the texture
	*/

	inline const TextureRef& Material::GetEmissiveMap() const
	{
		return m_emissiveMap;
	}

	/*!
	* \brief Gets the face culling
	* \return Current face culling side
	*/

	inline FaceSide Material::GetFaceCulling() const
	{
		return m_states.cullingSide;
	}

	/*!
	* \brief Gets the face filling
	* \return Current face filling
	*/

	inline FaceFilling Material::GetFaceFilling() const
	{
		return m_states.faceFilling;
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

	inline const RenderStates& Material::GetRenderStates() const
	{
		return m_states;
	}

	/*!
	* \brief Gets the shader of this material
	* \return Constant pointer to the ubershader used
	*/

	inline const UberShader* Material::GetShader() const
	{
		return m_uberShader;
	}

	/*!
	* \brief Gets the shader instance based on the flag
	* \return Constant pointer to the ubershader instance
	*
	* \param flags Flag of the shader
	*/

	inline const UberShaderInstance* Material::GetShaderInstance(UInt32 flags) const
	{
		const ShaderInstance& instance = m_shaders[flags];
		if (!instance.uberInstance)
			GenerateShader(flags);

		return instance.uberInstance;
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
		return m_states.srcBlend;
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
	* \brief Checks whether this material has alpha test enabled
	* \return true If it is the case
	*/

	inline bool Material::IsAlphaTestEnabled() const
	{
		return m_alphaTestEnabled;
	}

	/*!
	* \brief Checks whether this material has depth sorting enabled
	* \return true If it is the case
	*/

	inline bool Material::IsDepthSortingEnabled() const
	{
		return m_depthSortingEnabled;
	}

	/*!
	* \brief Checks whether this material has the render parameter enabled
	* \return true If it is the case
	*
	* \param parameter Parameter for the rendering
	*
	* \remark Produces a NazaraAssert if enumeration is invalid
	*/

	inline bool Material::IsEnabled(RendererParameter parameter) const
	{
		NazaraAssert(parameter <= RendererParameter_Max, "Renderer parameter out of enum");

		switch (parameter)
		{
			case RendererParameter_Blend:
				return m_states.blending;

			case RendererParameter_ColorWrite:
				return m_states.colorWrite;

			case RendererParameter_DepthBuffer:
				return m_states.depthBuffer;

			case RendererParameter_DepthWrite:
				return m_states.depthWrite;

			case RendererParameter_FaceCulling:
				return m_states.faceCulling;

			case RendererParameter_ScissorTest:
				return m_states.scissorTest;

			case RendererParameter_StencilTest:
				return m_states.stencilTest;
		}

		NazaraInternalError("Unhandled renderer parameter: 0x" + String::Number(parameter, 16));
		return false;
	}

	/*!
	* \brief Checks whether this material has lightning enabled
	* \return true If it is the case
	*/

	inline bool Material::IsLightingEnabled() const
	{
		return m_lightingEnabled;
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
		return m_shadowReceiveEnabled;
	}

	/*!
	* \brief Checks whether this material has transformation enabled
	* \return true If it is the case
	*/

	inline bool Material::IsTransformEnabled() const
	{
		return m_transformEnabled;
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
	* \remark Invalidates the shaders
	*/

	inline void Material::SetAlphaMap(TextureRef alphaMap)
	{
		m_alphaMap = std::move(alphaMap);

		InvalidateShaders();
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
	*/

	inline void Material::SetDepthFunc(RendererComparison depthFunc)
	{
		m_states.depthFunc = depthFunc;
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
	* \remark Invalidates the shaders
	*/

	inline void Material::SetDiffuseMap(TextureRef diffuseMap)
	{
		m_diffuseMap = std::move(diffuseMap);

		InvalidateShaders();
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
	*/

	inline void Material::SetDstBlend(BlendFunc func)
	{
		m_states.dstBlend = func;
	}

	/*!
	* \brief Sets the emissive map by name
	* \return true If successful
	*
	* \param textureName Named texture
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
	* \remark Invalidates the shaders
	*/

	inline void Material::SetEmissiveMap(TextureRef emissiveMap)
	{
		m_emissiveMap = std::move(emissiveMap);

		InvalidateShaders();
	}

	/*!
	* \brief Sets the face culling
	*
	* \param faceSide Face to cull
	*/

	inline void Material::SetFaceCulling(FaceSide faceSide)
	{
		m_states.cullingSide = faceSide;
	}

	/*!
	* \brief Sets the face filling
	*
	* \param filling Face to fill
	*/

	inline void Material::SetFaceFilling(FaceFilling filling)
	{
		m_states.faceFilling = filling;
	}

	/*!
	* \brief Sets the height map by name
	* \return true If successful
	*
	* \param textureName Named texture
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
	* \return true If successful
	*
	* \param heightMap Texture
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::SetHeightMap(TextureRef heightMap)
	{
		m_heightMap = std::move(heightMap);

		InvalidateShaders();
	}

	/*!
	* \brief Sets the normal map by name
	* \return true If successful
	*
	* \param textureName Named texture
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
	* \remark Invalidates the shaders
	*/

	inline void Material::SetNormalMap(TextureRef normalMap)
	{
		m_normalMap = std::move(normalMap);

		InvalidateShaders();
	}

	/*!
	* \brief Sets the render states
	*
	* \param states States for the rendering
	*/

	inline void Material::SetRenderStates(const RenderStates& states)
	{
		m_states = states;
	}

	/*!
	* \brief Sets the shader with a constant reference to a ubershader
	*
	* \param uberShader Uber shader to apply
	*
	* \remark Invalidates the shaders
	*/

	inline void Material::SetShader(UberShaderConstRef uberShader)
	{
		m_uberShader = std::move(uberShader);

		InvalidateShaders();
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
	* \remark Invalidates the shaders
	*/

	inline void Material::SetSpecularMap(TextureRef specularMap)
	{
		m_specularMap = std::move(specularMap);

		InvalidateShaders();
	}

	/*!
	* \brief Sets the specular sampler
	*
	* \param sampler Specular sample
	*/

	inline void Material::SetSpecularSampler(const TextureSampler& sampler)
	{
		m_specularSampler = sampler;
	}

	/*!
	* \brief Sets the src in blend
	*
	* \param func Function for src blending
	*/

	inline void Material::SetSrcBlend(BlendFunc func)
	{
		m_states.srcBlend = func;
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
	* \return Reference to the default material
	*/

	inline MaterialRef Material::GetDefault()
	{
		return s_defaultMaterial;
	}

	/*!
	* \brief Invalidates the shaders
	*/

	inline void Material::InvalidateShaders()
	{
		for (ShaderInstance& instance : m_shaders)
			instance.uberInstance = nullptr;
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
		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
