#include "SpacebattleDemo.hpp"
#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Components.hpp>
#include <NDK/Systems.hpp>

namespace
{
	const float maxLaserLife = 15.f;
	const float maxSmokeLife = 20.f;
}

struct SpaceshipComponent : public Ndk::Component<SpaceshipComponent>
{
	SpaceshipComponent()
	{
		engineSound.SetBuffer(Nz::SoundBufferManager::Get("resources/spaceship_loop.wav"));
		engineSound.EnableSpatialization(true);
		engineSound.SetMinDistance(10.f);
		engineSound.SetPitch(1.5f);

		hitSound.SetBuffer(Nz::SoundBufferManager::Get("resources/explosion.wav"));
		hitSound.EnableSpatialization(true);
		hitSound.SetMinDistance(150.f);

		laserSound.SetBuffer(Nz::SoundBufferManager::Get("resources/laser.wav"));
		laserSound.EnableSpatialization(true);
		laserSound.SetMinDistance(150.f);
		laserSound.SetVolume(60.f);
	}

	std::array<Nz::SpriteRef, 2> laserBeamSprites;
	Nz::Sound engineSound;
	Nz::Sound hitSound;
	Nz::Sound laserSound;
	Nz::UInt64 hitTime = 0;
	Nz::Vector3f targetPos = Nz::Vector3f::Zero();
	bool attacking = true;

	static Ndk::ComponentIndex componentIndex;
};
Ndk::ComponentIndex SpaceshipComponent::componentIndex;

struct LaserBeamComponent : public Ndk::Component<LaserBeamComponent>
{
	LaserBeamComponent()
	{
		Nz::MaterialRef laserBeamMaterial = Nz::MaterialLibrary::Get("LaserBeam");
		for (Nz::Sprite& sprite : sprites)
		{
			sprite.SetMaterial(laserBeamMaterial);
			sprite.SetOrigin(Nz::Vector2f(0.f, 0.5f));
			sprite.SetTextureCoords(Nz::Rectf(0.f, 0.f, 50.f, 1.f));
		}
	}

	void OnAttached() override
	{
		auto& spaceshipCom = m_entity->GetComponent<SpaceshipComponent>();
		spaceshipCom.laserSound.Play();
	}

	std::array<Nz::Sprite, 2> sprites;
	Nz::Vector3f origin = Nz::Vector3f::Zero();
	float length = 1500.f;
	float life = 2.f;
	float width = 2.f;

	static Ndk::ComponentIndex componentIndex;
};
Ndk::ComponentIndex LaserBeamComponent::componentIndex;

class LaserBeamSystem : public Ndk::System<LaserBeamSystem>
{
	public:
		LaserBeamSystem(const ExampleShared& sharedData) :
		m_sharedData(sharedData)
		{
			Requires<Ndk::GraphicsComponent, LaserBeamComponent>();
		}

		void OnEntityAdded(Ndk::Entity* entity) override
		{
			auto& laserComponent = entity->GetComponent<LaserBeamComponent>();
			auto& gfxComponent = entity->GetComponent<Ndk::GraphicsComponent>();

			for (Nz::Sprite& sprite : laserComponent.sprites)
				sprite.SetSize({laserComponent.length, laserComponent.width});

			gfxComponent.Attach(&laserComponent.sprites[0], Nz::Matrix4f::Transform(laserComponent.origin, Nz::EulerAnglesf(0.f, 90.f, 0.f)));
			gfxComponent.Attach(&laserComponent.sprites[1], Nz::Matrix4f::Transform(laserComponent.origin, Nz::EulerAnglesf(90.f, 90.f, 0.f)));
		}

		void OnUpdate(float elapsedTime) override
		{
			const float scrollSpeed = 2.f;
			for (const Ndk::EntityHandle& entity : GetEntities())
			{
				auto& laserComponent = entity->GetComponent<LaserBeamComponent>();
				for (Nz::Sprite& sprite : laserComponent.sprites)
				{
					Nz::Rectf rect = sprite.GetTextureCoords();
					rect.x = std::fmod(rect.x - elapsedTime * scrollSpeed, rect.width);

					sprite.SetTextureCoords(rect);
				}
			}
		}

		static Ndk::SystemIndex systemIndex;

	private:
		const ExampleShared& m_sharedData;
};
Ndk::SystemIndex LaserBeamSystem::systemIndex;

class SpaceshipSystem : public Ndk::System<SpaceshipSystem>
{
	public:
		SpaceshipSystem(const ExampleShared& sharedData) :
		m_sharedData(sharedData)
		{
			Requires<Ndk::NodeComponent, Ndk::VelocityComponent, SpaceshipComponent>();
		}

		void OnEntityAdded(Ndk::Entity* entity) override
		{
			std::uniform_real_distribution<float> pitchDis(0.8f, 1.5f);

			auto& nodeComponent = entity->GetComponent<Ndk::NodeComponent>();
			auto& spaceshipComponent = entity->GetComponent<SpaceshipComponent>();

			spaceshipComponent.engineSound.SetPosition(nodeComponent.GetPosition());
			spaceshipComponent.engineSound.Play();
			spaceshipComponent.engineSound.EnableLooping(true);

			spaceshipComponent.laserSound.SetPitch(pitchDis(m_sharedData.randomGen));
		}

