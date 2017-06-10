// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

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
			instancedRenderable.BindMethod("GetMaterial", [] (Nz::LuaInstance& lua, Nz::InstancedRenderable* instance, std::size_t argumentCount) -> int
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
						Nz::PlacementNew(instance, Nz::Material::New());
						return true;

					case 1:
					{
						int argIndex = 1;
						if (lua.IsOfType(argIndex, "MaterialPipeline"))
						{
							Nz::PlacementNew(instance, Nz::Material::New(*static_cast<Nz::MaterialPipelineRef*>(lua.ToUserdata(argIndex))));
							return true;
						}
						else if (lua.IsOfType(argIndex, "Material"))
						{
							Nz::PlacementNew(instance, Nz::Material::New(**static_cast<Nz::MaterialRef*>(lua.ToUserdata(argIndex))));
							return true;
						}
						else
						{
							Nz::PlacementNew(instance, Nz::Material::New(lua.Check<Nz::String>(&argIndex)));
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

			material.BindMethod("EnableAlphaTest", &Nz::Material::EnableAlphaTest);
			material.BindMethod("EnableBlending", &Nz::Material::EnableBlending);
			material.BindMethod("EnableColorWrite", &Nz::Material::EnableColorWrite);
			material.BindMethod("EnableDepthBuffer", &Nz::Material::EnableDepthBuffer);
			material.BindMethod("EnableDepthSorting", &Nz::Material::EnableDepthSorting);
			material.BindMethod("EnableDepthWrite", &Nz::Material::EnableDepthWrite);
			material.BindMethod("EnableFaceCulling", &Nz::Material::EnableFaceCulling);
			material.BindMethod("EnableReflectionMapping", &Nz::Material::EnableReflectionMapping);
			material.BindMethod("EnableScissorTest", &Nz::Material::EnableScissorTest);
			material.BindMethod("EnableShadowCasting", &Nz::Material::EnableShadowCasting);
			material.BindMethod("EnableShadowReceive", &Nz::Material::EnableShadowReceive);
			material.BindMethod("EnableStencilTest", &Nz::Material::EnableStencilTest);

			material.BindMethod("EnsurePipelineUpdate", &Nz::Material::EnsurePipelineUpdate);

			material.BindMethod("GetAlphaMap", &Nz::Material::GetAlphaMap);
			material.BindMethod("GetAlphaThreshold", &Nz::Material::GetAlphaThreshold);
			material.BindMethod("GetAmbientColor", &Nz::Material::GetAmbientColor);
			material.BindMethod("GetDepthFunc", &Nz::Material::GetDepthFunc);
			material.BindMethod("GetDepthMaterial", &Nz::Material::GetDepthMaterial);
			material.BindMethod("GetDiffuseColor", &Nz::Material::GetDiffuseColor);
			material.BindMethod("GetDiffuseMap", &Nz::Material::GetDiffuseMap);
			//material.BindMethod("GetDiffuseSampler",    &Nz::Material::GetDiffuseSampler);
			material.BindMethod("GetDstBlend", &Nz::Material::GetDstBlend);
			material.BindMethod("GetEmissiveMap", &Nz::Material::GetEmissiveMap);
			material.BindMethod("GetFaceCulling", &Nz::Material::GetFaceCulling);
			material.BindMethod("GetFaceFilling", &Nz::Material::GetFaceFilling);
			material.BindMethod("GetHeightMap", &Nz::Material::GetHeightMap);
			material.BindMethod("GetLineWidth", &Nz::Material::GetLineWidth);
			material.BindMethod("GetNormalMap", &Nz::Material::GetNormalMap);
			//material.BindMethod("GetPipeline",            &Nz::Material::GetPipeline);
			//material.BindMethod("GetPipelineInfo",        &Nz::Material::GetPipelineInfo);
			material.BindMethod("GetPointSize", &Nz::Material::GetPointSize);
			//material.BindMethod("GetShader",              &Nz::Material::GetShader);
			material.BindMethod("GetShininess", &Nz::Material::GetShininess);
			material.BindMethod("GetSpecularColor", &Nz::Material::GetSpecularColor);
			material.BindMethod("GetSpecularMap", &Nz::Material::GetSpecularMap);
			//material.BindMethod("GetSpecularSampler",   &Nz::Material::GetSpecularSampler);
			material.BindMethod("GetSrcBlend", &Nz::Material::GetSrcBlend);

			material.BindMethod("HasAlphaMap", &Nz::Material::HasAlphaMap);
			material.BindMethod("HasDepthMaterial", &Nz::Material::HasDepthMaterial);
			material.BindMethod("HasDiffuseMap", &Nz::Material::HasDiffuseMap);
			material.BindMethod("HasEmissiveMap", &Nz::Material::HasEmissiveMap);
			material.BindMethod("HasHeightMap", &Nz::Material::HasHeightMap);
			material.BindMethod("HasNormalMap", &Nz::Material::HasNormalMap);
			material.BindMethod("HasSpecularMap", &Nz::Material::HasSpecularMap);

			material.BindMethod("IsAlphaTestEnabled", &Nz::Material::IsAlphaTestEnabled);
			material.BindMethod("IsBlendingEnabled", &Nz::Material::IsBlendingEnabled);
			material.BindMethod("IsColorWriteEnabled", &Nz::Material::IsColorWriteEnabled);
			material.BindMethod("IsDepthBufferEnabled", &Nz::Material::IsDepthBufferEnabled);
			material.BindMethod("IsDepthSortingEnabled", &Nz::Material::IsDepthSortingEnabled);
			material.BindMethod("IsDepthWriteEnabled", &Nz::Material::IsDepthWriteEnabled);
			material.BindMethod("IsFaceCullingEnabled", &Nz::Material::IsFaceCullingEnabled);
			material.BindMethod("IsReflectionMappingEnabled", &Nz::Material::IsReflectionMappingEnabled);
			material.BindMethod("IsScissorTestEnabled", &Nz::Material::IsScissorTestEnabled);
			material.BindMethod("IsStencilTestEnabled", &Nz::Material::IsStencilTestEnabled);
			material.BindMethod("IsShadowCastingEnabled", &Nz::Material::IsShadowCastingEnabled);
			material.BindMethod("IsShadowReceiveEnabled", &Nz::Material::IsShadowReceiveEnabled);

			material.BindMethod("LoadFromFile", &Nz::Material::LoadFromFile, Nz::MaterialParams());

			material.BindMethod("Reset", &Nz::Material::Reset);

			material.BindMethod("SetAlphaThreshold", &Nz::Material::SetAlphaThreshold);
			material.BindMethod("SetAmbientColor", &Nz::Material::SetAmbientColor);
			material.BindMethod("SetDepthFunc", &Nz::Material::SetDepthFunc);
			material.BindMethod("SetDepthFunc", &Nz::Material::SetDepthFunc);
			material.BindMethod("SetDepthMaterial", &Nz::Material::SetDepthMaterial);
			material.BindMethod("SetDiffuseColor", &Nz::Material::SetDiffuseColor);
			//material.BindMethod("SetDiffuseSampler",      &Nz::Material::SetDiffuseSampler);
			material.BindMethod("SetDstBlend", &Nz::Material::SetDstBlend);
			material.BindMethod("SetFaceCulling", &Nz::Material::SetFaceCulling);
			material.BindMethod("SetFaceFilling", &Nz::Material::SetFaceFilling);
			material.BindMethod("SetLineWidth", &Nz::Material::SetLineWidth);
			material.BindMethod("SetPointSize", &Nz::Material::SetPointSize);
			material.BindMethod("SetShininess", &Nz::Material::SetShininess);
			material.BindMethod("SetSpecularColor", &Nz::Material::SetSpecularColor);
			material.BindMethod("SetSpecularColor", &Nz::Material::SetSpecularColor);
			//material.BindMethod("SetSpecularSampler",     &Nz::Material::SetSpecularSampler);
			material.BindMethod("SetSrcBlend", &Nz::Material::SetSrcBlend);

			material.BindStaticMethod("GetDefault", &Nz::Material::GetDefault);

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


			model.BindMethod("SetMaterial", [] (Nz::LuaInstance& lua, Nz::Model* instance, std::size_t argumentCount) -> int
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
							Nz::MaterialRef material(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(matIndex, std::move(material));
							return 0;
						}
						else if (lua.IsOfType(argIndex, Nz::LuaType_String))
						{
							Nz::String subMesh(lua.Check<Nz::String>(&argIndex));
							Nz::MaterialRef material(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(subMesh, std::move(material));
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
							Nz::MaterialRef material(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(skinIndex, matIndex, std::move(material));
							return 0;
						}
						else if (lua.IsOfType(argIndex, Nz::LuaType_String))
						{
							std::size_t skinIndex(lua.Check<std::size_t>(&argIndex));
							Nz::String subMesh(lua.Check<Nz::String>(&argIndex));
							Nz::MaterialRef material(lua.Check<Nz::MaterialRef>(&argIndex));

							instance->SetMaterial(skinIndex, subMesh, std::move(material));
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
	}
}
