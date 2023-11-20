// this file was automatically generated and should not be edited

/*
	Nazara Engine - Graphics module

	Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#ifndef NAZARA_GLOBAL_GRAPHICS_HPP
#define NAZARA_GLOBAL_GRAPHICS_HPP

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FrameGraphStructs.hpp>
#include <Nazara/Graphics/FramePass.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/FramePipelinePassRegistry.hpp>
#include <Nazara/Graphics/GraphicalMesh.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/LinearSlicedSprite.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/MaterialPassRegistry.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/PipelinePassList.hpp>
#include <Nazara/Graphics/PipelineViewer.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PointLightShadowData.hpp>
#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/PredefinedMaterials.hpp>
#include <Nazara/Graphics/PredefinedShaderStructBuilder.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderBufferPool.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/ShaderReflection.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/SpotLightShadowData.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/SpriteChainRenderer.hpp>
#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Graphics/TextureSamplerCache.hpp>
#include <Nazara/Graphics/Tilemap.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>

#ifdef NAZARA_ENTT

#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>

#endif

#endif // NAZARA_GLOBAL_GRAPHICS_HPP