		void OnUpdate(float elapsedTime) override
		{
			const float escapeMaxDist = 50.f;
			const float speed = 200.f;

			Nz::UInt64 curTime = Nz::GetElapsedMilliseconds();
			std::uniform_real_distribution<float> dis(-escapeMaxDist, escapeMaxDist);

			for (const Ndk::EntityHandle& entity : GetEntities())
			{
				auto& nodeComponent = entity->GetComponent<Ndk::NodeComponent>();
				auto& spaceshipComponent = entity->GetComponent<SpaceshipComponent>();
				auto& velocityComponent = entity->GetComponent<Ndk::VelocityComponent>();

				//< I agree, I need some kind of SoundEmitterComponent
				spaceshipComponent.engineSound.SetPosition(nodeComponent.GetPosition());
				spaceshipComponent.engineSound.SetVelocity(velocityComponent.linearVelocity);

				spaceshipComponent.hitSound.SetPosition(nodeComponent.GetPosition());
				spaceshipComponent.hitSound.SetVelocity(velocityComponent.linearVelocity);

				spaceshipComponent.laserSound.SetPosition(nodeComponent.GetPosition());
				spaceshipComponent.laserSound.SetVelocity(velocityComponent.linearVelocity);

				Nz::Vector3f targetDir = spaceshipComponent.targetPos - nodeComponent.GetPosition();
				targetDir.Normalize();

				Nz::Quaternionf targetRotation = Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), targetDir);

				nodeComponent.SetRotation(Nz::Quaternionf::Slerp(nodeComponent.GetRotation(), targetRotation, elapsedTime * 1.5f));

				Nz::Vector3f actualDir = nodeComponent.GetForward();
				float sqDistance = spaceshipComponent.targetPos.SquaredDistance(nodeComponent.GetPosition());

				if (spaceshipComponent.attacking)
				{
					float dotProduct = targetDir.DotProduct(actualDir);
					if (dotProduct > 0.9f && sqDistance < (150.f * 150.f) && !entity->HasComponent<LaserBeamComponent>())
					{
						auto& laserBeam = entity->AddComponent<LaserBeamComponent>();
						laserBeam.origin = Nz::Vector3f::Forward() * 12.f + Nz::Vector3f::Down() * 2.f;
					}

					if (sqDistance < (100.f * 100.f))
					{
						entity->RemoveComponent<LaserBeamComponent>();

						spaceshipComponent.targetPos -= Nz::Vector3f(dis(m_sharedData.randomGen), dis(m_sharedData.randomGen), dis(m_sharedData.randomGen)) * -actualDir * escapeMaxDist / 2.f;
						spaceshipComponent.attacking = false;
					}
				}
				else if (sqDistance < (50.f * 50.f) && spaceshipComponent.hitTime == 0)
				{
					spaceshipComponent.targetPos = Nz::Vector3f::Zero();
					spaceshipComponent.attacking = true;
				}

				if (spaceshipComponent.hitTime == 0 || curTime - spaceshipComponent.hitTime <= 1000)
					velocityComponent.linearVelocity = actualDir * speed;
				else if (curTime - spaceshipComponent.hitTime > 10000)
					entity->Kill();
			}
		}

		static Ndk::SystemIndex systemIndex;

	private:
		const ExampleShared& m_sharedData;
};
Ndk::SystemIndex SpaceshipSystem::systemIndex;

struct TorpedoParticle
{
	Nz::Color color;
	Nz::Vector2f size;
	Nz::Vector3f position;
	Nz::Vector3f velocity;
	float rotation;
	float life;
};

