// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/DeferredBloomPass.hpp>
#include <Nazara/Graphics/DeferredDOFPass.hpp>
#include <Nazara/Graphics/DeferredFinalPass.hpp>
#include <Nazara/Graphics/DeferredFogPass.hpp>
#include <Nazara/Graphics/DeferredForwardPass.hpp>
#include <Nazara/Graphics/DeferredFXAAPass.hpp>
#include <Nazara/Graphics/DeferredGeometryPass.hpp>
#include <Nazara/Graphics/DeferredPhongLightingPass.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <limits>
#include <memory>
#include <random>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	unsigned int RenderPassPriority[] =
	{
		6,    // nzRenderPassType_AA
		4,    // nzRenderPassType_Bloom
		7,    // nzRenderPassType_DOF
		0xFF, // nzRenderPassType_Final
		5,    // nzRenderPassType_Fog
		2,    // nzRenderPassType_Forward
		1,    // nzRenderPassType_Lighting
		0,    // nzRenderPassType_Geometry
		3,    // nzRenderPassType_SSAO
	};

	static_assert(sizeof(RenderPassPriority)/sizeof(unsigned int) == nzRenderPassType_Max+1, "Render pass priority array is incomplete");
}

NzDeferredRenderTechnique::NzDeferredRenderTechnique() :
m_renderQueue(static_cast<NzForwardRenderQueue*>(m_forwardTechnique.GetRenderQueue())),
m_GBufferSize(0U)
{
	m_depthStencilBuffer = new NzRenderBuffer;
	m_depthStencilBuffer->SetPersistent(false);

	for (unsigned int i = 0; i < 2; ++i)
	{
		m_workTextures[i] = new NzTexture;
		m_workTextures[i]->SetPersistent(false);
	}

	for (unsigned int i = 0; i < 3; ++i)
	{
		m_GBuffer[i] = new NzTexture;
		m_GBuffer[i]->SetPersistent(false);
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		std::unique_ptr<NzDeferredRenderPass> smartPtr; // Nous évite un leak en cas d'exception

		ResetPass(nzRenderPassType_Final, 0);
		ResetPass(nzRenderPassType_Geometry, 0);
		ResetPass(nzRenderPassType_Lighting, 0);
	}
	catch (const std::exception& e)
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowExceptionDisabled);

		NazaraError("Failed to add geometry and/or phong lighting pass");
		throw;
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		ResetPass(nzRenderPassType_AA, 0);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add FXAA pass");
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		ResetPass(nzRenderPassType_Bloom, 0);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add bloom pass");
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);

		NzDeferredRenderPass* dofPass = ResetPass(nzRenderPassType_DOF, 0);
		dofPass->Enable(false);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add DOF pass");
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);

		NzDeferredRenderPass* fogPass = ResetPass(nzRenderPassType_Fog, 0);
		fogPass->Enable(false);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add fog pass");
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		ResetPass(nzRenderPassType_Forward, 0);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add forward pass");
	}

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		ResetPass(nzRenderPassType_SSAO, 0);
	}
	catch (const std::exception& e)
	{
		NazaraWarning("Failed to add SSAO pass");
	}
}

NzDeferredRenderTechnique::~NzDeferredRenderTechnique() = default;

void NzDeferredRenderTechnique::Clear(const NzScene* scene) const
{
	NazaraUnused(scene);
}

bool NzDeferredRenderTechnique::Draw(const NzScene* scene) const
{
	NzRecti viewerViewport = scene->GetViewer()->GetViewport();

	NzVector2ui viewportDimensions(viewerViewport.width, viewerViewport.height);
	if (viewportDimensions != m_GBufferSize)
	{
		if (!Resize(viewportDimensions))
		{
			NazaraError("Failed to update RTT");
			return false;
		}
	}

	unsigned int sceneTexture = 0;
	unsigned int workTexture = 1;
	for (auto& passIt : m_passes)
	{
		for (auto& passIt2 : passIt.second)
		{
			const NzDeferredRenderPass* pass = passIt2.second.get();
			if (pass->IsEnabled())
			{
				if (pass->Process(scene, workTexture, sceneTexture))
					std::swap(workTexture, sceneTexture);
			}
		}
	}

	return true;
}

void NzDeferredRenderTechnique::EnablePass(nzRenderPassType renderPass, int position, bool enable)
{
	auto it = m_passes.find(renderPass);
	if (it != m_passes.end())
	{
		auto it2 = it->second.find(position);
		if (it2 != it->second.end())
			it2->second->Enable(enable);
	}
}

NzRenderBuffer* NzDeferredRenderTechnique::GetDepthStencilBuffer() const
{
	return m_depthStencilBuffer;
}

