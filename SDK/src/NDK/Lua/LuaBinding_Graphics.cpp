// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Lua/LuaBinding_Graphics.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindGraphics(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Graphics>(binding);
	}

	LuaBinding_Graphics::LuaBinding_Graphics(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::AbstractViewer ***********************************/
		abstractViewer.Reset("AbstractViewer");
		{
			abstractViewer.BindMethod("GetAspectRatio", &Nz::AbstractViewer::GetAspectRatio);
			abstractViewer.BindMethod("GetEyePosition", &Nz::AbstractViewer::GetEyePosition);
			abstractViewer.BindMethod("GetForward", &Nz::AbstractViewer::GetForward);
			//abstractViewer.BindMethod("GetFrustum", &Nz::AbstractViewer::GetFrustum);
			abstractViewer.BindMethod("GetProjectionMatrix", &Nz::AbstractViewer::GetProjectionMatrix);
			//abstractViewer.BindMethod("GetTarget", &Nz::AbstractViewer::GetTarget);
			abstractViewer.BindMethod("GetViewMatrix", &Nz::AbstractViewer::GetViewMatrix);
			abstractViewer.BindMethod("GetViewport", &Nz::AbstractViewer::GetViewport);
			abstractViewer.BindMethod("GetZFar", &Nz::AbstractViewer::GetZFar);
			abstractViewer.BindMethod("GetZNear", &Nz::AbstractViewer::GetZNear);
		}

		/*********************************** Nz::InstancedRenderable ***********************************/
		instancedRenderable.Reset("InstancedRenderable");
		{
			instancedRenderable.BindMethod("GetMaterial", [] (Nz::LuaState& lua, Nz::InstancedRenderable* instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);
				switch (argCount)
				{
					case 0:
					case 1:
					{
						int argIndex = 2;
						std::size_t matIndex(lua.Check<std::size_t>(&argIndex, 0));

						return lua.Push(instance->GetMaterial(matIndex));
					}

					case 2:
					{
						int argIndex = 2;
						std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
						std::size_t matIndex(lua.Check<std::size_t>(&argIndex));

						return lua.Push(instance->GetMaterial(skinIndex, matIndex));
					}
				}

				lua.Error("No matching overload for method GetMaterial");
				return 0;
			});

			instancedRenderable.BindMethod("GetMaterialCount", &Nz::InstancedRenderable::GetMaterialCount);
			instancedRenderable.BindMethod("GetSkin", &Nz::InstancedRenderable::GetSkin);
			instancedRenderable.BindMethod("GetSkinCount", &Nz::InstancedRenderable::GetSkinCount);

			instancedRenderable.BindMethod("SetSkin", &Nz::InstancedRenderable::SetSkin);
			instancedRenderable.BindMethod("SetSkinCount", &Nz::InstancedRenderable::SetSkinCount);
		}

		/*********************************** Nz::Material ***********************************/
		material.Reset("Material");
		{
			material.SetConstructor([] (Nz::LuaState& lua, Nz::MaterialRef* instance, std::size_t argumentCount)
			{
				switch (argumentCount)
				{
					case 0:
						Nz::PlacementNew(instance, Nz::BaseMaterial::New());
						return true;

					case 1:
					{
						int argIndex = 1;
						if (lua.IsOfType(argIndex, "MaterialPipeline"))
						{
							Nz::PlacementNew(instance, Nz::BaseMaterial::New(*static_cast<Nz::MaterialPipelineRef*>(lua.ToUserdata(argIndex))));
							return true;
						}
						else if (lua.IsOfType(argIndex, "Material"))
						{
							Nz::PlacementNew(instance, Nz::BaseMaterial::New(**static_cast<Nz::MaterialRef*>(lua.ToUserdata(argIndex))));
							return true;
						}
						else
						{
							Nz::PlacementNew(instance, Nz::BaseMaterial::New(lua.Check<Nz::String>(&argIndex)));
							return true;
						}
					}
				}

				lua.Error("No matching overload for constructor");
				return false;
			});

			material.BindMethod("Configure", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "MaterialPipeline"))
				{
					instance->Configure(*static_cast<Nz::MaterialPipelineRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
				{
					lua.Push(instance->Configure(lua.Check<Nz::String>(&argIndex)));
					return 1;
				}
			});

			material.BindMethod("EnableAlphaTest", &Nz::BaseMaterial::EnableAlphaTest);
			material.BindMethod("EnableBlending", &Nz::BaseMaterial::EnableBlending);
			material.BindMethod("EnableColorWrite", &Nz::BaseMaterial::EnableColorWrite);
			material.BindMethod("EnableDepthBuffer", &Nz::BaseMaterial::EnableDepthBuffer);
			material.BindMethod("EnableDepthSorting", &Nz::BaseMaterial::EnableDepthSorting);
			material.BindMethod("EnableDepthWrite", &Nz::BaseMaterial::EnableDepthWrite);
			material.BindMethod("EnableFaceCulling", &Nz::BaseMaterial::EnableFaceCulling);
			material.BindMethod("EnableReflectionMapping", &Nz::BaseMaterial::EnableReflectionMapping);
			material.BindMethod("EnableScissorTest", &Nz::BaseMaterial::EnableScissorTest);
			material.BindMethod("EnableShadowCasting", &Nz::BaseMaterial::EnableShadowCasting);
			material.BindMethod("EnableShadowReceive", &Nz::BaseMaterial::EnableShadowReceive);
			material.BindMethod("EnableStencilTest", &Nz::BaseMaterial::EnableStencilTest);

			material.BindMethod("EnsurePipelineUpdate", &Nz::BaseMaterial::EnsurePipelineUpdate);

			material.BindMethod("GetAlphaMap", &Nz::BaseMaterial::GetAlphaMap);
			material.BindMethod("GetAlphaThreshold", &Nz::BaseMaterial::GetAlphaThreshold);
			material.BindMethod("GetAmbientColor", &Nz::BaseMaterial::GetAmbientColor);
			material.BindMethod("GetDepthFunc", &Nz::BaseMaterial::GetDepthFunc);
			material.BindMethod("GetDepthMaterial", &Nz::BaseMaterial::GetDepthMaterial);
			material.BindMethod("GetDiffuseColor", &Nz::BaseMaterial::GetDiffuseColor);
			material.BindMethod("GetDiffuseMap", &Nz::BaseMaterial::GetDiffuseMap);
			//material.BindMethod("GetDiffuseSampler",    &Nz::Material::GetDiffuseSampler);
			material.BindMethod("GetDstBlend", &Nz::BaseMaterial::GetDstBlend);
			material.BindMethod("GetEmissiveMap", &Nz::BaseMaterial::GetEmissiveMap);
			material.BindMethod("GetFaceCulling", &Nz::BaseMaterial::GetFaceCulling);
			material.BindMethod("GetFaceFilling", &Nz::BaseMaterial::GetFaceFilling);
			material.BindMethod("GetHeightMap", &Nz::BaseMaterial::GetHeightMap);
			material.BindMethod("GetLineWidth", &Nz::BaseMaterial::GetLineWidth);
			material.BindMethod("GetNormalMap", &Nz::BaseMaterial::GetNormalMap);
			//material.BindMethod("GetPipeline",            &Nz::Material::GetPipeline);
			//material.BindMethod("GetPipelineInfo",        &Nz::Material::GetPipelineInfo);
			material.BindMethod("GetPointSize", &Nz::BaseMaterial::GetPointSize);
			material.BindMethod("GetReflectionMode", &Nz::BaseMaterial::GetReflectionMode);
			//material.BindMethod("GetShader",              &Nz::Material::GetShader);
			material.BindMethod("GetShininess", &Nz::BaseMaterial::GetShininess);
			material.BindMethod("GetSpecularColor", &Nz::BaseMaterial::GetSpecularColor);
			material.BindMethod("GetSpecularMap", &Nz::BaseMaterial::GetSpecularMap);
			//material.BindMethod("GetSpecularSampler",   &Nz::Material::GetSpecularSampler);
			material.BindMethod("GetSrcBlend", &Nz::BaseMaterial::GetSrcBlend);

			material.BindMethod("HasAlphaMap", &Nz::BaseMaterial::HasAlphaMap);
			material.BindMethod("HasDepthMaterial", &Nz::BaseMaterial::HasDepthMaterial);
			material.BindMethod("HasDiffuseMap", &Nz::BaseMaterial::HasDiffuseMap);
			material.BindMethod("HasEmissiveMap", &Nz::BaseMaterial::HasEmissiveMap);
			material.BindMethod("HasHeightMap", &Nz::BaseMaterial::HasHeightMap);
			material.BindMethod("HasNormalMap", &Nz::BaseMaterial::HasNormalMap);
			material.BindMethod("HasSpecularMap", &Nz::BaseMaterial::HasSpecularMap);

			material.BindMethod("IsAlphaTestEnabled", &Nz::BaseMaterial::IsAlphaTestEnabled);
			material.BindMethod("IsBlendingEnabled", &Nz::BaseMaterial::IsBlendingEnabled);
			material.BindMethod("IsColorWriteEnabled", &Nz::BaseMaterial::IsColorWriteEnabled);
			material.BindMethod("IsDepthBufferEnabled", &Nz::BaseMaterial::IsDepthBufferEnabled);
			material.BindMethod("IsDepthSortingEnabled", &Nz::BaseMaterial::IsDepthSortingEnabled);
			material.BindMethod("IsDepthWriteEnabled", &Nz::BaseMaterial::IsDepthWriteEnabled);
			material.BindMethod("IsFaceCullingEnabled", &Nz::BaseMaterial::IsFaceCullingEnabled);
			material.BindMethod("IsReflectionMappingEnabled", &Nz::BaseMaterial::IsReflectionMappingEnabled);
			material.BindMethod("IsScissorTestEnabled", &Nz::BaseMaterial::IsScissorTestEnabled);
			material.BindMethod("IsStencilTestEnabled", &Nz::BaseMaterial::IsStencilTestEnabled);
			material.BindMethod("IsShadowCastingEnabled", &Nz::BaseMaterial::IsShadowCastingEnabled);
			material.BindMethod("IsShadowReceiveEnabled", &Nz::BaseMaterial::IsShadowReceiveEnabled);

			material.BindMethod("LoadFromFile", &Nz::BaseMaterial::LoadFromFile, Nz::MaterialParams());

			material.BindMethod("Reset", &Nz::BaseMaterial::Reset);

			material.BindMethod("SetAlphaThreshold", &Nz::BaseMaterial::SetAlphaThreshold);
			material.BindMethod("SetAmbientColor", &Nz::BaseMaterial::SetAmbientColor);
			material.BindMethod("SetDepthFunc", &Nz::BaseMaterial::SetDepthFunc);
			material.BindMethod("SetDepthFunc", &Nz::BaseMaterial::SetDepthFunc);
			material.BindMethod("SetDepthMaterial", &Nz::BaseMaterial::SetDepthMaterial);
			material.BindMethod("SetDiffuseColor", &Nz::BaseMaterial::SetDiffuseColor);
			//material.BindMethod("SetDiffuseSampler",      &Nz::Material::SetDiffuseSampler);
			material.BindMethod("SetDstBlend", &Nz::BaseMaterial::SetDstBlend);
			material.BindMethod("SetFaceCulling", &Nz::BaseMaterial::SetFaceCulling);
			material.BindMethod("SetFaceFilling", &Nz::BaseMaterial::SetFaceFilling);
			material.BindMethod("SetLineWidth", &Nz::BaseMaterial::SetLineWidth);
			material.BindMethod("SetPointSize", &Nz::BaseMaterial::SetPointSize);
			material.BindMethod("SetReflectionMode", &Nz::BaseMaterial::SetReflectionMode);
			material.BindMethod("SetShininess", &Nz::BaseMaterial::SetShininess);
			material.BindMethod("SetSpecularColor", &Nz::BaseMaterial::SetSpecularColor);
			material.BindMethod("SetSpecularColor", &Nz::BaseMaterial::SetSpecularColor);
			//material.BindMethod("SetSpecularSampler",     &Nz::Material::SetSpecularSampler);
			material.BindMethod("SetSrcBlend", &Nz::BaseMaterial::SetSrcBlend);

			material.BindStaticMethod("GetDefault", &Nz::BaseMaterial::GetDefault);

			material.BindMethod("SetAlphaMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetAlphaMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetAlphaMap(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetDiffuseMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetDiffuseMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetDiffuseMap(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetEmissiveMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetEmissiveMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetEmissiveMap(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetHeightMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetHeightMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetHeightMap(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetNormalMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetNormalMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetNormalMap(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetShader", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "UberShader"))
				{
					instance->SetShader(*static_cast<Nz::UberShaderRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetShader(lua.Check<Nz::String>(&argIndex)));
			});

			material.BindMethod("SetSpecularMap", [] (Nz::LuaState& lua, Nz::MaterialRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					instance->SetSpecularMap(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)));
					return 0;
				}
				else
					return lua.Push(instance->SetSpecularMap(lua.Check<Nz::String>(&argIndex)));
			});
		}

		/*********************************** Nz::Model ***********************************/
		model.Reset("Model");
		{
			model.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::ModelRef* modelRef) -> Nz::InstancedRenderableRef*
			{
				return reinterpret_cast<Nz::InstancedRenderableRef*>(modelRef); //TODO: Make a ObjectRefCast
			});

			model.SetConstructor([] (Nz::LuaState& /*lua*/, Nz::ModelRef* instance, std::size_t /*argumentCount*/)
			{
				Nz::PlacementNew(instance, Nz::Model::New());
				return true;
			});

			//modelClass.SetMethod("GetMesh", &Nz::Model::GetMesh);

			model.BindMethod("IsAnimated", &Nz::Model::IsAnimated);
			model.BindMethod("LoadFromFile", &Nz::Model::LoadFromFile, Nz::ModelParameters());


			model.BindMethod("SetMaterial", [] (Nz::LuaState& lua, Nz::Model* instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);
				switch (argCount)
				{
					case 2:
					{
						int argIndex = 2;
						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						{
							std::size_t matIndex(lua.Check<std::size_t>(&argIndex));
							Nz::MaterialRef mat(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(matIndex, std::move(mat));
							return 0;
						}
						else if (lua.IsOfType(argIndex, Nz::LuaType_String))
						{
							Nz::String subMesh(lua.Check<Nz::String>(&argIndex));
							Nz::MaterialRef mat(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(subMesh, std::move(mat));
							return 0;
						}

						break;
					}

					case 3:
					{
						int argIndex = 2;
						if (lua.IsOfType(argIndex, Nz::LuaType_Number))
						{
							std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
							std::size_t matIndex(lua.Check<std::size_t>(&argIndex));
							Nz::MaterialRef mat(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(skinIndex, matIndex, std::move(mat));
							return 0;
						}
						else if (lua.IsOfType(argIndex, Nz::LuaType_String))
						{
							std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
							Nz::String subMesh(lua.Check<Nz::String>(&argIndex));
							Nz::MaterialRef materialRef(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(skinIndex, subMesh, std::move(materialRef));
							return 0;
						}

						break;
					}
				}

				lua.Error("No matching overload for method SetMaterial");
				return 0;
			});

			//modelClass.SetMethod("SetMesh", &Nz::Model::SetMesh);
			//modelClass.SetMethod("SetSequence", &Nz::Model::SetSequence);
		}

		/*********************************** Nz::Sprite ***********************************/
		sprite.Reset("Sprite");
		{
			sprite.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::SpriteRef* spriteRef) -> Nz::InstancedRenderableRef*
			{
				return reinterpret_cast<Nz::InstancedRenderableRef*>(spriteRef); //TODO: Make a ObjectRefCast
			});

			sprite.SetConstructor([] (Nz::LuaState& /*lua*/, Nz::SpriteRef* instance, std::size_t /*argumentCount*/)
			{
				Nz::PlacementNew(instance, Nz::Sprite::New());
				return true;
			});

			sprite.BindMethod("GetColor", &Nz::Sprite::GetColor);
			sprite.BindMethod("GetCornerColor", &Nz::Sprite::GetCornerColor);
			sprite.BindMethod("GetOrigin", &Nz::Sprite::GetOrigin);
			sprite.BindMethod("GetSize", &Nz::Sprite::GetSize);
			sprite.BindMethod("GetTextureCoords", &Nz::Sprite::GetTextureCoords);

			sprite.BindMethod("SetColor", &Nz::Sprite::SetColor);
			sprite.BindMethod("SetCornerColor", &Nz::Sprite::SetCornerColor);
			sprite.BindMethod("SetDefaultMaterial", &Nz::Sprite::SetDefaultMaterial);
			sprite.BindMethod("SetOrigin", &Nz::Sprite::SetOrigin);
			sprite.BindMethod("SetSize", (void(Nz::Sprite::*)(const Nz::Vector2f&)) &Nz::Sprite::SetSize);
			sprite.BindMethod("SetTextureCoords", &Nz::Sprite::SetTextureCoords);
			sprite.BindMethod("SetTextureRect", &Nz::Sprite::SetTextureRect);

			sprite.BindMethod("SetMaterial", [] (Nz::LuaState& lua, Nz::SpriteRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Material"))
				{
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					instance->SetMaterial(*static_cast<Nz::MaterialRef*>(lua.ToUserdata(argIndex)), resizeSprite);
				}
				else if (lua.IsOfType(argIndex, Nz::LuaType_String))
				{
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					instance->SetMaterial(lua.ToString(argIndex), resizeSprite);
				}
				else if (lua.IsOfType(argIndex, Nz::LuaType_Number))
				{
					std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					if (lua.IsOfType(argIndex, "Material"))
						instance->SetMaterial(skinIndex, *static_cast<Nz::MaterialRef*>(lua.ToUserdata(argIndex)), resizeSprite);
					else
						instance->SetMaterial(skinIndex, lua.Check<Nz::String>(&argIndex), resizeSprite);
				}

				return 0;
			});

			sprite.BindMethod("SetTexture", [] (Nz::LuaState& lua, Nz::SpriteRef& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Texture"))
				{
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					instance->SetTexture(*static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)), resizeSprite);
				}
				else if (lua.IsOfType(argIndex, Nz::LuaType_String))
				{
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					instance->SetTexture(lua.ToString(argIndex), resizeSprite);
				}
				else if (lua.IsOfType(argIndex, Nz::LuaType_Number))
				{
					std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
					bool resizeSprite = lua.CheckBoolean(argIndex + 1, true);

					if (lua.IsOfType(argIndex, "Texture"))
						instance->SetTexture(skinIndex, *static_cast<Nz::TextureRef*>(lua.ToUserdata(argIndex)), resizeSprite);
					else
						instance->SetTexture(skinIndex, lua.Check<Nz::String>(&argIndex), resizeSprite);
				}

				return 0;
			});
		}

		/*********************************** Nz::SpriteLibrary ***********************************/
		spriteLibrary.Reset("SpriteLibrary");
		{
			spriteLibrary.BindStaticMethod("Get", &Nz::SpriteLibrary::Get);
			spriteLibrary.BindStaticMethod("Has", &Nz::SpriteLibrary::Has);
			spriteLibrary.BindStaticMethod("Register", &Nz::SpriteLibrary::Register);
			spriteLibrary.BindStaticMethod("Query", &Nz::SpriteLibrary::Query);
			spriteLibrary.BindStaticMethod("Unregister", &Nz::SpriteLibrary::Unregister);
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Graphics classes
	*/

	void LuaBinding_Graphics::Register(Nz::LuaState& state)
	{
		abstractViewer.Register(state);
		instancedRenderable.Register(state);
		material.Register(state);
		model.Register(state);
		sprite.Register(state);
		spriteLibrary.Register(state);

		// Nz::ReflectionMode
		static_assert(Nz::ReflectionMode_Max + 1 == 3, "Nz::ReflectionMode has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 3);
		{
			state.PushField("Probe",    Nz::ReflectionMode_Probe);
			state.PushField("RealTime", Nz::ReflectionMode_RealTime);
			state.PushField("Skybox",   Nz::ReflectionMode_Skybox);
		}
		state.SetGlobal("ReflectionMode");
	}
}
