// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShaderTransfer.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/SpotLight.hpp>

namespace Nz
{
	void ShaderTransfer::WriteLight(const DirectionalLight* light, void* basePtr)
	{
		const Color& lightColor = light->GetColor();

		AccessByOffset<Vector3f&>(basePtr, PredefinedDirectionalLightOffsets.colorOffset) = Vector3f(lightColor.r, lightColor.g, lightColor.b) * light->GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedDirectionalLightOffsets.directionOffset) = light->GetDirection();
		AccessByOffset<float&>(basePtr, PredefinedDirectionalLightOffsets.ambientFactorOffset) = light->GetAmbientFactor();
		AccessByOffset<float&>(basePtr, PredefinedDirectionalLightOffsets.diffuseFactorOffset) = light->GetDiffuseFactor();
		AccessByOffset<Vector2f&>(basePtr, PredefinedDirectionalLightOffsets.invShadowMapSizeOffset) = (light->IsShadowCaster()) ? Vector2f(1.f / light->GetShadowMapSize()) : Vector2f(-1.f, -1.f);
	}

	void ShaderTransfer::WriteLight(const PointLight* light, void* basePtr)
	{
		const Color& lightColor = light->GetColor();

		AccessByOffset<Vector3f&>(basePtr, PredefinedPointLightOffsets.colorOffset) = Vector3f(lightColor.r, lightColor.g, lightColor.b) * light->GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedPointLightOffsets.positionOffset) = light->GetPosition();
		AccessByOffset<Vector2f&>(basePtr, PredefinedPointLightOffsets.invShadowMapSizeOffset) = (light->IsShadowCaster()) ? Vector2f(1.f / light->GetShadowMapSize()) : Vector2f(-1.f, -1.f);
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.ambientFactorOffset) = light->GetAmbientFactor();
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.diffuseFactorOffset) = light->GetDiffuseFactor();
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.radiusOffset) = light->GetRadius();
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.invRadiusOffset) = light->GetInvRadius();
	}

	void ShaderTransfer::WriteLight(const SpotLight* light, void* basePtr)
	{
		const Color& lightColor = light->GetColor();

		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.colorOffset) = Vector3f(lightColor.r, lightColor.g, lightColor.b) * light->GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.directionOffset) = light->GetDirection();
		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.positionOffset) = light->GetPosition();
		AccessByOffset<Vector2f&>(basePtr, PredefinedSpotLightOffsets.invShadowMapSizeOffset) = (light->IsShadowCaster()) ? Vector2f(1.f / light->GetShadowMapSize()) : Vector2f(-1.f, -1.f);
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.ambientFactorOffset) = light->GetAmbientFactor();
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.diffuseFactorOffset) = light->GetDiffuseFactor();
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.innerAngleOffset) = light->GetInnerAngleCos();
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.outerAngleOffset) = light->GetOuterAngleCos();
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.invRadiusOffset) = light->GetInvRadius();
		AccessByOffset<Matrix4f&>(basePtr, PredefinedSpotLightOffsets.viewProjMatrixOffset) = light->GetViewProjMatrix();

		float baseRadius = light->GetRadius() * light->GetOuterAngleTan() * 1.1f;
		AccessByOffset<Matrix4f&>(basePtr, PredefinedSpotLightOffsets.worldMatrixOffset) = Matrix4f::Transform(light->GetPosition(), light->GetRotation(), Vector3f(baseRadius, baseRadius, light->GetRadius()));
	}

	void ShaderTransfer::WriteLightShadowData(const AbstractViewer* viewer, const DirectionalLightShadowData* shadowData, void* basePtr)
	{
		float* cascadeFarPlanes = AccessByOffset<float*>(basePtr, PredefinedDirectionalLightOffsets.cascadeFarPlanesOffset);
		Matrix4f* cascadeViewProj = AccessByOffset<Matrix4f*>(basePtr, PredefinedDirectionalLightOffsets.cascadeViewProjMatricesOffset);

		shadowData->GetCascadeData(viewer, SparsePtr<float>(cascadeFarPlanes, 4 * sizeof(float)), SparsePtr(cascadeViewProj));

		AccessByOffset<UInt32&>(basePtr, PredefinedDirectionalLightOffsets.cascadeCountOffset) = SafeCast<UInt32>(shadowData->GetCascadeCount());
	}
}
