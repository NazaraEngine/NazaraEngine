---
title: LightShadowData
description: Nothing
---

# Nz::LightShadowData

Class description

## Constructors

- `LightShadowData()`
- `LightShadowData(`LightShadowData` const&)`
- `LightShadowData(`LightShadowData`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `PrepareRendering(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `void` | `RegisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& matInstance)` |
| `void` | `RegisterPassInputs(`[`FramePass`](documentation/generated/Graphics/FramePass.md)`& pass)` |
| `void` | `RegisterToFrameGraph(`[`FrameGraph`](documentation/generated/Graphics/FrameGraph.md)`& frameGraph)` |
| [`Texture`](documentation/generated/Renderer/Texture.md)` const*` | `RetrieveLightShadowmap(`[`BakedFrameGraph`](documentation/generated/Graphics/BakedFrameGraph.md)` const& bakedGraph)` |
| `void` | `UnregisterMaterialInstance(`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)` const& matInstance)` |
| LightShadowData`&` | `operator=(`LightShadowData` const&)` |
| LightShadowData`&` | `operator=(`LightShadowData`&&)` |
