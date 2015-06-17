// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEPTHRENDERTECHNIQUE_HPP
#define NAZARA_DEPTHRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

class NAZARA_GRAPHICS_API NzDepthRenderTechnique : public NzAbstractRenderTechnique
{
	public:
		NzDepthRenderTechnique();
		~NzDepthRenderTechnique() = default;

		bool Draw(const NzSceneData& sceneData) const override;

		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;

		static bool Initialize();
		static void Uninitialize();

	private:
		void DrawBasicSprites(const NzSceneData& sceneData) const;
		void DrawBillboards(const NzSceneData& sceneData) const;
		void DrawOpaqueModels(const NzSceneData& sceneData) const;

		NzBuffer m_vertexBuffer;
		mutable NzDepthRenderQueue m_renderQueue;
		NzVertexBuffer m_billboardPointBuffer;
		NzVertexBuffer m_spriteBuffer;

		static NzIndexBuffer s_quadIndexBuffer;
		static NzMaterialRef s_material;
		static NzVertexBuffer s_quadVertexBuffer;
		static NzVertexDeclaration s_billboardInstanceDeclaration;
		static NzVertexDeclaration s_billboardVertexDeclaration;
};

#include <Nazara/Graphics/DepthRenderTechnique.inl>

#endif // NAZARA_DEPTHRENDERTECHNIQUE_HPP
