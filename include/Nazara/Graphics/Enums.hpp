// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_GRAPHICS_HPP
#define NAZARA_ENUMS_GRAPHICS_HPP

namespace Nz
{
	enum BackgroundType
	{
		BackgroundType_Color,   // ColorBackground
		BackgroundType_Skybox,  // SkyboxBackground
		BackgroundType_Texture, // TextureBackground
		BackgroundType_User,

		BackgroundType_Max = BackgroundType_User
	};

	enum class CullTest
	{
		NoTest,
		Sphere,
		Volume
	};

	enum ProjectionType
	{
		ProjectionType_Orthogonal,
		ProjectionType_Perspective,

		ProjectionType_Max = ProjectionType_Perspective
	};

	enum LightType
	{
		LightType_Directional,
		LightType_Point,
		LightType_Spot,

		LightType_Max = LightType_Spot
	};

	enum MaterialUniform
	{
		MaterialUniform_AlphaMap,
		MaterialUniform_AlphaThreshold,
		MaterialUniform_Ambient,
		MaterialUniform_Diffuse,
		MaterialUniform_DiffuseMap,
		MaterialUniform_EmissiveMap,
		MaterialUniform_HeightMap,
		MaterialUniform_NormalMap,
		MaterialUniform_Shininess,
		MaterialUniform_Specular,
		MaterialUniform_SpecularMap,

		MaterialUniform_Max = MaterialUniform_SpecularMap
	};

	enum ParticleComponent
	{
		ParticleComponent_Unused = -1,

		ParticleComponent_Color,
		ParticleComponent_Life,
		ParticleComponent_Mass,
		ParticleComponent_Normal,
		ParticleComponent_Position,
		ParticleComponent_Radius,
		ParticleComponent_Rotation,
		ParticleComponent_Size,
		ParticleComponent_Velocity,
		ParticleComponent_Userdata0,
		ParticleComponent_Userdata1,
		ParticleComponent_Userdata2,
		ParticleComponent_Userdata3,
		ParticleComponent_Userdata4,
		ParticleComponent_Userdata5,
		ParticleComponent_Userdata6,
		ParticleComponent_Userdata7,
		ParticleComponent_Userdata8,

		ParticleComponent_Max = ParticleComponent_Userdata8
	};

	enum ParticleLayout
	{
		ParticleLayout_Billboard,
		ParticleLayout_Model,
		ParticleLayout_Sprite,

		ParticleLayout_Max = ParticleLayout_Sprite
	};

	enum RenderPassType
	{
		RenderPassType_AA,
		RenderPassType_Bloom,
		RenderPassType_DOF,
		RenderPassType_Final,
		RenderPassType_Fog,
		RenderPassType_Forward,
		RenderPassType_Lighting,
		RenderPassType_Geometry,
		RenderPassType_SSAO,

		RenderPassType_Max = RenderPassType_SSAO
	};

	enum RenderTechniqueType
	{
		RenderTechniqueType_AdvancedForward, // AdvancedForwardRenderTechnique
		RenderTechniqueType_BasicForward,    // BasicForwardRenderTechnique
		RenderTechniqueType_DeferredShading, // DeferredRenderTechnique
		RenderTechniqueType_Depth,           // DepthRenderTechnique
		RenderTechniqueType_LightPrePass,    // LightPrePassRenderTechnique
		RenderTechniqueType_User,

		RenderTechniqueType_Max = RenderTechniqueType_User
	};

	enum SceneNodeType
	{
		SceneNodeType_Light,           // Light
		SceneNodeType_Model,           // Model
		SceneNodeType_ParticleEmitter, // ParticleEmitter
		SceneNodeType_Root,            // SceneRoot
		SceneNodeType_Sprite,          // Sprite
		SceneNodeType_TextSprite,      // TextSprite
		SceneNodeType_User,

		SceneNodeType_Max = SceneNodeType_User
	};

	// These parameters are independant of the material: they can not be asked for the moment
	enum ShaderFlags
	{
		ShaderFlags_None = 0,

		ShaderFlags_Billboard      = 0x01,
		ShaderFlags_Deferred       = 0x02,
		ShaderFlags_Instancing     = 0x04,
		ShaderFlags_TextureOverlay = 0x08,
		ShaderFlags_VertexColor    = 0x10,

		ShaderFlags_Max = ShaderFlags_VertexColor * 2 - 1
	};

	enum TextureMap
	{
		TextureMap_Alpha,
		TextureMap_Diffuse,
		TextureMap_Emissive,
		TextureMap_Height,
		TextureMap_ReflectionCube,
		TextureMap_Normal,
		TextureMap_Overlay,
		TextureMap_Shadow2D_1,
		TextureMap_Shadow2D_2,
		TextureMap_Shadow2D_3,
		TextureMap_ShadowCube_1,
		TextureMap_ShadowCube_2,
		TextureMap_ShadowCube_3,
		TextureMap_Specular,

		TextureMap_Max = TextureMap_Specular
	};
}

#endif // NAZARA_ENUMS_GRAPHICS_HPP