SpacebattleExample::SpacebattleExample(ExampleShared& sharedData) :
ParticleDemo("Space battle", sharedData)
{
	Ndk::InitializeComponent<LaserBeamComponent>("Lasrbeam");
	Ndk::InitializeComponent<SpaceshipComponent>("Spceship");
	Ndk::InitializeSystem<LaserBeamSystem>();
	Ndk::InitializeSystem<SpaceshipSystem>();

	Nz::ModelParameters parameters;
	parameters.mesh.optimizeIndexBuffers = false;

	Nz::Color grey(100, 100, 100);

	if (!m_turret.baseModel.LoadFromFile("resources/Turret/base.obj", parameters))
		NazaraWarning("Failed to load base.obj");

	for (unsigned int i = 0; i < m_turret.baseModel.GetMaterialCount(); ++i)
		m_turret.baseModel.GetMaterial(i)->SetDiffuseColor(grey);

	if (!m_turret.rotatingBaseModel.LoadFromFile("resources/Turret/rotating_base.obj", parameters))
		NazaraWarning("Failed to load rotating_base.obj");

	for (unsigned int i = 0; i < m_turret.rotatingBaseModel.GetMaterialCount(); ++i)
		m_turret.rotatingBaseModel.GetMaterial(i)->SetDiffuseColor(grey);

	if (!m_turret.cannonBaseModel.LoadFromFile("resources/Turret/cannon_base.obj", parameters))
		NazaraWarning("Failed to load cannon_base.obj");

	for (unsigned int i = 0; i < m_turret.cannonBaseModel.GetMaterialCount(); ++i)
		m_turret.cannonBaseModel.GetMaterial(i)->SetDiffuseColor(grey);

	parameters.mesh.texCoordScale.Set(40.f, 40.f);
	parameters.mesh.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, 180.f, 0.f));
	if (!m_turret.cannonModel.LoadFromFile("resources/Turret/cannon.obj", parameters))
		NazaraWarning("Failed to load cannon.obj");

	// Since OBJ don't support normal maps..
	m_turret.cannonModel.GetMaterial(0)->SetNormalMap("resources/Turret/198_norm.jpg");

	parameters.mesh.matrix.MakeIdentity();
	parameters.mesh.texCoordScale.Set(1.f, 1.f);

	parameters.mesh.center = true;
	if (!m_spacestationModel.LoadFromFile("resources/SpaceStation/space_station.obj", parameters))
		NazaraWarning("Failed to load space_station.obj");

	parameters.mesh.texCoordScale.Set(1.f, -1.f);
	parameters.mesh.matrix.MakeRotation(Nz::EulerAnglesf(0.f, -90.f, 0.f));

	if (!m_spaceshipModel.LoadFromFile("resources/space_frigate_6/space_frigate_6.obj", parameters))
		NazaraWarning("Failed to load space_frigate_6.obj");

	// Since OBJ don't support normal maps..
	for (unsigned int i = 0; i < m_spaceshipModel.GetMaterialCount(); ++i)
	{
		m_spaceshipModel.GetMaterial(i)->SetEmissiveMap("resources/space_frigate_6/space_frigate_6_illumination.jpg");
		m_spaceshipModel.GetMaterial(i)->SetNormalMap("resources/space_frigate_6/space_frigate_6_normal.png");
	}

	Nz::TextureRef skyboxCubemap = Nz::Texture::New();
	if (skyboxCubemap->Create(Nz::ImageType_Cubemap, Nz::PixelFormatType_RGBA8, 2048, 2048))
	{
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_PositiveX, "resources/purple_nebula_skybox/purple_nebula_skybox_right1.png");
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_PositiveY, "resources/purple_nebula_skybox/purple_nebula_skybox_top3.png");
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_PositiveZ, "resources/purple_nebula_skybox/purple_nebula_skybox_front5.png");
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_NegativeX, "resources/purple_nebula_skybox/purple_nebula_skybox_left2.png");
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_NegativeY, "resources/purple_nebula_skybox/purple_nebula_skybox_bottom4.png");
		skyboxCubemap->LoadFaceFromFile(Nz::CubemapFace_NegativeZ, "resources/purple_nebula_skybox/purple_nebula_skybox_back6.png");

		m_skybox.SetTexture(std::move(skyboxCubemap));
	}

	m_torpedoDeclaration = Nz::ParticleDeclaration::New();
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Color,     Nz::ComponentType_Color,  NazaraOffsetOf(TorpedoParticle, color));
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Position,  Nz::ComponentType_Float3, NazaraOffsetOf(TorpedoParticle, position));
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Rotation,  Nz::ComponentType_Float1, NazaraOffsetOf(TorpedoParticle, rotation));
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Size,      Nz::ComponentType_Float2, NazaraOffsetOf(TorpedoParticle, size));
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Life,      Nz::ComponentType_Float1, NazaraOffsetOf(TorpedoParticle, life));
	m_torpedoDeclaration->EnableComponent(Nz::ParticleComponent_Velocity,  Nz::ComponentType_Float3, NazaraOffsetOf(TorpedoParticle, velocity));

	Nz::TextureSampler diffuseSampler;
	diffuseSampler.SetWrapMode(Nz::SamplerWrap_Repeat);

	Nz::MaterialRef material = Nz::Material::New("Translucent3D");
	material->SetDiffuseMap("resources/LaserBeam.png");
	material->SetDiffuseSampler(diffuseSampler);

	Nz::MaterialLibrary::Register("LaserBeam", std::move(material));

	Nz::MaterialRef sparkleMat1 = Nz::Material::New("Translucent3D");

	sparkleMat1->SetDiffuseMap("resources/flare1.png");
	Nz::MaterialLibrary::Register("TorpedoFlare1", std::move(sparkleMat1));

	m_spaceshipTemplate = m_shared.world3D->CreateEntity();
	m_spaceshipTemplate->Enable(false);

	m_spaceshipTemplate->AddComponent<Ndk::NodeComponent>();
	m_spaceshipTemplate->AddComponent<Ndk::VelocityComponent>();
	m_spaceshipTemplate->AddComponent<SpaceshipComponent>();
	auto& gfxComponent = m_spaceshipTemplate->AddComponent<Ndk::GraphicsComponent>();
	gfxComponent.Attach(&m_spaceshipModel);

	m_ambientMusic.OpenFromFile("resources/ambience.ogg");
	m_ambientMusic.SetVolume(60.f);
}

