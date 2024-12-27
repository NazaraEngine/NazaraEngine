// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Core/MaterialData.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/RenderResourceReferences.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>

namespace Nz
{
	bool MaterialInstanceParams::IsValid() const
	{
		return true;
	}

	MaterialInstance::MaterialInstance(std::shared_ptr<const Material> parent) :
	m_parent(std::move(parent)),
	m_materialSettings(m_parent->GetSettings())
	{
		const auto& settings = m_parent->GetSettings();
		m_textureOverride.resize(settings.GetTexturePropertyCount());
		for (std::size_t i = 0; i < m_textureOverride.size(); ++i)
			m_textureOverride[i].samplerInfo = settings.GetTextureProperty(i).defaultSamplerInfo;

		m_bufferOverride.resize(settings.GetBufferPropertyCount());
		m_valueOverride.resize(settings.GetValuePropertyCount());

		const auto& passSettings = settings.GetPasses();
		m_passes.resize(passSettings.size());
		for (std::size_t i = 0; i < m_passes.size(); ++i)
		{
			const auto& passSettingOpt = passSettings[i];
			if (!passSettingOpt.has_value())
				continue;

			const auto& passSetting = *passSettingOpt;

			auto& pass = m_passes[i];
			pass.enabled = true;
			pass.flags = passSetting.flags;

			static_cast<RenderStates&>(pass.pipelineInfo) = passSetting.states;

			pass.shaders.reserve(passSetting.shaders.size());
			for (const std::shared_ptr<UberShader>& uberShader : passSetting.shaders)
			{
				auto& shaderEntry = pass.shaders.emplace_back();
				shaderEntry.shader = uberShader;
				shaderEntry.onShaderUpdated.Connect(shaderEntry.shader->OnShaderUpdated, [this, passIndex = i](UberShader*)
				{
					InvalidatePassPipeline(passIndex);
				});

				auto& pipelineShaderEntry = pass.pipelineInfo.shaders.emplace_back();
				pipelineShaderEntry.uberShader = uberShader;
			}
		}

		m_storageBufferBindings.resize(m_parent->GetStorageBufferCount());
		m_textureBinding.resize(m_parent->GetTextureCount());
		m_uniformBuffers.resize(m_parent->GetUniformBlockCount());
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uniformBlockData = m_parent->GetUniformBlockData(i);

			auto& uniformBuffer = m_uniformBuffers[i];
			std::tie(uniformBuffer.renderBuffer, uniformBuffer.bufferView) = uniformBlockData.bufferPool->Allocate(uniformBuffer.poolBufferIndex);
			uniformBuffer.values.resize(uniformBlockData.bufferPool->GetBufferSize());
		}

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
			handler->Update(*this);
	}

	MaterialInstance::MaterialInstance(const MaterialInstance& material, CopyToken) :
	m_parent(material.m_parent),
	m_optionValuesOverride(material.m_optionValuesOverride),
	m_bufferOverride(material.m_bufferOverride),
	m_valueOverride(material.m_valueOverride),
	m_storageBufferBindings(material.m_storageBufferBindings),
	m_textureBinding(material.m_textureBinding),
	m_textureOverride(material.m_textureOverride),
	m_materialSettings(material.m_materialSettings)
	{
		m_passes.resize(material.m_passes.size());
		for (std::size_t i = 0; i < m_passes.size(); ++i)
		{
			m_passes[i].enabled = material.m_passes[i].enabled;
			m_passes[i].flags = material.m_passes[i].flags;
			m_passes[i].pipeline = material.m_passes[i].pipeline;
			m_passes[i].pipelineInfo = material.m_passes[i].pipelineInfo;
			m_passes[i].shaders.resize(material.m_passes[i].shaders.size());
			for (std::size_t j = 0; j < m_passes[i].shaders.size(); ++j)
			{
				m_passes[i].shaders[j].shader = material.m_passes[i].shaders[j].shader;
				m_passes[i].shaders[j].onShaderUpdated.Connect(m_passes[i].shaders[j].shader->OnShaderUpdated, [this, passIndex = i](UberShader*)
				{
					InvalidatePassPipeline(passIndex);
				});
			}
		}

		m_uniformBuffers.resize(m_parent->GetUniformBlockCount());
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uniformBlockData = m_parent->GetUniformBlockData(i);

			auto& uniformBuffer = m_uniformBuffers[i];
			std::tie(uniformBuffer.renderBuffer, uniformBuffer.bufferView) = uniformBlockData.bufferPool->Allocate(uniformBuffer.poolBufferIndex);
			assert(material.m_uniformBuffers[i].values.size() == uniformBlockData.bufferPool->GetBufferSize());
			uniformBuffer.values = material.m_uniformBuffers[i].values;
		}
	}

	MaterialInstance::~MaterialInstance()
	{
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			auto& uniformBuffer = m_uniformBuffers[i];
			m_parent->GetUniformBlockData(i).bufferPool->Free(uniformBuffer.poolBufferIndex);
		}
	}

	void MaterialInstance::ApplyPreset(MaterialInstancePresetFlags presetFlags)
	{
		auto& defaultMaterials = Graphics::Instance()->GetDefaultMaterials();
		for (MaterialInstancePreset presetFlag : presetFlags)
			defaultMaterials.presetModifier[presetFlag](*this);
	}

	void MaterialInstance::DisablePass(std::string_view passName)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return DisablePass(passIndex);
	}

	void MaterialInstance::EnablePass(std::string_view passName, bool enable)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return EnablePass(passIndex, enable);
	}

	void MaterialInstance::FillShaderBinding(RenderResourceReferences& resourceReferences, std::vector<ShaderBinding::Binding>& bindings) const
	{
		// Textures
		const auto& defaultTextures = Graphics::Instance()->GetDefaultTextures();
		const auto& renderDevice = Graphics::Instance()->GetRenderDevice();
		auto& samplerCache = Graphics::Instance()->GetSamplerCache();

		// Storage buffer
		for (std::size_t i = 0; i < m_storageBufferBindings.size(); ++i)
		{
			const auto& storageSlot = m_parent->GetStorageBufferData(i);
			const auto& storageInfo = m_storageBufferBindings[i];

			bindings.push_back({
				storageSlot.bindingIndex,
				ShaderBinding::StorageBufferBinding {
					storageInfo.renderBuffer.get(), storageInfo.offset, storageInfo.size
				}
			});

			resourceReferences.renderBuffers.emplace(storageInfo.renderBuffer);
		}

		// Textures
		for (std::size_t i = 0; i < m_textureBinding.size(); ++i)
		{
			const auto& textureSlot = m_parent->GetTextureData(i);
			const auto& textureBinding = m_textureBinding[i];

			const Texture* texture = nullptr;
			if (textureBinding.texture)
			{
				if (const auto& textureObject = textureBinding.texture->GetOrCreateTexture(*renderDevice))
				{
					texture = textureObject.get();
					resourceReferences.textures.emplace(textureObject);
				}
			}

			if (!texture)
				texture = defaultTextures.whiteTextures[textureSlot.imageType]->GetOrCreateTexture(*renderDevice).get();

			const std::shared_ptr<TextureSampler>& sampler = (textureBinding.sampler) ? textureBinding.sampler : samplerCache.Get({});
			if (textureBinding.sampler)
				resourceReferences.samplers.emplace(textureBinding.sampler);

			bindings.push_back({
				textureSlot.bindingIndex,
				ShaderBinding::SampledTextureBinding {
					texture, sampler.get()
				}
			});
		}

		// UBO
		for (std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		{
			const auto& uboSlot = m_parent->GetUniformBlockData(i);
			const auto& uboInfo = m_uniformBuffers[i];

			bindings.push_back({
				uboSlot.bindingIndex,
				ShaderBinding::UniformBufferBinding {
					uboInfo.bufferView.GetBuffer(), uboInfo.bufferView.GetOffset(), uboInfo.bufferView.GetSize()
				}
			});

			resourceReferences.renderBuffers.emplace(uboInfo.renderBuffer);
		}
	}

	const std::shared_ptr<MaterialPipeline>& MaterialInstance::GetPipeline(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size() || !m_passes[passIndex].enabled)
		{
			static std::shared_ptr<MaterialPipeline> s_invalidPipeline;
			return s_invalidPipeline;
		}

		auto& pass = m_passes[passIndex];
		if (!pass.pipeline)
		{
			pass.pipelineInfo.pipelineLayout = m_parent->GetRenderPipelineLayout();

			// Options
			pass.pipelineInfo.optionValues.clear();

			const MaterialPass* passSetting = m_materialSettings.GetPass(passIndex);
			assert(passSetting);

			// Pass options
			for (const auto& [hash, value] : passSetting->options)
			{
				if (m_optionValuesOverride.find(hash) != m_optionValuesOverride.end())
					continue;

				auto& optionValue = pass.pipelineInfo.optionValues.emplace_back();
				optionValue.hash = hash;
				optionValue.value = value;
			}

			// Custom options
			for (const auto& [hash, value] : m_optionValuesOverride)
			{
				auto& optionValue = pass.pipelineInfo.optionValues.emplace_back();
				optionValue.hash = hash;
				optionValue.value = value;
			}

			// make option values consistent (required for hash/equality)
			std::sort(pass.pipelineInfo.optionValues.begin(), pass.pipelineInfo.optionValues.end(), [](const auto& lhs, const auto& rhs) { return lhs.hash < rhs.hash; });

			m_passes[passIndex].pipeline = MaterialPipeline::Get(pass.pipelineInfo);
		}

		return m_passes[passIndex].pipeline;
	}

	bool MaterialInstance::HasPass(std::string_view passName) const
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return HasPass(passIndex);
	}

	void MaterialInstance::OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder)
	{
		UploadPool& uploadPool = renderResources.GetUploadPool();
		for (UniformBuffer& uniformBuffer : m_uniformBuffers)
		{
			if (!uniformBuffer.dataInvalidated)
				continue;

			auto& allocation = uploadPool.Allocate(uniformBuffer.values.size());
			std::memcpy(allocation.mappedPtr, uniformBuffer.values.data(), uniformBuffer.values.size());

			builder.CopyBuffer(allocation, uniformBuffer.bufferView);

			uniformBuffer.dataInvalidated = false;
		}
	}

	void MaterialInstance::SetBufferProperty(std::size_t valueIndex, const MaterialSettings::BufferValue& bufferValue)
	{
		assert(valueIndex < m_bufferOverride.size());
		m_bufferOverride[valueIndex] = bufferValue;

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
		{
			if (handler->NeedsUpdateOnStorageBufferUpdate(valueIndex))
				handler->Update(*this);
		}
	}

	void MaterialInstance::SetTextureProperty(std::size_t textureIndex, std::shared_ptr<TextureAsset> texture)
	{
		assert(textureIndex < m_textureOverride.size());
		m_textureOverride[textureIndex].texture = std::move(texture);

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
		{
			if (handler->NeedsUpdateOnTextureUpdate(textureIndex))
				handler->Update(*this);
		}
	}

	void MaterialInstance::SetTextureProperty(std::size_t textureIndex, std::shared_ptr<TextureAsset> texture, const TextureSamplerInfo& samplerInfo)
	{
		assert(textureIndex < m_textureOverride.size());
		m_textureOverride[textureIndex].samplerInfo = samplerInfo;
		m_textureOverride[textureIndex].texture = std::move(texture);

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
		{
			if (handler->NeedsUpdateOnTextureUpdate(textureIndex))
				handler->Update(*this);
		}
	}

	void MaterialInstance::SetTextureSamplerProperty(std::size_t textureIndex, const TextureSamplerInfo& samplerInfo)
	{
		assert(textureIndex < m_textureOverride.size());
		m_textureOverride[textureIndex].samplerInfo = samplerInfo;

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
		{
			if (handler->NeedsUpdateOnTextureUpdate(textureIndex))
				handler->Update(*this);
		}
	}

	void MaterialInstance::SetValueProperty(std::size_t valueIndex, const MaterialSettings::Value& value)
	{
		assert(valueIndex < m_valueOverride.size());
		m_valueOverride[valueIndex] = value;

		for (const auto& handler : m_materialSettings.GetPropertyHandlers())
		{
			if (handler->NeedsUpdateOnValueUpdate(valueIndex))
				handler->Update(*this);
		}
	}

	void MaterialInstance::UpdateOptionValue(UInt32 optionHash, const nzsl::Ast::ConstantSingleValue& value)
	{
		auto it = m_optionValuesOverride.find(optionHash);
		if (it == m_optionValuesOverride.end())
			m_optionValuesOverride.emplace(optionHash, value);
		else if (it->second != value)
			it->second = value;
		else
			return;

		for (std::size_t i = 0; i < m_passes.size(); ++i)
			InvalidatePassPipeline(i);
	}

	void MaterialInstance::UpdatePassFlags(std::string_view passName, MaterialPassFlags materialFlags)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return UpdatePassFlags(passIndex, materialFlags);
	}

	void MaterialInstance::UpdatePassStates(std::string_view passName, FunctionRef<bool(RenderStates&)> stateUpdater)
	{
		std::size_t passIndex = Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex(passName);
		return UpdatePassStates(passIndex, stateUpdater);
	}

	void MaterialInstance::UpdatePassesStates(std::initializer_list<std::string_view> passesName, FunctionRef<bool(RenderStates&)> stateUpdater)
	{
		auto& materialPassRegistry = Graphics::Instance()->GetMaterialPassRegistry();
		for (std::string_view passName : passesName)
			UpdatePassStates(materialPassRegistry.GetPassIndex(passName), stateUpdater);
	}

	void MaterialInstance::UpdateStorageBufferBinding(std::size_t storageBufferBinding, std::shared_ptr<RenderBuffer> storageBuffer)
	{
		NazaraAssertMsg(storageBuffer, "invalid buffer");

		UInt64 size = storageBuffer->GetSize();
		return UpdateStorageBufferBinding(storageBufferBinding, std::move(storageBuffer), 0, size);
	}

	void MaterialInstance::UpdateStorageBufferBinding(std::size_t storageBufferBinding, std::shared_ptr<RenderBuffer> storageBuffer, UInt64 offset, UInt64 size)
	{
		NazaraAssertMsg(storageBuffer, "invalid buffer");
		NazaraAssertMsg(storageBuffer->GetType() == Nz::BufferType::Storage, "buffer is not a storage buffer");

		assert(storageBufferBinding < m_storageBufferBindings.size());
		auto& binding = m_storageBufferBindings[storageBufferBinding];
		binding.renderBuffer = std::move(storageBuffer);
		binding.offset = offset;
		binding.size = size;

		InvalidateShaderBinding();
	}

	void MaterialInstance::UpdateTextureBinding(std::size_t textureBinding, std::shared_ptr<TextureAsset> texture, std::shared_ptr<TextureSampler> textureSampler)
	{
		assert(textureBinding < m_textureBinding.size());
		auto& binding = m_textureBinding[textureBinding];
		binding.texture = std::move(texture);
		binding.sampler = std::move(textureSampler);

		InvalidateShaderBinding();
	}

	void MaterialInstance::UpdateUniformBufferData(std::size_t uniformBufferIndex, std::size_t offset, std::size_t size, const void* data)
	{
		assert(uniformBufferIndex < m_uniformBuffers.size());
		auto& uniformBlock = m_uniformBuffers[uniformBufferIndex];
		uniformBlock.dataInvalidated = true;

		assert(offset + size <= uniformBlock.values.size());
		std::memcpy(&uniformBlock.values[offset], data, size);

		OnTransferRequired(this);
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::Build(const ParameterList& materialData)
	{
		MaterialType matType = MaterialType::Basic;
		if (auto value = materialData.GetStringViewParameter(MaterialData::Type))
		{
			std::string_view valueStr = value.GetValue();
			if (valueStr == "Basic")
				matType = MaterialType::Basic;
			else if (valueStr == "Phong")
				matType = MaterialType::Phong;
			else if (valueStr == "PhysicallyBased")
				matType = MaterialType::PhysicallyBased;
			else
				NazaraWarning("unknown material type: {}", valueStr);
		}

		std::shared_ptr<MaterialInstance> matInstance = Instantiate(matType);

		auto ConvertBool = [&](const char* paramKey, std::string_view matKey)
		{
			if (auto value = materialData.GetBooleanParameter(paramKey))
				matInstance->SetValueProperty(matKey, value.GetValue());
		};

		auto ConvertColor = [&](const char* paramKey, std::string_view matKey)
		{
			if (auto value = materialData.GetColorParameter(paramKey))
				matInstance->SetValueProperty(matKey, value.GetValue());
		};

		auto ConvertFloat = [&](const char* paramKey, std::string_view matKey)
		{
			if (auto value = materialData.GetDoubleParameter(paramKey))
				matInstance->SetValueProperty(matKey, SafeCast<float>(value.GetValue()));
		};

		auto ConvertTexture = [&](const char* paramKey, std::string_view matKey, const char* paramFilterKey, const char* paramWrapKey)
		{
			if (auto value = materialData.GetStringViewParameter(paramKey))
				matInstance->SetTextureProperty(matKey, TextureAsset::OpenFromFile(Utf8Path(value.GetValue())));

			TextureSamplerInfo samplerInfo;
			if (auto value = materialData.GetIntegerParameter(paramFilterKey))
			{
				long long filterInt = value.GetValue();
				if (filterInt >= 0 || filterInt <= SafeCast<long long>(SamplerFilter::Max))
					samplerInfo.magFilter = samplerInfo.minFilter = static_cast<SamplerFilter>(filterInt);
				else
					NazaraError("invalid value for {0} sampler filter: {1}", paramFilterKey, filterInt);
			}

			if (auto value = materialData.GetIntegerParameter(paramWrapKey))
			{
				long long wrapInt = value.GetValue();
				if (wrapInt >= 0 || wrapInt <= SafeCast<long long>(SamplerWrap::Max))
					samplerInfo.wrapModeU = samplerInfo.wrapModeV = samplerInfo.wrapModeW = static_cast<SamplerWrap>(wrapInt);
				else
					NazaraError("invalid value for {0} sampler wrap: {1}", paramWrapKey, wrapInt);
			}

			matInstance->SetTextureSamplerProperty(matKey, samplerInfo);
		};

		ConvertColor(MaterialData::BaseColor, "BaseColor");
		ConvertBool(MaterialData::AlphaTest, "AlphaTest");
		ConvertFloat(MaterialData::AlphaThreshold, "AlphaTestThreshold");
		ConvertTexture(MaterialData::BaseColorTexturePath, "BaseColorMap", MaterialData::BaseColorTextureFilter, MaterialData::BaseColorTextureWrap);
		ConvertTexture(MaterialData::AlphaTexturePath, "AlphaMap", MaterialData::AlphaTextureFilter, MaterialData::AlphaTextureWrap);

		switch (matType)
		{
			case MaterialType::Basic:
				break;

			case MaterialType::Phong:
				ConvertColor(MaterialData::AmbientColor, "AmbientColor");
				ConvertColor(MaterialData::SpecularColor, "SpecularColor");
				ConvertFloat(MaterialData::Shininess, "Shininess");
				ConvertTexture(MaterialData::EmissiveTexturePath, "EmissiveMap", MaterialData::EmissiveTextureFilter, MaterialData::EmissiveTextureWrap);
				ConvertTexture(MaterialData::HeightTexturePath, "HeightMap", MaterialData::HeightTextureFilter, MaterialData::HeightTextureWrap);
				ConvertTexture(MaterialData::NormalTexturePath, "NormalMap", MaterialData::NormalTextureFilter, MaterialData::NormalTextureWrap);
				ConvertTexture(MaterialData::SpecularTexturePath, "SpecularMap", MaterialData::SpecularTextureFilter, MaterialData::SpecularTextureWrap);
				break;

			case MaterialType::PhysicallyBased:
				ConvertTexture(MaterialData::EmissiveTexturePath, "EmissiveMap", MaterialData::EmissiveTextureFilter, MaterialData::EmissiveTextureWrap);
				ConvertTexture(MaterialData::HeightTexturePath, "HeightMap", MaterialData::HeightTextureFilter, MaterialData::HeightTextureWrap);
				ConvertTexture(MaterialData::MetallicTexturePath, "MetallicMap", MaterialData::MetallicTextureFilter, MaterialData::MetallicTextureWrap);
				ConvertTexture(MaterialData::NormalTexturePath, "NormalMap", MaterialData::NormalTextureFilter, MaterialData::NormalTextureWrap);
				ConvertTexture(MaterialData::RoughnessTexturePath, "RoughnessMap", MaterialData::RoughnessTextureFilter, MaterialData::RoughnessTextureWrap);
				ConvertTexture(MaterialData::SpecularTexturePath, "SpecularMap", MaterialData::SpecularTextureFilter, MaterialData::SpecularTextureWrap);
				break;
		}

		return matInstance;
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::GetDefault(MaterialType materialType, MaterialInstancePresetFlags presetFlags)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Core module has not been initialized");

		std::size_t cacheKey = static_cast<std::size_t>(presetFlags);

		auto& defaultMaterials = graphics->GetDefaultMaterials();
		auto& materialData = defaultMaterials.materials[materialType];
		if (materialData.presetCache[cacheKey])
			return materialData.presetCache[cacheKey];

		std::shared_ptr<MaterialInstance> matInstance;
		if (presetFlags.Count() == 0)
		{
			// No flag, just get the default mat instance
			matInstance = materialData.material->GetDefaultInstance();
		}
		else
		{
			// Build a new instance
			matInstance = materialData.material->Instantiate();
			matInstance->ApplyPreset(presetFlags);
		}
		materialData.presetCache[cacheKey] = matInstance;

		return matInstance;
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::LoadFromFile(const std::filesystem::path& filePath, const MaterialInstanceParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Core module has not been initialized");

		return graphics->GetMaterialInstanceLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::LoadFromMemory(const void* data, std::size_t size, const MaterialInstanceParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Core module has not been initialized");

		return graphics->GetMaterialInstanceLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::LoadFromStream(Stream& stream, const MaterialInstanceParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssertMsg(graphics, "Core module has not been initialized");

		return graphics->GetMaterialInstanceLoader().LoadFromStream(stream, params);
	}

	std::shared_ptr<MaterialInstance> MaterialInstance::Instantiate(MaterialType materialType, MaterialInstancePresetFlags presetFlags)
	{
		return GetDefault(materialType, presetFlags)->Clone();
	}
}
