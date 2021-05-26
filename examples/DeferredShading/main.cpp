#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Shader.hpp>
#include <Nazara/Shader/SpirvConstantCache.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <iostream>
#include <random>

/*
[layout(std140)]
struct PointLight
{
	color: vec3<f32>,
	position: vec3<f32>,

	constant: f32,
	linear: f32,
	quadratic: f32,
}

[layout(std140)]
struct SpotLight
{
	color: vec3<f32>,
	position: vec3<f32>,
	direction: vec3<f32>,

	constant: f32,
	linear: f32,
	quadratic: f32,

	innerAngle: f32,
	outerAngle: f32,
}
*/

struct PointLight
{
	Nz::Color color = Nz::Color::White;
	Nz::Vector3f position = Nz::Vector3f::Zero();

	float radius = 1.f;
};

struct SpotLight
{
	Nz::Color color = Nz::Color::White;
	Nz::Matrix4f transformMatrix;
	Nz::Vector3f position = Nz::Vector3f::Zero();
	Nz::Vector3f direction = Nz::Vector3f::Forward();

	float radius = 1.f;

	Nz::RadianAnglef innerAngle = Nz::DegreeAnglef(15.f);
	Nz::RadianAnglef outerAngle = Nz::DegreeAnglef(20.f);
};

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Graphics> nazara(rendererConfig);

	Nz::RenderWindow window;

	Nz::MeshParams meshParams;
	meshParams.storage = Nz::DataStorage::Software;
	meshParams.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, 90.f, 180.f)) * Nz::Matrix4f::Scale(Nz::Vector3f(0.002f));
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();
	const Nz::RenderDeviceInfo& deviceInfo = device->GetDeviceInfo();

	std::string windowTitle = "Graphics Test";
	if (!window.Create(device, Nz::VideoMode(1920, 1080, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	std::shared_ptr<Nz::Mesh> spaceship = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!spaceship)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = std::make_shared<Nz::GraphicalMesh>(*spaceship);

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	Nz::MeshParams meshPrimitiveParams;
	meshPrimitiveParams.storage = Nz::DataStorage::Software;

	std::shared_ptr<Nz::Mesh> planeMesh = std::make_shared<Nz::Mesh>();
	planeMesh->CreateStatic();
	planeMesh->BuildSubMesh(Nz::Primitive::Plane(Nz::Vector2f(20.f, 20.f), Nz::Vector2ui(0u), Nz::Matrix4f::Rotate(Nz::EulerAnglesf(180.f, 0.f, 0.f)), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);
	//planeMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), planeParams);
	planeMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = std::make_shared<Nz::GraphicalMesh>(*planeMesh);

	meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ);

	std::shared_ptr<Nz::Mesh> coneMesh = std::make_shared<Nz::Mesh>();
	coneMesh->CreateStatic();
	coneMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), meshPrimitiveParams);
	coneMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> coneMeshGfx = std::make_shared<Nz::GraphicalMesh>(*coneMesh);

	auto customSettings = Nz::BasicMaterial::GetSettings()->GetBuilderData();
	customSettings.shaders[UnderlyingCast(Nz::ShaderStageType::Fragment)] = std::make_shared<Nz::UberShader>(Nz::ShaderStageType::Fragment, Nz::ShaderLang::Parse(resourceDir / "deferred_frag.nzsl"));
	customSettings.shaders[UnderlyingCast(Nz::ShaderStageType::Vertex)] = std::make_shared<Nz::UberShader>(Nz::ShaderStageType::Vertex, Nz::ShaderLang::Parse(resourceDir / "deferred_vert.nzsl"));

	auto customMatSettings = std::make_shared<Nz::MaterialSettings>(std::move(customSettings));

	std::shared_ptr<Nz::Material> spaceshipMat = std::make_shared<Nz::Material>(customMatSettings);
	spaceshipMat->EnableDepthBuffer(true);
	{
		Nz::BasicMaterial basicMat(*spaceshipMat);
		basicMat.EnableAlphaTest(false);
		basicMat.SetAlphaMap(Nz::Texture::LoadFromFile(resourceDir / "alphatile.png", texParams));
		basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));
	}

	std::shared_ptr<Nz::Material> planeMat = std::make_shared<Nz::Material>(customMatSettings);
	planeMat->EnableDepthBuffer(true);
	{
		Nz::BasicMaterial basicMat(*planeMat);
		basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "dev_grey.png", texParams));

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;
		basicMat.SetDiffuseSampler(planeSampler);
	}

	Nz::Model spaceshipModel(std::move(gfxMesh));
	for (std::size_t i = 0; i < spaceshipModel.GetSubMeshCount(); ++i)
		spaceshipModel.SetMaterial(i, spaceshipMat);

	Nz::Model planeModel(std::move(planeMeshGfx));
	for (std::size_t i = 0; i < planeModel.GetSubMeshCount(); ++i)
		planeModel.SetMaterial(i, planeMat);

	Nz::PredefinedInstanceData instanceUboOffsets = Nz::PredefinedInstanceData::GetOffsets();
	Nz::PredefinedViewerData viewerUboOffsets = Nz::PredefinedViewerData::GetOffsets();

	std::vector<std::uint8_t> viewerDataBuffer(viewerUboOffsets.totalSize);

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::AccessByOffset<Nz::Matrix4f&>(viewerDataBuffer.data(), viewerUboOffsets.viewMatrixOffset) = Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1);
	Nz::AccessByOffset<Nz::Matrix4f&>(viewerDataBuffer.data(), viewerUboOffsets.projMatrixOffset) = Nz::Matrix4f::Perspective(70.f, float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
	Nz::AccessByOffset<Nz::Vector2f&>(viewerDataBuffer.data(), viewerUboOffsets.invTargetSizeOffset) = 1.f / Nz::Vector2f(window.GetSize().x, window.GetSize().y);

	std::vector<std::uint8_t> instanceDataBuffer(instanceUboOffsets.totalSize);

	Nz::ModelInstance modelInstance1(spaceshipMat->GetSettings());
	{
		spaceshipMat->UpdateShaderBinding(modelInstance1.GetShaderBinding());

		Nz::AccessByOffset<Nz::Matrix4f&>(instanceDataBuffer.data(), instanceUboOffsets.worldMatrixOffset) = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right());

		std::shared_ptr<Nz::AbstractBuffer>& instanceDataUBO = modelInstance1.GetInstanceBuffer();
		instanceDataUBO->Fill(instanceDataBuffer.data(), 0, instanceDataBuffer.size());
	}

	Nz::ModelInstance modelInstance2(spaceshipMat->GetSettings());
	{
		spaceshipMat->UpdateShaderBinding(modelInstance2.GetShaderBinding());

		Nz::AccessByOffset<Nz::Matrix4f&>(instanceDataBuffer.data(), instanceUboOffsets.worldMatrixOffset) = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right() * 3.f);

		std::shared_ptr<Nz::AbstractBuffer>& instanceDataUBO = modelInstance2.GetInstanceBuffer();
		instanceDataUBO->Fill(instanceDataBuffer.data(), 0, instanceDataBuffer.size());
	}

	Nz::ModelInstance planeInstance(planeMat->GetSettings());
	{
		planeMat->UpdateShaderBinding(planeInstance.GetShaderBinding());

		Nz::AccessByOffset<Nz::Matrix4f&>(instanceDataBuffer.data(), instanceUboOffsets.worldMatrixOffset) = Nz::Matrix4f::Translate(Nz::Vector3f::Up() * 2.f);

		std::shared_ptr<Nz::AbstractBuffer>& instanceDataUBO = planeInstance.GetInstanceBuffer();
		instanceDataUBO->Fill(instanceDataBuffer.data(), 0, instanceDataBuffer.size());
	}

	std::shared_ptr<Nz::AbstractBuffer> viewerDataUBO = Nz::Graphics::Instance()->GetViewerDataUBO();

	Nz::RenderWindowImpl* windowImpl = window.GetImpl();
	std::shared_ptr<Nz::CommandPool> commandPool = windowImpl->CreateCommandPool(Nz::QueueType::Graphics);

	Nz::RenderPipelineLayoutInfo fullscreenPipelineLayoutInfo;
	fullscreenPipelineLayoutInfo.bindings.push_back({
		Nz::ShaderBindingType::Texture,
		Nz::ShaderStageType::Fragment,
		0
	});

	Nz::RenderPipelineLayoutInfo lightingPipelineLayoutInfo;
	for (unsigned int i = 0; i < 3; ++i)
	{
		lightingPipelineLayoutInfo.bindings.push_back({
			Nz::ShaderBindingType::Texture,
			Nz::ShaderStageType::Fragment,
			i
		});
	}

	lightingPipelineLayoutInfo.bindings.push_back({
		Nz::ShaderBindingType::UniformBuffer,
		Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex,
		3
	});

	lightingPipelineLayoutInfo.bindings.push_back({
		Nz::ShaderBindingType::UniformBuffer,
		Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex,
		4
	});

	/*Nz::FieldOffsets pointLightOffsets(Nz::StructLayout::Std140);
	std::size_t colorOffset = pointLightOffsets.AddField(Nz::StructFieldType::Float3);
	std::size_t positionOffset = pointLightOffsets.AddField(Nz::StructFieldType::Float3);
	std::size_t constantOffset = pointLightOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t linearOffset = pointLightOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t quadraticOffset = pointLightOffsets.AddField(Nz::StructFieldType::Float1);

	std::size_t alignedPointLightSize = Nz::Align(pointLightOffsets.GetSize(), static_cast<std::size_t>(deviceInfo.limits.minUniformBufferOffsetAlignment));*/

	/*
	[layout(std140)]
	struct SpotLight
	{
		color: vec3<f32>,
		position: vec3<f32>,
		direction: vec3<f32>,

		constant: f32,
		linear: f32,
		quadratic: f32,

		innerAngle: f32,
		outerAngle: f32,
	}
	*/

	Nz::FieldOffsets spotLightOffsets(Nz::StructLayout::Std140);
	std::size_t transformMatrixOffset = spotLightOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true);
	std::size_t colorOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float3);
	std::size_t positionOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float3);
	std::size_t directionOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float3);
	std::size_t radiusOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t invRadiusOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t innerAngleOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t outerAngleOffset = spotLightOffsets.AddField(Nz::StructFieldType::Float1);

	std::size_t alignedSpotLightSize = Nz::Align(spotLightOffsets.GetAlignedSize(), static_cast<std::size_t>(deviceInfo.limits.minUniformBufferOffsetAlignment));

	constexpr std::size_t MaxPointLight = 2000;

	std::shared_ptr<Nz::AbstractBuffer> lightUbo = device->InstantiateBuffer(Nz::BufferType::Uniform);
	if (!lightUbo->Initialize(MaxPointLight * alignedSpotLightSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
		return __LINE__;

	std::vector<SpotLight> spotLights;
	/*auto& firstSpot = spotLights.emplace_back();
	firstSpot.position = Nz::Vector3f::Right() + Nz::Vector3f::Forward();
	firstSpot.direction = Nz::Vector3f::Up();*/

	std::random_device rng;
	std::mt19937 randomEngine(rng());
	std::uniform_int_distribution<unsigned int> colorDis(0, 255);
	std::uniform_real_distribution<float> heightDis(1.5f, 1.95f);
	std::uniform_real_distribution<float> posDis(-10.f, 10.f);
	std::uniform_real_distribution<float> dirDis(-1.f, 1.f);
	std::uniform_real_distribution<float> dirYDis(0.f, 0.75f);
	std::uniform_real_distribution<float> radiusDis(1.f, 5.f);

	for (std::size_t i = 0; i < 1000; ++i)
	{
		auto& light = spotLights.emplace_back();
		light.color = Nz::Color(colorDis(randomEngine), colorDis(randomEngine), colorDis(randomEngine));
		light.position = Nz::Vector3f(posDis(randomEngine), heightDis(randomEngine), posDis(randomEngine));
		light.direction = Nz::Vector3f(dirDis(randomEngine), dirYDis(randomEngine), dirDis(randomEngine)).GetNormal();
		light.radius = radiusDis(randomEngine);
	}


	const std::shared_ptr<const Nz::VertexDeclaration>& vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_UV);


	unsigned int offscreenWidth = window.GetSize().x;
	unsigned int offscreenHeight = window.GetSize().y;

	// Fullscreen data

	Nz::RenderPipelineInfo fullscreenPipelineInfo;
	fullscreenPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleStrip;
	fullscreenPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(fullscreenPipelineLayoutInfo);
	fullscreenPipelineInfo.vertexBuffers.push_back({
		0,
		vertexDeclaration
	});

	fullscreenPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment, Nz::ShaderLanguage::NazaraBinary, resourceDir / "fullscreen.frag.shader", {}));
	fullscreenPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraBinary, resourceDir / "fullscreen.vert.shader", {}));


	const std::shared_ptr<const Nz::VertexDeclaration>& lightingVertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_UV);

	std::shared_ptr<Nz::RenderPipeline> fullscreenPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfo);

	Nz::RenderPipelineInfo lightingPipelineInfo;
	lightingPipelineInfo.blending = true;
	lightingPipelineInfo.blend.dstColor = Nz::BlendFunc::One;
	lightingPipelineInfo.blend.srcColor = Nz::BlendFunc::One;
	lightingPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	lightingPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(lightingPipelineLayoutInfo);
	lightingPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});

	lightingPipelineInfo.faceCulling = true;
	lightingPipelineInfo.cullingSide = Nz::FaceSide::Front;

	lightingPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "lighting.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> lightingPipeline = device->InstantiateRenderPipeline(lightingPipelineInfo);

	std::vector<std::shared_ptr<Nz::ShaderBinding>> lightingShaderBindings;

	std::array<Nz::VertexStruct_XYZ_UV, 3> vertexData = {
		{
			{
				Nz::Vector3f(-1.f, 1.f, 0.0f),
				Nz::Vector2f(0.0f, 1.0f),
			},
			{
				Nz::Vector3f(-1.f, -3.f, 0.0f),
				Nz::Vector2f(0.0f, -1.0f),
			},
			{
				Nz::Vector3f(3.f, 1.f, 0.0f),
				Nz::Vector2f(2.0f, 1.0f),
			}
		}
	};

	/*std::array<Nz::VertexStruct_XYZ_UV, 4> vertexData = {
		{
			{
				Nz::Vector3f(-1.f, -1.f, 0.0f),
				Nz::Vector2f(0.0f, 0.0f),
			},
			{
				Nz::Vector3f(1.f, -1.f, 0.0f),
				Nz::Vector2f(1.0f, 0.0f),
			},
			{
				Nz::Vector3f(-1.f, 1.f, 0.0f),
				Nz::Vector2f(0.0f, 1.0f),
			},
			{
				Nz::Vector3f(1.f, 1.f, 0.0f),
				Nz::Vector2f(1.0f, 1.0f),
			},
		}
	};*/

	std::shared_ptr<Nz::AbstractBuffer> vertexBuffer = device->InstantiateBuffer(Nz::BufferType::Vertex);
	if (!vertexBuffer->Initialize(vertexDeclaration->GetStride() * vertexData.size(), Nz::BufferUsage::DeviceLocal))
		return __LINE__;

	if (!vertexBuffer->Fill(vertexData.data(), 0, vertexBuffer->GetSize()))
		return __LINE__;

	std::shared_ptr<Nz::ShaderBinding> finalBlitBinding = fullscreenPipelineInfo.pipelineLayout->AllocateShaderBinding();

	std::size_t colorTexture;
	std::size_t normalTexture;
	std::size_t positionTexture;
	std::size_t depthBuffer;
	std::size_t backbuffer;

	bool viewerUboUpdate = true;

	Nz::BakedFrameGraph bakedGraph = [&]{
		Nz::FrameGraph graph;

		colorTexture = graph.AddAttachment({
			"Color",
			Nz::PixelFormat::RGBA8
		});
		
		normalTexture = graph.AddAttachment({
			"Normal",
			Nz::PixelFormat::RGBA8
		});

		positionTexture = graph.AddAttachment({
			"Position",
			Nz::PixelFormat::RGBA32F
		});

		depthBuffer = graph.AddAttachment({
			"Depth buffer",
			Nz::PixelFormat::Depth24Stencil8
		});

		backbuffer = graph.AddAttachment({
			"Backbuffer",
			Nz::PixelFormat::RGBA8
		});

		Nz::FramePass& gbufferPass = graph.AddPass("GBuffer");

		std::size_t geometryAlbedo = gbufferPass.AddOutput(colorTexture);
		gbufferPass.SetClearColor(geometryAlbedo, Nz::Color::Black);

		std::size_t geometryNormal = gbufferPass.AddOutput(normalTexture);
		gbufferPass.SetClearColor(geometryNormal, Nz::Color::Black);

		std::size_t positionAttachment = gbufferPass.AddOutput(positionTexture);
		gbufferPass.SetClearColor(positionAttachment, Nz::Color::Black);

		gbufferPass.SetDepthStencilClear(1.f, 0);

		gbufferPass.SetDepthStencilOutput(depthBuffer);

		gbufferPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder)
		{
			builder.SetScissor(Nz::Recti{ 0, 0, int(offscreenWidth), int(offscreenHeight) });
			builder.SetViewport(Nz::Recti{ 0, 0, int(offscreenWidth), int(offscreenHeight) });

			for (Nz::ModelInstance& modelInstance : { std::ref(modelInstance1), std::ref(modelInstance2) })
			{
				builder.BindShaderBinding(modelInstance.GetShaderBinding());

				for (std::size_t i = 0; i < spaceshipModel.GetSubMeshCount(); ++i)
				{
					builder.BindIndexBuffer(spaceshipModel.GetIndexBuffer(i).get());
					builder.BindVertexBuffer(0, spaceshipModel.GetVertexBuffer(i).get());
					builder.BindPipeline(*spaceshipModel.GetRenderPipeline(i));

					builder.DrawIndexed(static_cast<Nz::UInt32>(spaceshipModel.GetIndexCount(i)));
				}
			}

			// Plane
			builder.BindShaderBinding(planeInstance.GetShaderBinding());

			for (std::size_t i = 0; i < planeModel.GetSubMeshCount(); ++i)
			{
				builder.BindIndexBuffer(planeModel.GetIndexBuffer(i).get());
				builder.BindVertexBuffer(0, planeModel.GetVertexBuffer(i).get());
				builder.BindPipeline(*planeModel.GetRenderPipeline(i));

				builder.DrawIndexed(static_cast<Nz::UInt32>(planeModel.GetIndexCount(i)));
			}
		});

		Nz::FramePass& lightingPass = graph.AddPass("Lighting pass");
		lightingPass.SetExecutionCallback([&]
		{
			return (viewerUboUpdate) ? Nz::FramePassExecution::UpdateAndExecute : Nz::FramePassExecution::Execute;
		});

		lightingPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder)
		{
			builder.SetScissor(Nz::Recti{ 0, 0, int(offscreenWidth), int(offscreenHeight) });
			builder.SetViewport(Nz::Recti{ 0, 0, int(offscreenWidth), int(offscreenHeight) });

			builder.BindPipeline(*lightingPipeline);
			//builder.BindVertexBuffer(0, vertexBuffer.get());
			builder.BindIndexBuffer(coneMeshGfx->GetIndexBuffer(0).get());
			builder.BindVertexBuffer(0, coneMeshGfx->GetVertexBuffer(0).get());

			for (std::size_t i = 0; i < spotLights.size(); ++i)
			{
				builder.BindShaderBinding(*lightingShaderBindings[i]);
				builder.DrawIndexed(coneMeshGfx->GetIndexCount(0));
			}
		});

		lightingPass.AddInput(colorTexture);
		lightingPass.AddInput(normalTexture);
		lightingPass.AddInput(positionTexture);
		lightingPass.SetClearColor(lightingPass.AddOutput(backbuffer), Nz::Color::Black);
		//lightingPass.SetDepthStencilInput(depthBuffer);

		graph.SetBackbufferOutput(backbuffer);

		return graph.Bake();
	}();

	bakedGraph.Resize(offscreenWidth, offscreenHeight);

	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});


	for (std::size_t i = 0; i < MaxPointLight; ++i)
	{
		std::shared_ptr<Nz::ShaderBinding> lightingShaderBinding = lightingPipelineInfo.pipelineLayout->AllocateShaderBinding();
		lightingShaderBinding->Update({
			{
				0,
				Nz::ShaderBinding::TextureBinding {
					bakedGraph.GetAttachmentTexture(colorTexture).get(),
					textureSampler.get()
				}
			},
			{
				1,
				Nz::ShaderBinding::TextureBinding {
					bakedGraph.GetAttachmentTexture(normalTexture).get(),
					textureSampler.get()
				}
			},
			{
				2,
				Nz::ShaderBinding::TextureBinding {
					bakedGraph.GetAttachmentTexture(positionTexture).get(),
					textureSampler.get()
				}
			},
			{
				3,
				Nz::ShaderBinding::UniformBufferBinding {
					lightUbo.get(),
					i * alignedSpotLightSize, spotLightOffsets.GetAlignedSize()
				}
			},
			{
				4,
				Nz::ShaderBinding::UniformBufferBinding {
					viewerDataUBO.get(),
					0, viewerDataUBO->GetSize()
				}
			}
		});

		lightingShaderBindings.emplace_back(std::move(lightingShaderBinding));
	}

	finalBlitBinding->Update({
		{
			0,
			Nz::ShaderBinding::TextureBinding {
				bakedGraph.GetAttachmentTexture(backbuffer).get(),
				textureSampler.get()
			}
		}
	});


	Nz::CommandBufferPtr drawCommandBuffer;
	auto RebuildCommandBuffer = [&]
	{
		Nz::Vector2ui windowSize = window.GetSize();

		drawCommandBuffer = commandPool->BuildCommandBuffer([&](Nz::CommandBufferBuilder& builder)
		{
			Nz::Recti windowRenderRect(0, 0, window.GetSize().x, window.GetSize().y);

			builder.TextureBarrier(Nz::PipelineStage::ColorOutput, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ColorWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::ColorOutput, Nz::TextureLayout::ColorInput, *bakedGraph.GetAttachmentTexture(backbuffer));

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green);
			{
				builder.BeginRenderPass(windowImpl->GetFramebuffer(), windowImpl->GetRenderPass(), windowRenderRect);
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.BindShaderBinding(*finalBlitBinding);
					builder.BindPipeline(*fullscreenPipeline);
					builder.BindVertexBuffer(0, vertexBuffer.get());
					builder.Draw(3);
				}
				builder.EndRenderPass();
			}
			builder.EndDebugRegion();
		});
	};
	RebuildCommandBuffer();


	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	std::size_t totalFrameCount = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	float elapsedTime = 0.f;
	Nz::UInt64 time = Nz::GetElapsedMicroseconds();

	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		elapsedTime += (now - time) / 1'000'000.f;
		time = now;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				case Nz::WindowEventType::MouseMoved: // La souris a bougé
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch + event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camQuat = camAngles;
					
					viewerUboUpdate = true;
					break;
				}

				case Nz::WindowEventType::KeyPressed:
				{
					if (event.key.scancode == Nz::Keyboard::Scancode::Space)
					{
						auto& whiteLight = spotLights.emplace_back();
						whiteLight.radius = 5.f;
						whiteLight.position = viewerPos;
						whiteLight.direction = camQuat * Nz::Vector3f::Forward();

						viewerUboUpdate = true;
					}
					break;
				}

				case Nz::WindowEventType::Resized:
				{
					Nz::Vector2ui windowSize = window.GetSize();
					Nz::AccessByOffset<Nz::Matrix4f&>(viewerDataBuffer.data(), viewerUboOffsets.projMatrixOffset) = Nz::Matrix4f::Perspective(70.f, float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
					viewerUboUpdate = true;
					break;
				}

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float cameraSpeed = 2.f * updateClock.GetSeconds();
			updateClock.Restart();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				viewerPos += camQuat * Nz::Vector3f::Forward() * cameraSpeed;

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				viewerPos += camQuat * Nz::Vector3f::Backward() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				viewerPos += camQuat * Nz::Vector3f::Left() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				viewerPos += camQuat * Nz::Vector3f::Right() * cameraSpeed;

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				viewerPos += Nz::Vector3f::Up() * cameraSpeed;

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				viewerPos += Nz::Vector3f::Down() * cameraSpeed;

			viewerUboUpdate = true;
		}

		Nz::RenderFrame frame = windowImpl->Acquire();
		if (!frame)
			continue;

		if (frame.IsFramebufferInvalidated())
			RebuildCommandBuffer();

		//if (viewerUboUpdate)
		{
			Nz::AccessByOffset<Nz::Matrix4f&>(viewerDataBuffer.data(), viewerUboOffsets.viewMatrixOffset) = Nz::Matrix4f::ViewMatrix(viewerPos, camAngles);

			//Nz::AccessByOffset<Nz::Vector3f&>(lightData.data(), colorOffset) = Nz::Vector3f(std::sin(totalFrameCount / 10000.f) * 0.5f + 0.5f, std::cos(totalFrameCount / 1000.f) * 0.5f + 0.5f, std::sin(totalFrameCount / 100.f) * 0.5f + 0.5f);

			Nz::UploadPool& uploadPool = frame.GetUploadPool();

			frame.Execute([&](Nz::CommandBufferBuilder& builder)
			{
				builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow);
				{
					builder.PreTransferBarrier();

					auto& viewerDataAllocation = uploadPool.Allocate(viewerDataBuffer.size());
					std::memcpy(viewerDataAllocation.mappedPtr, viewerDataBuffer.data(), viewerDataBuffer.size());
					builder.CopyBuffer(viewerDataAllocation, viewerDataUBO.get());

					if (!spotLights.empty())
					{
						auto& lightDataAllocation = uploadPool.Allocate(alignedSpotLightSize * spotLights.size());
						Nz::UInt8* lightDataPtr = static_cast<Nz::UInt8*>(lightDataAllocation.mappedPtr);
						for (const SpotLight& spotLight : spotLights)
						{
							Nz::Vector3f direction = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, elapsedTime * 90.f, 0.f)) * spotLight.direction;

							Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, colorOffset) = Nz::Vector3f(spotLight.color.r / 255.f, spotLight.color.g / 255.f, spotLight.color.b / 255.f);
							Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, positionOffset) = spotLight.position;
							Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, directionOffset) = direction;
							Nz::AccessByOffset<float&>(lightDataPtr, radiusOffset) = spotLight.radius;
							Nz::AccessByOffset<float&>(lightDataPtr, invRadiusOffset) = 1.f / spotLight.radius;
							Nz::AccessByOffset<float&>(lightDataPtr, innerAngleOffset) = spotLight.innerAngle.GetCos();
							Nz::AccessByOffset<float&>(lightDataPtr, outerAngleOffset) = spotLight.outerAngle.GetCos();

							float baseRadius = spotLight.radius * spotLight.outerAngle.GetTan() * 1.1f;
							Nz::Matrix4f transformMatrix = Nz::Matrix4f::Transform(spotLight.position, Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), direction), Nz::Vector3f(baseRadius, baseRadius, spotLight.radius));
							Nz::AccessByOffset<Nz::Matrix4f&>(lightDataPtr, transformMatrixOffset) = transformMatrix;

							lightDataPtr += alignedSpotLightSize;
						}

						builder.CopyBuffer(lightDataAllocation, lightUbo.get());
					}

					spaceshipMat->UpdateBuffers(uploadPool, builder);

					builder.PostTransferBarrier();
				}
				builder.EndDebugRegion();
			}, Nz::QueueType::Transfer);
		}

		bakedGraph.Execute(frame);
		frame.SubmitCommandBuffer(drawCommandBuffer.get(), Nz::QueueType::Graphics);

		frame.Present();

		window.Display();

		viewerUboUpdate = false;

		// On incrémente le compteur de FPS improvisé
		fps++;
		totalFrameCount++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard, quel que soit l'encodage du fichier,
			via quelque chose de similaire à u8"Cha\u00CEne de caract\u00E8res".
			Cependant, si le code source est encodé en UTF-8 (Comme c'est le cas dans ce fichier),
			cela fonctionnera aussi comme ceci : "Chaîne de caractères".
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;

			// Et on relance l'horloge pour refaire ça dans une seconde
			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}