void SpacebattleExample::Enter(Ndk::StateMachine& fsm)
{
	ParticleDemo::Enter(fsm);

	m_shared.world3D->AddSystem<LaserBeamSystem>(m_shared);
	m_shared.world3D->AddSystem<SpaceshipSystem>(m_shared);

	Ndk::RenderSystem& renderSystem2D = m_shared.world2D->GetSystem<Ndk::RenderSystem>();
	Ndk::RenderSystem& renderSystem3D = m_shared.world3D->GetSystem<Ndk::RenderSystem>();
	renderSystem2D.SetDefaultBackground(nullptr);
	renderSystem3D.SetDefaultBackground(&m_skybox);

	CreateSpaceShip();
	CreateTurret();

	Ndk::EntityHandle light = m_shared.world3D->CreateEntity();
	Ndk::NodeComponent& lightNode = light->AddComponent<Ndk::NodeComponent>();
	Ndk::LightComponent& lightComp = light->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);
	lightNode.SetRotation(Nz::EulerAnglesf(-30.f, 0.f, 0.f));
	RegisterEntity(light);

	Ndk::NodeComponent& cameraNode = m_shared.viewer3D->GetComponent<Ndk::NodeComponent>();
	cameraNode.SetParent(m_turret.cannonAnchorEntity);
	cameraNode.SetPosition(Nz::Vector3f::Up() * 4.f - Nz::Vector3f::Backward() * 6.f);
	cameraNode.SetRotation(Nz::EulerAnglesf(0.f, 180.f, 0.f));

	m_introTimer = 10.f;
	m_spaceshipSpawnCounter = -5.f;
	m_turretBaseRotation = 0.f;
	m_turretCannonBaseRotation = 0.f;
	m_turretShootTimer = 0.f;

	Ndk::EntityHandle torpedoGroupEntity = m_shared.world3D->CreateEntity();
	m_torpedoGroup = torpedoGroupEntity->AddComponent<Ndk::ParticleGroupComponent>(200, m_torpedoDeclaration).CreateHandle();
	RegisterParticleGroup(torpedoGroupEntity);

	m_torpedoGroup->AddController(Nz::ParticleFunctionController::New([] (Nz::ParticleGroup& group, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		auto positionPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto lifePtr     = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);
		auto rotationPtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Rotation);
		auto velocityPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Velocity);

		for (unsigned int i = startId; i <= endId; ++i)
		{
			rotationPtr[i] += elapsedTime * 90.f;
			positionPtr[i] += velocityPtr[i] * elapsedTime;

			lifePtr[i] -= elapsedTime;
			if (lifePtr[i] < 0.f)
				group.KillParticle(i);
		}
	}));


	m_torpedoGroup->AddController(Nz::ParticleFunctionController::New([this] (Nz::ParticleGroup& group, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		auto positionPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto sizePtr     = mapper.GetComponentPtr<Nz::Vector2f>(Nz::ParticleComponent_Size);

		auto& spaceshipSystem = m_shared.world3D->GetSystem<SpaceshipSystem>();

		for (unsigned int i = startId; i <= endId; ++i)
		{
			Nz::Spheref torpedoSphere(positionPtr[i], std::max(sizePtr[i].x, sizePtr[i].y) * 0.1f);

			for (const Ndk::EntityHandle& entity : spaceshipSystem.GetEntities())
			{
				auto& spaceshipNode = entity->GetComponent<Ndk::NodeComponent>();

				Nz::Spheref spaceshipSphere(spaceshipNode.GetPosition(), 10.f);
				if (torpedoSphere.Intersect(spaceshipSphere))
				{
					entity->RemoveComponent<LaserBeamComponent>();

					group.KillParticle(i);

					const float hitMaxDist = 500.f;

					std::uniform_real_distribution<float> dis(-hitMaxDist, hitMaxDist);

					auto& spaceshipComponent = entity->GetComponent<SpaceshipComponent>();
					spaceshipComponent.attacking = false;
					spaceshipComponent.engineSound.Stop();
					spaceshipComponent.hitSound.Play();
					spaceshipComponent.hitTime = Nz::GetElapsedMilliseconds();
					spaceshipComponent.targetPos = Nz::Vector3f(dis(m_shared.randomGen), dis(m_shared.randomGen), dis(m_shared.randomGen));

					auto& emitter = entity->AddComponent<Ndk::ParticleEmitterComponent>();
					emitter.SetEmissionCount(2);
					emitter.SetEmissionRate(200.f);

					emitter.SetSetupFunc([this] (const Ndk::EntityHandle& emitterEntity, Nz::ParticleMapper& particleMapper, unsigned int count)
					{
						auto& gen = m_shared.randomGen;

						const float maxFireVel = 15.f;
						std::uniform_real_distribution<float> lifeDis(-0.5f, 0.5f);
						std::uniform_real_distribution<float> normalDis(-1.f, 1.f);
						std::uniform_real_distribution<float> posDis(-0.1f, 0.1f);
						std::uniform_real_distribution<float> rotDis(-180.f, 180.f);
						std::uniform_real_distribution<float> sizeDis(1.0f, 4.f);
						std::uniform_real_distribution<float> velDis(-maxFireVel, maxFireVel);

						Nz::Vector3f pos = emitterEntity->GetComponent<Ndk::NodeComponent>().GetPosition();

						Nz::ParticleStruct_Billboard* billboards = static_cast<Nz::ParticleStruct_Billboard*>(particleMapper.GetPointer());
						Nz::ParticleStruct_Billboard* smokeParticles = static_cast<Nz::ParticleStruct_Billboard*>(m_smokeGroup->CreateParticles(count));
						for (unsigned int j = 0; j < count; ++j)
						{
							billboards[j].color = Nz::Color::White;
							billboards[j].life = 1.f + lifeDis(gen);
							billboards[j].position = pos + Nz::Vector3f(posDis(gen), posDis(gen), posDis(gen));
							billboards[j].rotation = rotDis(gen);
							billboards[j].size = {1.28f, 1.28f};
							billboards[j].size *= sizeDis(gen);
							billboards[j].velocity.Set(normalDis(gen), normalDis(gen), normalDis(gen));
							billboards[j].velocity.Normalize();
							billboards[j].velocity *= velDis(gen);

							smokeParticles[j].color = Nz::Color(128, 128, 128, 0);
							smokeParticles[j].life = maxSmokeLife;
							smokeParticles[j].position = billboards[j].position;
							smokeParticles[j].rotation = billboards[j].rotation;
							smokeParticles[j].size = {2.56f, 2.56f};
							smokeParticles[j].size *= sizeDis(gen);
							smokeParticles[j].velocity = billboards[j].velocity / 2.f;
						}
					});
					m_fireGroup->AddEmitter(entity);

					break;
				}
			}
		}
	}));

	m_torpedoGroup->SetRenderer(Nz::ParticleFunctionRenderer::New([sparkleMat1 = Nz::MaterialLibrary::Get("TorpedoFlare1")] (const Nz::ParticleGroup& /*group*/, const Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, Nz::AbstractRenderQueue* renderQueue)
	{
		auto positionPtr = mapper.GetComponentPtr<const Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto rotationPtr = mapper.GetComponentPtr<const float>(Nz::ParticleComponent_Rotation);
		auto sizePtr     = mapper.GetComponentPtr<const Nz::Vector2f>(Nz::ParticleComponent_Size);

		renderQueue->AddBillboards(0, sparkleMat1, endId - startId + 1, Nz::Recti(-1, -1), positionPtr, sizePtr, rotationPtr);
		for (unsigned int i = startId; i <= endId; ++i)
		{
			Nz::AbstractRenderQueue::PointLight pointLight;
			pointLight.ambientFactor = 0.f;
			pointLight.attenuation = 0.9f;
			pointLight.color = Nz::Color::Cyan;
			pointLight.diffuseFactor = 1.f;
			pointLight.position = positionPtr[i];
			pointLight.radius = std::max(sizePtr[i].x, sizePtr[i].y) * 2.f;
			pointLight.invRadius = 1.f / pointLight.radius;
			pointLight.shadowMap = nullptr;

			renderQueue->AddPointLight(pointLight);
		}
	}));


	//////////////////////////////////////////////////////////////////////////

	Ndk::EntityHandle fireGroupEntity = m_shared.world3D->CreateEntity();
	m_fireGroup = fireGroupEntity->AddComponent<Ndk::ParticleGroupComponent>(40000, Nz::ParticleDeclaration::Get(Nz::ParticleLayout_Billboard)).CreateHandle();
	RegisterParticleGroup(fireGroupEntity);

	Ndk::EntityHandle smokeGroupEntity = m_shared.world3D->CreateEntity();
	m_smokeGroup = smokeGroupEntity->AddComponent<Ndk::ParticleGroupComponent>(40000, Nz::ParticleDeclaration::Get(Nz::ParticleLayout_Billboard)).CreateHandle();
	RegisterParticleGroup(smokeGroupEntity);

	auto movementController = Nz::ParticleFunctionController::New([this] (Nz::ParticleGroup& group, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		auto lifePtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);
		auto posPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto sizePtr = mapper.GetComponentPtr<Nz::Vector2f>(Nz::ParticleComponent_Size);
		auto velPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Velocity);

		auto& spaceshipSystem = m_shared.world3D->GetSystem<SpaceshipSystem>();

		const Nz::Vector2f sizeGrowth(0.5f);

		float velFactor = std::pow(0.9f, elapsedTime * 15.f);
		for (unsigned int i = startId; i <= endId; ++i)
		{
			float& remainingLife = lifePtr[i];

			remainingLife -= elapsedTime;
			if (remainingLife <= 0.f)
			{
				group.KillParticle(i);
				continue;
			}

			Nz::Vector3f& position = posPtr[i];
			Nz::Vector2f& size = sizePtr[i];
			Nz::Vector3f& velocity = velPtr[i];

			position += velPtr[i] * elapsedTime;
			size     += sizeGrowth * elapsedTime;
			velocity *= (velocity.GetSquaredLength() >= 1.f) ? velFactor : 1.f;

			if (remainingLife <= 18.f)
			{
				for (const Ndk::EntityHandle& entity : spaceshipSystem.GetEntities())
				{
					auto& spaceshipNode = entity->GetComponent<Ndk::NodeComponent>();

					Nz::Spheref spaceshipSphere(spaceshipNode.GetPosition(), 5.f);
					if (spaceshipSphere.Contains(position))
					{
						auto& spaceshipVel = entity->GetComponent<Ndk::VelocityComponent>();

						Nz::Vector3f force = spaceshipVel.linearVelocity * 2.f + (position - spaceshipSphere.GetPosition()) * 10.f;
						velocity += force * elapsedTime;
					}
				}

				TorpedoParticle* torpedos = static_cast<TorpedoParticle*>(m_torpedoGroup->GetBuffer());
				std::size_t torpedoCount = m_torpedoGroup->GetParticleCount();
				for (std::size_t j = 0; j < torpedoCount; ++j)
				{
					Nz::Spheref tordedoSphere(torpedos[j].position, 5.f);

					if (tordedoSphere.Contains(position))
					{
						Nz::Spheref tordedoCenter(torpedos[j].position, 2.f);
						if (tordedoCenter.Contains(position))
						{
							group.KillParticle(i);
							break;
						}

						Nz::Vector3f dir = (torpedos[j].position - position);
						float length;
						dir.Normalize(&length);

						remainingLife -= 100.f * elapsedTime / length;
						size -= 100.f * sizeGrowth * elapsedTime / length;
						velocity += 500.f * dir * elapsedTime / length;
						velocity += torpedos[j].velocity * elapsedTime;

						break; //< There's no way a particle would be in multiple torpedo at once
					}
				}
			}
		}
	});

	m_fireGroup->AddController(movementController);
	m_fireGroup->AddController(Nz::ParticleFunctionController::New([] (Nz::ParticleGroup& /*group*/, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		auto colorPtr = mapper.GetComponentPtr<Nz::Color>(Nz::ParticleComponent_Color);
		auto lifePtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);

		float velFactor = std::pow(0.9f, elapsedTime / 0.1f);
		for (unsigned int i = startId; i <= endId; ++i)
			colorPtr[i].a = static_cast<Nz::UInt8>(Nz::Clamp(lifePtr[i] * 255.f, 0.f, 255.f));
	}));

	m_smokeGroup->AddController(movementController);
	m_smokeGroup->AddController(Nz::ParticleFunctionController::New([] (Nz::ParticleGroup& /*group*/, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime)
	{
		auto colorPtr = mapper.GetComponentPtr<Nz::Color>(Nz::ParticleComponent_Color);
		auto lifePtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);

		for (unsigned int i = startId; i <= endId; ++i)
		{
			float alpha = std::min((maxSmokeLife - lifePtr[i]) * 255.f / 5.f, 255.f);
			alpha -= std::max((maxSmokeLife - lifePtr[i]) / maxSmokeLife * 255.f, 0.f);

			colorPtr[i].a = static_cast<Nz::UInt8>(Nz::Clamp(alpha, 0.f, 255.f));
		}
	}));

	Nz::MaterialRef fireMat = Nz::Material::New("Translucent3D");
	fireMat->EnableFaceCulling(true);
	fireMat->SetDiffuseMap("resources/fire_particle.png");
	// Additive blending for fire
	fireMat->SetDstBlend(Nz::BlendFunc_One);
	fireMat->SetSrcBlend(Nz::BlendFunc_SrcAlpha);

	Nz::MaterialRef smokeMat = Nz::Material::New("Translucent3D");
	smokeMat->EnableFaceCulling(true);
	smokeMat->SetDiffuseColor(Nz::Color(128, 128, 128));
	smokeMat->SetDiffuseMap("resources/smoke.png");

	m_fireGroup->SetRenderer(Nz::ParticleFunctionRenderer::New([fireMat] (const Nz::ParticleGroup& /*group*/, const Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, Nz::AbstractRenderQueue* renderQueue)
	{
		auto colorPtr = mapper.GetComponentPtr<const Nz::Color>(Nz::ParticleComponent_Color);
		auto posPtr = mapper.GetComponentPtr<const Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto rotPtr = mapper.GetComponentPtr<const float>(Nz::ParticleComponent_Rotation);
		auto sizePtr = mapper.GetComponentPtr<const Nz::Vector2f>(Nz::ParticleComponent_Size);

		renderQueue->AddBillboards(0, fireMat, endId - startId + 1, Nz::Recti(-1, -1), posPtr, sizePtr, rotPtr, colorPtr);
	}));

	m_smokeGroup->SetRenderer(Nz::ParticleFunctionRenderer::New([smokeMat] (const Nz::ParticleGroup& /*group*/, const Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, Nz::AbstractRenderQueue* renderQueue)
	{
		auto colorPtr = mapper.GetComponentPtr<const Nz::Color>(Nz::ParticleComponent_Color);
		auto posPtr = mapper.GetComponentPtr<const Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto rotPtr = mapper.GetComponentPtr<const float>(Nz::ParticleComponent_Rotation);
		auto sizePtr = mapper.GetComponentPtr<const Nz::Vector2f>(Nz::ParticleComponent_Size);

		renderQueue->AddBillboards(0, smokeMat, endId - startId + 1, Nz::Recti(-1, -1), posPtr, sizePtr, rotPtr, colorPtr);
	}));

	//////////////////////////////////////////////////////////////////////////

	m_ambientMusic.Play();
	m_turretFireSound.LoadFromFile("resources/turretFire.wav");
	m_turretReloadSound.LoadFromFile("resources/turretReload.wav");

	m_onMouseMoved.Connect(m_shared.target->GetEventHandler().OnMouseMoved, this, &SpacebattleExample::OnMouseMoved);
	m_shared.target->SetCursor(Nz::SystemCursor_None);

	//////////////////////////////////////////////////////////////////////////

	Nz::TextSpriteRef introText = Nz::TextSprite::New();
	introText->Update(Nz::SimpleTextDrawer::Draw("--Tourelle de défense du secteur A407M2--\nLes contrôles ont été adaptés à vos contrôleurs:\nLa souris contrôle l'orientation de la tourelle, cliquez pour tirer.\n", 72));
	introText->SetScale(0.5f);

	m_introText = m_shared.world3D->CreateEntity();
	Ndk::NodeComponent& introNode = m_introText->AddComponent<Ndk::NodeComponent>();
	Ndk::GraphicsComponent& introGfx = m_introText->AddComponent<Ndk::GraphicsComponent>();
	introGfx.Attach(introText, 1);
	RegisterEntity(m_introText);

	Ndk::NodeComponent& cannonNode = m_turret.cannonEntity->GetComponent<Ndk::NodeComponent>();

	Nz::Boxf introAABB = introGfx.GetBoundingVolume().aabb;
	introNode.SetPosition(cannonNode.GetForward() * 500.f + introNode.GetLeft() * introAABB.width / 2.f + introNode.GetUp() * introAABB.height / 2.f);
}