NzTexture* NzDeferredRenderTechnique::GetGBuffer(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 3)
	{
		NazaraError("GBuffer texture index out of range (" + NzString::Number(i) + " >= 3)");
		return nullptr;
	}
	#endif

	return m_GBuffer[i];
}

NzRenderTexture* NzDeferredRenderTechnique::GetGBufferRTT() const
{
	return &m_GBufferRTT;
}

const NzForwardRenderTechnique* NzDeferredRenderTechnique::GetForwardTechnique() const
{
	return &m_forwardTechnique;
}

NzDeferredRenderPass* NzDeferredRenderTechnique::GetPass(nzRenderPassType renderPass, int position)
{
	auto it = m_passes.find(renderPass);
	if (it != m_passes.end())
	{
		auto it2 = it->second.find(position);
		if (it2 != it->second.end())
			return it2->second.get();
	}

	return nullptr;
}

NzAbstractRenderQueue* NzDeferredRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzDeferredRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_DeferredShading;
}

NzRenderTexture* NzDeferredRenderTechnique::GetWorkRTT() const
{
	return &m_workRTT;
}

NzTexture* NzDeferredRenderTechnique::GetWorkTexture(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 2)
	{
		NazaraError("Work texture index out of range (" + NzString::Number(i) + " >= 2)");
		return nullptr;
	}
	#endif

	return m_workTextures[i];
}

bool NzDeferredRenderTechnique::IsPassEnabled(nzRenderPassType renderPass, int position)
{
	auto it = m_passes.find(renderPass);
	if (it != m_passes.end())
	{
		auto it2 = it->second.find(position);
		if (it2 != it->second.end())
			return it2->second->IsEnabled();
	}

	return false;
}

NzDeferredRenderPass* NzDeferredRenderTechnique::ResetPass(nzRenderPassType renderPass, int position)
{
	std::unique_ptr<NzDeferredRenderPass> smartPtr; // Nous évite un leak en cas d'exception

	switch (renderPass)
	{
		case nzRenderPassType_AA:
			smartPtr.reset(new NzDeferredFXAAPass);
			break;

		case nzRenderPassType_Bloom:
			smartPtr.reset(new NzDeferredBloomPass);
			break;

		case nzRenderPassType_DOF:
			smartPtr.reset(new NzDeferredDOFPass);
			break;

		case nzRenderPassType_Final:
			smartPtr.reset(new NzDeferredFinalPass);
			break;

		case nzRenderPassType_Fog:
			smartPtr.reset(new NzDeferredFogPass);
			break;

		case nzRenderPassType_Forward:
			smartPtr.reset(new NzDeferredForwardPass);
			break;

		case nzRenderPassType_Geometry:
			smartPtr.reset(new NzDeferredGeometryPass);
			break;

		case nzRenderPassType_Lighting:
			smartPtr.reset(new NzDeferredPhongLightingPass);
			break;

		case nzRenderPassType_SSAO:
			//smartPtr.reset(new NzDeferredSSAOPass);
			break;
	}

	NzDeferredRenderPass* oldPass = GetPass(renderPass, position);
	if (oldPass && !oldPass->IsEnabled())
		smartPtr->Enable(false);

    SetPass(renderPass, position, smartPtr.get());
    return smartPtr.release();
}

void NzDeferredRenderTechnique::SetPass(nzRenderPassType relativeTo, int position, NzDeferredRenderPass* pass)
{
	if (pass)
	{
		pass->Initialize(this);
		if (m_GBufferSize != NzVector2ui(0U))
			pass->Resize(m_GBufferSize);

		m_passes[relativeTo][position].reset(pass);
	}
	else
		m_passes[relativeTo].erase(position);
}

bool NzDeferredRenderTechnique::IsSupported()
{
	// On ne va pas s'embêter à écrire un Deferred Renderer qui ne passe pas par le MRT, ce serait lent et inutile (OpenGL 2 garanti cette fonctionnalité en plus)
	return NzRenderer::HasCapability(nzRendererCap_RenderTexture) &&
	       NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets) &&
	       NzRenderer::GetMaxColorAttachments() >= 4 &&
	       NzRenderer::GetMaxRenderTargets() >= 4 &&
	       NzTexture::IsFormatSupported(nzPixelFormat_RGBA32F);
}

bool NzDeferredRenderTechnique::Resize(const NzVector2ui& dimensions) const
{
	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);

		for (auto& passIt : m_passes)
			for (auto& passIt2 : passIt.second)
				passIt2.second->Resize(dimensions);

		m_GBufferSize = dimensions;

		return true;
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to create work RTT/G-Buffer");
		return false;
	}
}

bool NzDeferredRenderTechnique::RenderPassComparator::operator()(nzRenderPassType pass1, nzRenderPassType pass2)
{
	return RenderPassPriority[pass1] < RenderPassPriority[pass2];
}
