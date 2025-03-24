---
title: SpotLightShadowData
description: Nothing
---

# Nz::SpotLightShadowData

Class description

## Constructors

- `SpotLightShadowData(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& pipeline, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry, `[`SpotLight`](documentation/generated/Graphics/SpotLight.md)` const& light)`
- `SpotLightShadowData(`SpotLightShadowData` const&)`
- `SpotLightShadowData(`SpotLightShadowData`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `PrepareRendering(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `void` | `RegisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& matInstance)` |
| `void` | `RegisterPassInputs(`[`FramePass`](documentation/generated/Graphics/FramePass.md)`& pass)` |
| `void` | `RegisterToFrameGraph(`[`FrameGraph`](documentation/generated/Graphics/FrameGraph.md)`& frameGraph)` |
| [`Texture`](documentation/generated/Renderer/Texture.md)` const*` | `RetrieveLightShadowmap(`[`BakedFrameGraph`](documentation/generated/Graphics/BakedFrameGraph.md)` const& bakedGraph)` |
| `void` | `UnregisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& matInstance)` |
| SpotLightShadowData`&` | `operator=(`SpotLightShadowData` const&)` |
| SpotLightShadowData`&` | `operator=(`SpotLightShadowData`&&)` |