void SpacebattleExample::Leave(Ndk::StateMachine& fsm)
{
	m_ambientMusic.Stop();
	m_shared.world3D->RemoveSystem<LaserBeamSystem>();
	m_shared.world3D->RemoveSystem<SpaceshipSystem>();
	m_turretFireSound.Stop();
	m_turretReloadSound.Stop();

	ParticleDemo::Leave(fsm);
}

bool SpacebattleExample::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	if (!ParticleDemo::Update(fsm, elapsedTime))
		return false;

	const float speed = 100.f;

	/*if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z))
		m_turretCannonBaseRotation = std::max(m_turretCannonBaseRotation - speed * elapsedTime, -65.f);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::S))
		m_turretCannonBaseRotation = std::min(m_turretCannonBaseRotation + speed * elapsedTime, 40.f);

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Q))
		m_turretBaseRotation += speed * elapsedTime;

	if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::D))
		m_turretBaseRotation -= speed * elapsedTime;*/

	m_turret.cannonBaseEntity->GetComponent<Ndk::NodeComponent>().SetRotation(Nz::EulerAnglesf(m_turretCannonBaseRotation, 0.f, 0.f));
	m_turret.rotatingBaseEntity->GetComponent<Ndk::NodeComponent>().SetRotation(Nz::EulerAnglesf(0.f, m_turretBaseRotation, 0.f));

	bool discharged = m_turretShootTimer < 1.f;
	if (Nz::Mouse::IsButtonPressed(Nz::Mouse::Left) && !discharged)
	{
		m_turretFireSound.Play();

		m_turretShootTimer = -1.f;

		Ndk::NodeComponent& cannonNode = m_turret.cannonEntity->GetComponent<Ndk::NodeComponent>();

		TorpedoParticle* particle = static_cast<TorpedoParticle*>(m_torpedoGroup->CreateParticle());
		particle->color = Nz::Color::White;
		particle->position = cannonNode.ToGlobalPosition(Nz::Vector3f::Forward() * 10.f);
		particle->rotation = 0.f;
		particle->life = 15.f;
		particle->size.Set(13.34f, 7.41f);
		particle->size *= 2.f;
		particle->velocity = cannonNode.GetForward() * 100.f;
	}

	m_turretShootTimer += elapsedTime * 2.f;
	if (discharged && m_turretShootTimer >= 1.f)
		m_turretReloadSound.Play();

	m_turret.cannonEntity->GetComponent<Ndk::NodeComponent>().SetPosition(Nz::Vector3f::Backward() * std::sin(std::min(m_turretShootTimer, 0.f) * float(M_PI)) * 3.f);

	m_spaceshipSpawnCounter += elapsedTime;
	if (m_spaceshipSpawnCounter >= 10.f)
	{
		m_spaceshipSpawnCounter -= 10.f;

		auto& spacestationNode = m_spacestationEntity->GetComponent<Ndk::NodeComponent>();

		const Ndk::EntityHandle& spaceship = m_spaceshipTemplate->Clone();
		RegisterEntity(spaceship);
		auto& nodeComponent = spaceship->GetComponent<Ndk::NodeComponent>();
		auto& spaceshipComponent = spaceship->GetComponent<SpaceshipComponent>();

		spaceshipComponent.targetPos = m_shared.viewer3D->GetComponent<Ndk::NodeComponent>().GetPosition();
		nodeComponent.SetPosition(spacestationNode.GetPosition());
		nodeComponent.SetRotation(Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), spacestationNode.GetRight()));
		nodeComponent.Move(Nz::Vector3f::Forward() * 15.f + Nz::Vector3f::Down() * 5.f, Nz::CoordSys_Local);
	}

	m_introTimer -= elapsedTime;
	if (m_introTimer <= 0.f && m_introText)
		m_introText->Kill();

	return true;
}

void SpacebattleExample::CreateSpaceShip()
{
	m_spacestationEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_spacestationEntity);

	Ndk::NodeComponent& spacestationNode = m_spacestationEntity->AddComponent<Ndk::NodeComponent>();
	spacestationNode.SetPosition(Nz::Vector3f::Forward() * 500.f + Nz::Vector3f::Up() * 200.f + Nz::Vector3f::Right() * 250.f);
	spacestationNode.SetRotation(Nz::EulerAnglesf(0.f, 15.f, 0.f));
	spacestationNode.SetScale(0.1f);

	Ndk::GraphicsComponent& spacestationGfx = m_spacestationEntity->AddComponent<Ndk::GraphicsComponent>();
	spacestationGfx.Attach(&m_spacestationModel);
}

void SpacebattleExample::CreateTurret()
{
	// Fixed base
	m_turret.baseEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_turret.baseEntity);

	Ndk::NodeComponent& baseNode = m_turret.baseEntity->AddComponent<Ndk::NodeComponent>();
	//baseNode.SetParent(m_spacestationEntity);
	baseNode.SetRotation(Nz::EulerAnglesf(0.f, 180.f, 0.f));

	Ndk::GraphicsComponent& baseGfx = m_turret.baseEntity->AddComponent<Ndk::GraphicsComponent>();
	baseGfx.Attach(&m_turret.baseModel);

	// Rotating base
	m_turret.rotatingBaseEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_turret.rotatingBaseEntity);

	Ndk::NodeComponent& rotatingBaseNode = m_turret.rotatingBaseEntity->AddComponent<Ndk::NodeComponent>();
	rotatingBaseNode.SetParent(m_turret.baseEntity);

	Ndk::GraphicsComponent& rotatingBaseGfx = m_turret.rotatingBaseEntity->AddComponent<Ndk::GraphicsComponent>();
	rotatingBaseGfx.Attach(&m_turret.rotatingBaseModel);

	// Cannon base
	m_turret.cannonBaseEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_turret.cannonBaseEntity);

	Ndk::NodeComponent& cannonBaseNode = m_turret.cannonBaseEntity->AddComponent<Ndk::NodeComponent>();
	cannonBaseNode.SetPosition({0.f, 3.39623547f, 0.f});
	cannonBaseNode.SetParent(m_turret.rotatingBaseEntity);

	Ndk::GraphicsComponent& cannonBaseGfx = m_turret.cannonBaseEntity->AddComponent<Ndk::GraphicsComponent>();
	cannonBaseGfx.Attach(&m_turret.cannonBaseModel);

	// Cannon anchor
	m_turret.cannonAnchorEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_turret.cannonAnchorEntity);

	Ndk::NodeComponent& cannonAnchorNode = m_turret.cannonAnchorEntity->AddComponent<Ndk::NodeComponent>();
	cannonAnchorNode.SetPosition({0.f, 2.96482944f, 3.20705462f});
	cannonAnchorNode.SetParent(m_turret.cannonBaseEntity);

	// Cannon
	m_turret.cannonEntity = m_shared.world3D->CreateEntity();
	RegisterEntity(m_turret.cannonEntity);

	Ndk::NodeComponent& cannonNode = m_turret.cannonEntity->AddComponent<Ndk::NodeComponent>();
	cannonNode.SetParent(m_turret.cannonAnchorEntity);
	cannonNode.SetRotation(Nz::EulerAnglesf(0.f, 180.f, 0.f));

	Ndk::GraphicsComponent& cannonGfx = m_turret.cannonEntity->AddComponent<Ndk::GraphicsComponent>();
	cannonGfx.Attach(&m_turret.cannonModel);
}

void SpacebattleExample::OnMouseMoved(const Nz::EventHandler* /*eventHandler*/, const Nz::WindowEvent::MouseMoveEvent& event)
{
	const float speed = 0.1f;

	m_turretCannonBaseRotation = Nz::Clamp(m_turretCannonBaseRotation + speed * event.deltaY, -65.f, 40.f);
	m_turretBaseRotation -= event.deltaX * speed;

	Nz::Vector2ui size = m_shared.target->GetSize();
	Nz::Mouse::SetPosition(size.x / 2, size.y / 2, *m_shared.target);
}
