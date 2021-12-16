#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Shader.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <iostream>
#include <random>

NAZARA_REQUEST_DEDICATED_GPU()

constexpr std::size_t BloomSubdivisionCount = 5;

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
	meshParams.center = true;
	meshParams.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, 90.f, 0.f)) * Nz::Matrix4f::Scale(Nz::Vector3f(0.002f));
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

	// Plane
	Nz::MeshParams meshPrimitiveParams;
	meshPrimitiveParams.storage = Nz::DataStorage::Software;

	std::shared_ptr<Nz::Mesh> planeMesh = std::make_shared<Nz::Mesh>();
	planeMesh->CreateStatic();
	planeMesh->BuildSubMesh(Nz::Primitive::Plane(Nz::Vector2f(25.f, 25.f), Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);
	//planeMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), planeParams);
	planeMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = std::make_shared<Nz::GraphicalMesh>(*planeMesh);

	// Skybox
	meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ);

	std::shared_ptr<Nz::Mesh> cubeMesh = std::make_shared<Nz::Mesh>();
	cubeMesh->CreateStatic();
	cubeMesh->BuildSubMesh(Nz::Primitive::Box(Nz::Vector3f::Unit(), Nz::Vector3ui(0), Nz::Matrix4f::Scale({ 1.f, -1.f, 1.f })), meshPrimitiveParams);
	cubeMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> cubeMeshGfx = std::make_shared<Nz::GraphicalMesh>(*cubeMesh);

	Nz::RenderPipelineLayoutInfo skyboxPipelineLayoutInfo;
	Nz::Graphics::FillViewerPipelineLayout(skyboxPipelineLayoutInfo, 0);

	auto& textureBinding = skyboxPipelineLayoutInfo.bindings.emplace_back();
	textureBinding.setIndex = 0;
	textureBinding.bindingIndex = 1;
	textureBinding.shaderStageFlags = Nz::ShaderStageType::Fragment;
	textureBinding.type = Nz::ShaderBindingType::Texture;

	std::shared_ptr<Nz::RenderPipelineLayout> skyboxPipelineLayout = device->InstantiateRenderPipelineLayout(std::move(skyboxPipelineLayoutInfo));

	Nz::RenderPipelineInfo skyboxPipelineInfo;
	skyboxPipelineInfo.depthBuffer = true;
	skyboxPipelineInfo.depthCompare = Nz::RendererComparison::Equal;
	skyboxPipelineInfo.faceCulling = true;
	skyboxPipelineInfo.cullingSide = Nz::FaceSide::Front;
	skyboxPipelineInfo.pipelineLayout = skyboxPipelineLayout;
	skyboxPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "skybox.nzsl", {}));
	skyboxPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});

	std::shared_ptr<Nz::RenderPipeline> skyboxPipeline = device->InstantiateRenderPipeline(std::move(skyboxPipelineInfo));

	// Skybox
	std::shared_ptr<Nz::Texture> skyboxTexture;
	{
		Nz::Image skyboxImage(Nz::ImageType::Cubemap, Nz::PixelFormat::RGBA8, 2048, 2048);
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveX, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_right1.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveY, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_top3.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::PositiveZ, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_front5.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeX, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_left2.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeY, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_bottom4.png");
		skyboxImage.LoadFaceFromFile(Nz::CubemapFace::NegativeZ, resourceDir / "purple_nebula_skybox/purple_nebula_skybox_back6.png");

		skyboxTexture = Nz::Texture::CreateFromImage(skyboxImage, texParams);
	}


	// Cone mesh
	std::shared_ptr<Nz::Mesh> coneMesh = std::make_shared<Nz::Mesh>();
	coneMesh->CreateStatic();
	coneMesh->BuildSubMesh(Nz::Primitive::Cone(1.f, 1.f, 16, Nz::Matrix4f::Rotate(Nz::EulerAnglesf(90.f, 0.f, 0.f))), meshPrimitiveParams);
	coneMesh->SetMaterialCount(1);

	std::shared_ptr<Nz::GraphicalMesh> coneMeshGfx = std::make_shared<Nz::GraphicalMesh>(*coneMesh);

	auto customSettings = Nz::BasicMaterial::GetSettings()->GetBuilderData();
	customSettings.shaders.clear();
	customSettings.shaders.emplace_back(std::make_shared<Nz::UberShader>(Nz::ShaderStageType::Fragment, Nz::ShaderLang::Parse(resourceDir / "deferred_frag.nzsl")));
	customSettings.shaders.emplace_back(std::make_shared<Nz::UberShader>(Nz::ShaderStageType::Vertex, Nz::ShaderLang::Parse(resourceDir / "deferred_vert.nzsl")));

	auto customMatSettings = std::make_shared<Nz::MaterialSettings>(std::move(customSettings));

	std::shared_ptr<Nz::Material> spaceshipMat = std::make_shared<Nz::Material>();

	std::shared_ptr<Nz::MaterialPass> spaceshipMatPass = std::make_shared<Nz::MaterialPass>(customMatSettings);
	spaceshipMatPass->EnableDepthBuffer(true);
	{
		Nz::BasicMaterial basicMat(*spaceshipMatPass);
		basicMat.EnableAlphaTest(false);
		basicMat.SetAlphaMap(Nz::Texture::LoadFromFile(resourceDir / "alphatile.png", texParams));
		basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));
	}
	spaceshipMat->AddPass("ForwardPass", spaceshipMatPass);

	std::shared_ptr<Nz::Material> planeMat = std::make_shared<Nz::Material>();

	std::shared_ptr<Nz::MaterialPass> planeMatPass = std::make_shared<Nz::MaterialPass>(customMatSettings);
	planeMatPass->EnableDepthBuffer(true);
	{
		Nz::BasicMaterial basicMat(*planeMatPass);
		basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "dev_grey.png", texParams));

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;
		basicMat.SetDiffuseSampler(planeSampler);
	}
	planeMat->AddPass("ForwardPass", planeMatPass);

	Nz::Model spaceshipModel(std::move(gfxMesh), spaceship->GetAABB());
	for (std::size_t i = 0; i < spaceshipModel.GetSubMeshCount(); ++i)
		spaceshipModel.SetMaterial(i, spaceshipMat);

	Nz::Model planeModel(std::move(planeMeshGfx), planeMesh->GetAABB());
	for (std::size_t i = 0; i < planeModel.GetSubMeshCount(); ++i)
		planeModel.SetMaterial(i, planeMat);

	Nz::PredefinedInstanceData instanceUboOffsets = Nz::PredefinedInstanceData::GetOffsets();
	Nz::PredefinedViewerData viewerUboOffsets = Nz::PredefinedViewerData::GetOffsets();

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::ViewerInstance viewerInstance;
	viewerInstance.UpdateTargetSize(Nz::Vector2f(windowSize));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Up() * 1));

	Nz::WorldInstance modelInstance1;
	modelInstance1.UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Left() + Nz::Vector3f::Up()));

	Nz::WorldInstance modelInstance2;
	modelInstance2.UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Right() + Nz::Vector3f::Up()));

	Nz::WorldInstance planeInstance;


	Nz::RenderPipelineLayoutInfo lightingPipelineLayoutInfo;
	Nz::Graphics::FillViewerPipelineLayout(lightingPipelineLayoutInfo, 0);

	for (unsigned int i = 0; i < 3; ++i)
	{
		lightingPipelineLayoutInfo.bindings.push_back({
			0,
			i + 1,
			Nz::ShaderBindingType::Texture,
			Nz::ShaderStageType::Fragment,
		});
	}

	lightingPipelineLayoutInfo.bindings.push_back({
		1,
		0,
		Nz::ShaderBindingType::UniformBuffer,
		Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex,
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
	std::uniform_real_distribution<float> heightDis(0.15f, 1.f);
	std::uniform_real_distribution<float> posDis(-10.f, 10.f);
	std::uniform_real_distribution<float> dirDis(-1.f, 1.f);
	std::uniform_real_distribution<float> dirYDis(-0.33f, 0.f);
	std::uniform_real_distribution<float> radiusDis(1.f, 5.f);

	for (std::size_t i = 0; i < 1000; ++i)
	{
		auto& light = spotLights.emplace_back();
		light.color = Nz::Color(colorDis(randomEngine), colorDis(randomEngine), colorDis(randomEngine));
		light.position = Nz::Vector3f(posDis(randomEngine), heightDis(randomEngine), posDis(randomEngine));
		light.direction = Nz::Vector3f(dirDis(randomEngine), dirYDis(randomEngine), dirDis(randomEngine)).GetNormal();
		light.radius = radiusDis(randomEngine);
	}


	const std::shared_ptr<const Nz::VertexDeclaration>& fullscreenVertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XY_UV);


	unsigned int offscreenWidth = windowSize.x;
	unsigned int offscreenHeight = windowSize.y;

	// Bloom data

	Nz::RenderPipelineLayoutInfo fullscreenPipelineLayoutInfoViewer;
	Nz::Graphics::FillViewerPipelineLayout(fullscreenPipelineLayoutInfoViewer, 0);

	fullscreenPipelineLayoutInfoViewer.bindings.push_back({
		0, 1,
		Nz::ShaderBindingType::Texture,
		Nz::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo fullscreenPipelineInfoViewer;
	fullscreenPipelineInfoViewer.primitiveMode = Nz::PrimitiveMode::TriangleList;
	fullscreenPipelineInfoViewer.pipelineLayout = device->InstantiateRenderPipelineLayout(fullscreenPipelineLayoutInfoViewer);
	fullscreenPipelineInfoViewer.vertexBuffers.push_back({
		0,
		fullscreenVertexDeclaration
	});

	fullscreenPipelineInfoViewer.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "bloom_bright.nzsl", {}));

	std::shared_ptr<Nz::ShaderBinding> bloomBrightShaderBinding;

	std::shared_ptr<Nz::RenderPipeline> bloomBrightPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfoViewer);

	// Gaussian Blur

	Nz::RenderPipelineLayoutInfo gaussianBlurPipelineLayoutInfo = fullscreenPipelineLayoutInfoViewer;

	gaussianBlurPipelineLayoutInfo.bindings.push_back({
		0, 2,
		Nz::ShaderBindingType::UniformBuffer,
		Nz::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo gaussianBlurPipelineInfo = fullscreenPipelineInfoViewer;
	gaussianBlurPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(gaussianBlurPipelineLayoutInfo);

	Nz::FieldOffsets gaussianBlurDataOffsets(Nz::StructLayout::Std140);
	std::size_t gaussianBlurDataDirection = gaussianBlurDataOffsets.AddField(Nz::StructFieldType::Float2);
	std::size_t gaussianBlurDataSize = gaussianBlurDataOffsets.AddField(Nz::StructFieldType::Float1);

	gaussianBlurPipelineInfo.shaderModules.clear();
	gaussianBlurPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "gaussian_blur.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> gaussianBlurPipeline = device->InstantiateRenderPipeline(gaussianBlurPipelineInfo);
	std::vector<std::shared_ptr<Nz::ShaderBinding>> gaussianBlurShaderBinding(BloomSubdivisionCount * 2);

	std::vector<Nz::UInt8> gaussianBlurData(gaussianBlurDataOffsets.GetSize());
	std::vector<std::shared_ptr<Nz::AbstractBuffer>> gaussianBlurUbos;

	float sizeFactor = 2.f;
	for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
	{
		Nz::AccessByOffset<Nz::Vector2f&>(gaussianBlurData.data(), gaussianBlurDataDirection) = Nz::Vector2f(1.f, 0.f);
		Nz::AccessByOffset<float&>(gaussianBlurData.data(), gaussianBlurDataSize) = sizeFactor;

		std::shared_ptr<Nz::AbstractBuffer> horizontalBlurData = device->InstantiateBuffer(Nz::BufferType::Uniform);
		if (!horizontalBlurData->Initialize(gaussianBlurDataOffsets.GetSize(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
			return __LINE__;

		horizontalBlurData->Fill(gaussianBlurData.data(), 0, gaussianBlurDataOffsets.GetSize());

		Nz::AccessByOffset<Nz::Vector2f&>(gaussianBlurData.data(), gaussianBlurDataDirection) = Nz::Vector2f(0.f, 1.f);

		std::shared_ptr<Nz::AbstractBuffer> verticalBlurData = device->InstantiateBuffer(Nz::BufferType::Uniform);
		if (!verticalBlurData->Initialize(gaussianBlurDataOffsets.GetSize(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic))
			return __LINE__;

		verticalBlurData->Fill(gaussianBlurData.data(), 0, gaussianBlurDataOffsets.GetSize());

		sizeFactor *= 2.f;

		gaussianBlurUbos.push_back(horizontalBlurData);
		gaussianBlurUbos.push_back(verticalBlurData);
	}

	// Tone mapping
	std::shared_ptr<Nz::ShaderBinding> toneMappingShaderBinding;

	fullscreenPipelineInfoViewer.shaderModules.clear();
	fullscreenPipelineInfoViewer.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "tone_mapping.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> toneMappingPipeline = device->InstantiateRenderPipeline(fullscreenPipelineInfoViewer);

	// Bloom blend

	std::shared_ptr<Nz::ShaderBinding> bloomBlitBinding;

	Nz::RenderPipelineLayoutInfo bloomBlendPipelineLayoutInfo;
	Nz::Graphics::FillViewerPipelineLayout(bloomBlendPipelineLayoutInfo, 0);

	/*bloomBlendPipelineLayoutInfo.bindings.push_back({
		0, 1,
		Nz::ShaderBindingType::Texture,
		Nz::ShaderStageType::Fragment,
	});*/

	bloomBlendPipelineLayoutInfo.bindings.push_back({
		0, 2,
		Nz::ShaderBindingType::Texture,
		Nz::ShaderStageType::Fragment,
	});


	Nz::RenderPipelineInfo bloomBlendPipelineInfo;
	bloomBlendPipelineInfo.blending = true;
	bloomBlendPipelineInfo.blend.dstColor = Nz::BlendFunc::One;
	bloomBlendPipelineInfo.blend.srcColor = Nz::BlendFunc::One;
	bloomBlendPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	bloomBlendPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(bloomBlendPipelineLayoutInfo);
	bloomBlendPipelineInfo.vertexBuffers.push_back({
		0,
		fullscreenVertexDeclaration
	});

	bloomBlendPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "bloom_final.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> bloomBlendPipeline = device->InstantiateRenderPipeline(bloomBlendPipelineInfo);

	std::vector<std::shared_ptr<Nz::ShaderBinding>> bloomBlendShaderBinding(BloomSubdivisionCount);

	// Gamma correction
	
	Nz::RenderPipelineLayoutInfo fullscreenPipelineLayoutInfo;

	fullscreenPipelineLayoutInfo.bindings.push_back({
		0, 0,
		Nz::ShaderBindingType::Texture,
		Nz::ShaderStageType::Fragment,
	});

	Nz::RenderPipelineInfo fullscreenPipelineInfo;
	fullscreenPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	fullscreenPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(fullscreenPipelineLayoutInfo);
	fullscreenPipelineInfo.vertexBuffers.push_back({
		0,
		fullscreenVertexDeclaration
	});

	fullscreenPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "gamma.nzsl", {}));

	// God rays

	Nz::RenderPipelineLayoutInfo godraysPipelineLayoutInfo;

	godraysPipelineLayoutInfo.bindings = {
		{
			{
				0, 0,
				Nz::ShaderBindingType::UniformBuffer,
				Nz::ShaderStageType::Fragment,
			},
			{
				0, 1,
				Nz::ShaderBindingType::UniformBuffer,
				Nz::ShaderStageType::Fragment,
			},
			{
				0, 2,
				Nz::ShaderBindingType::Texture,
				Nz::ShaderStageType::Fragment,
			},
		}
	};

	Nz::RenderPipelineInfo godraysPipelineInfo;
	godraysPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	godraysPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(godraysPipelineLayoutInfo);
	godraysPipelineInfo.vertexBuffers.push_back({
		0,
		fullscreenVertexDeclaration
	});

	godraysPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "god_rays.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> godraysPipeline = device->InstantiateRenderPipeline(godraysPipelineInfo);

	Nz::FieldOffsets godraysFieldOffsets(Nz::StructLayout::Std140);
	std::size_t gr_exposureOffset = godraysFieldOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t gr_decayOffset = godraysFieldOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t gr_densityOffset = godraysFieldOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t gr_weightOffset = godraysFieldOffsets.AddField(Nz::StructFieldType::Float1);
	std::size_t gr_lightPositionOffset = godraysFieldOffsets.AddField(Nz::StructFieldType::Float2);

	std::shared_ptr<Nz::ShaderBinding> godRaysShaderBinding = godraysPipelineInfo.pipelineLayout->AllocateShaderBinding(0);

	/*
			uniformExposure = 0.0034f;
		uniformDecay = 1.0f;
		uniformDensity = 0.84f;
		uniformWeight = 5.65f;
	*/

	std::vector<Nz::UInt8> godRaysData(godraysFieldOffsets.GetSize());
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_exposureOffset) = 0.0034f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_decayOffset) = 0.9f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_densityOffset) = 0.84f;
	Nz::AccessByOffset<float&>(godRaysData.data(), gr_weightOffset) = 5.65f;
	Nz::AccessByOffset<Nz::Vector2f&>(godRaysData.data(), gr_lightPositionOffset) = Nz::Vector2f(0.5f, 0.1f);

	std::shared_ptr<Nz::AbstractBuffer> godRaysUBO = device->InstantiateBuffer(Nz::BufferType::Uniform);
	godRaysUBO->Initialize(godRaysData.size(), Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic);
	godRaysUBO->Fill(godRaysData.data(), 0, godRaysData.size());

	std::shared_ptr<Nz::ShaderBinding> godRaysBlitShaderBinding;


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
	lightingPipelineInfo.depthBuffer = false;
	lightingPipelineInfo.faceCulling = true;
	lightingPipelineInfo.cullingSide = Nz::FaceSide::Front;
	lightingPipelineInfo.stencilTest = true;
	lightingPipelineInfo.stencilBack.compare = Nz::RendererComparison::NotEqual;
	lightingPipelineInfo.stencilBack.fail = Nz::StencilOperation::Zero;
	lightingPipelineInfo.stencilBack.depthFail = Nz::StencilOperation::Zero;
	lightingPipelineInfo.stencilBack.pass = Nz::StencilOperation::Zero;

	lightingPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "lighting.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> lightingPipeline = device->InstantiateRenderPipeline(lightingPipelineInfo);

	Nz::RenderPipelineInfo stencilPipelineInfo;
	stencilPipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleList;
	stencilPipelineInfo.pipelineLayout = device->InstantiateRenderPipelineLayout(lightingPipelineLayoutInfo);
	stencilPipelineInfo.vertexBuffers.push_back({
		0,
		meshPrimitiveParams.vertexDeclaration
	});

	stencilPipelineInfo.colorWrite = false;
	stencilPipelineInfo.depthBuffer = true;
	stencilPipelineInfo.depthWrite = false;
	stencilPipelineInfo.faceCulling = false;
	stencilPipelineInfo.stencilTest = true;
	stencilPipelineInfo.stencilFront.compare = Nz::RendererComparison::Always;
	stencilPipelineInfo.stencilFront.depthFail = Nz::StencilOperation::Invert;
	stencilPipelineInfo.stencilBack.compare = Nz::RendererComparison::Always;
	stencilPipelineInfo.stencilBack.depthFail = Nz::StencilOperation::Invert;

	stencilPipelineInfo.shaderModules.push_back(device->InstantiateShaderModule(Nz::ShaderStageType::Vertex, Nz::ShaderLanguage::NazaraShader, resourceDir / "lighting.nzsl", {}));

	std::shared_ptr<Nz::RenderPipeline> stencilPipeline = device->InstantiateRenderPipeline(stencilPipelineInfo);


	std::vector<std::shared_ptr<Nz::ShaderBinding>> lightingShaderBindings;

	std::array<Nz::VertexStruct_XY_UV, 3> vertexData = {
		{
			{
				Nz::Vector2f(-1.f, 1.f),
				Nz::Vector2f(0.0f, 1.0f),
			},
			{
				Nz::Vector2f(-1.f, -3.f),
				Nz::Vector2f(0.0f, -1.0f),
			},
			{
				Nz::Vector2f(3.f, 1.f),
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

	std::shared_ptr<Nz::AbstractBuffer> fullscreenVertexBuffer = device->InstantiateBuffer(Nz::BufferType::Vertex);
	if (!fullscreenVertexBuffer->Initialize(fullscreenVertexDeclaration->GetStride() * vertexData.size(), Nz::BufferUsage::DeviceLocal))
		return __LINE__;

	if (!fullscreenVertexBuffer->Fill(vertexData.data(), 0, fullscreenVertexBuffer->GetSize()))
		return __LINE__;

	std::shared_ptr<Nz::ShaderBinding> bloomSkipBlit;
	std::shared_ptr<Nz::ShaderBinding> finalBlitBinding;

	bool lightUpdate = true;
	bool matUpdate = false;

	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	std::shared_ptr<Nz::ShaderBinding> skyboxShaderBinding = skyboxPipelineLayout->AllocateShaderBinding(0);
	skyboxShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::UniformBufferBinding {
				viewerInstance.GetViewerBuffer().get(),
				0, viewerInstance.GetViewerBuffer()->GetSize()
			}
		},
		{
			1,
			Nz::ShaderBinding::TextureBinding {
				skyboxTexture.get(),
				textureSampler.get()
			}
		}
	});

	std::shared_ptr<Nz::ShaderBinding> gbufferShaderBinding;

	bool bloomEnabled = true;
	bool forwardEnabled = true;
	bool lightAnimation = true;

	std::size_t colorTexture;
	std::size_t normalTexture;
	std::size_t positionTexture;
	std::size_t depthBuffer1;
	std::size_t depthBuffer2;
	std::size_t bloomBrightOutput;
	std::size_t bloomOutput;
	std::vector<std::size_t> bloomTextures(BloomSubdivisionCount * 2);
	std::size_t lightOutput;
	std::size_t occluderTexture;
	std::size_t godRaysTexture;

	std::size_t toneMappingOutput;

	Nz::SubmeshRenderer submeshRenderer;
	std::unique_ptr<Nz::ElementRendererData> submeshRendererData = submeshRenderer.InstanciateData();

	std::size_t forwardPassIndex = Nz::Graphics::Instance()->GetMaterialPassRegistry().GetPassIndex("ForwardPass");

	Nz::BakedFrameGraph bakedGraph = [&]
	{
		Nz::PixelFormat depthStencilFormat = Nz::PixelFormat::Undefined;
		for (Nz::PixelFormat candidate : { Nz::PixelFormat::Depth24Stencil8, Nz::PixelFormat::Depth32FStencil8, Nz::PixelFormat::Depth16Stencil8 })
		{
			if (device->IsTextureFormatSupported(candidate, Nz::TextureUsage::DepthStencilAttachment))
			{
				depthStencilFormat = candidate;
				break;
			}
		}

		if (depthStencilFormat == Nz::PixelFormat::Undefined)
		{
			std::cerr << "no depth-stencil format found" << std::endl;
			std::exit(__LINE__);
		}

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

		depthBuffer1 = graph.AddAttachment({
			"Depth buffer",
			depthStencilFormat
		});
		
		depthBuffer2 = graph.AddAttachment({
			"Depth buffer",
			depthStencilFormat
		});

		lightOutput = graph.AddAttachment({
			"Light output",
			Nz::PixelFormat::RGBA16F
		});

		occluderTexture = graph.AddAttachment({
			"Occluder texture",
			Nz::PixelFormat::RGBA8,
		});

		godRaysTexture = graph.AddAttachment({
			"God rays texture",
			Nz::PixelFormat::RGBA16F
		});

		bloomOutput = graph.AddAttachmentProxy("Bloom output", lightOutput);

		unsigned int bloomSize = 50'000;
		bloomBrightOutput = graph.AddAttachment({
			"Bloom bright output",
			Nz::PixelFormat::RGBA16F,
			bloomSize,
			bloomSize
		});

		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
		{
			bloomTextures[i * 2 + 0] = graph.AddAttachment({
				"Bloom texture #" + std::to_string(i),
				Nz::PixelFormat::RGBA16F,
				bloomSize,
				bloomSize
			});


			bloomTextures[i * 2 + 1] = graph.AddAttachment({
				"Bloom texture #" + std::to_string(i),
				Nz::PixelFormat::RGBA16F,
				bloomSize,
				bloomSize
			});

			bloomSize /= 2;
		}

		toneMappingOutput = graph.AddAttachment({
			"Tone mapping",
			Nz::PixelFormat::RGBA8,
			100'000,
			100'000
		});

		Nz::FramePass& gbufferPass = graph.AddPass("GBuffer");

		std::size_t geometryAlbedo = gbufferPass.AddOutput(colorTexture);
		gbufferPass.SetClearColor(geometryAlbedo, Nz::Color::Black);

		std::size_t geometryNormal = gbufferPass.AddOutput(normalTexture);
		gbufferPass.SetClearColor(geometryNormal, Nz::Color::Black);

		std::size_t positionAttachment = gbufferPass.AddOutput(positionTexture);
		gbufferPass.SetClearColor(positionAttachment, Nz::Color::Black);

		gbufferPass.SetDepthStencilClear(1.f, 0);

		gbufferPass.SetDepthStencilOutput(depthBuffer1);

		gbufferPass.SetExecutionCallback([&]
		{
			return (matUpdate) ? Nz::FramePassExecution::UpdateAndExecute : Nz::FramePassExecution::Execute;
		});

		gbufferPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetViewport(renderArea);

			spaceshipModel.UpdateScissorBox(renderArea);
			planeModel.UpdateScissorBox(renderArea);

			std::vector<std::unique_ptr<Nz::RenderElement>> elements;
			spaceshipModel.BuildElement(forwardPassIndex, modelInstance1, elements);
			spaceshipModel.BuildElement(forwardPassIndex, modelInstance2, elements);
			planeModel.BuildElement(forwardPassIndex, planeInstance, elements);

			std::vector<Nz::Pointer<const Nz::RenderElement>> elementPointers;
			elementPointers.reserve(elements.size());
			for (const auto& element : elements)
				elementPointers.emplace_back(element.get());

			Nz::RenderFrame dummy;

			submeshRenderer.Prepare(viewerInstance, *submeshRendererData, dummy, elementPointers.data(), elementPointers.size());
			submeshRenderer.Render(viewerInstance, *submeshRendererData, builder, elementPointers.data(), elementPointers.size());
		});

		Nz::FramePass& lightingPass = graph.AddPass("Lighting pass");
		lightingPass.SetExecutionCallback([&]
		{
			return (lightUpdate) ? Nz::FramePassExecution::UpdateAndExecute : Nz::FramePassExecution::Execute;
		});

		lightingPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			//builder.BindVertexBuffer(0, vertexBuffer.get());
			builder.BindIndexBuffer(*coneMeshGfx->GetIndexBuffer(0).get());
			builder.BindVertexBuffer(0, *coneMeshGfx->GetVertexBuffer(0).get());

			builder.BindShaderBinding(0, *gbufferShaderBinding);

			for (std::size_t i = 0; i < spotLights.size(); ++i)
			{
				builder.BindShaderBinding(1, *lightingShaderBindings[i]);

				builder.BindPipeline(*stencilPipeline);
				builder.DrawIndexed(coneMeshGfx->GetIndexCount(0));

				builder.BindPipeline(*lightingPipeline);
				builder.DrawIndexed(coneMeshGfx->GetIndexCount(0));
			}
		});

		lightingPass.AddInput(colorTexture);
		lightingPass.AddInput(normalTexture);
		lightingPass.AddInput(positionTexture);

		lightingPass.SetClearColor(lightingPass.AddOutput(lightOutput), Nz::Color::Black);
		lightingPass.SetDepthStencilInput(depthBuffer1);

		Nz::FramePass& forwardPass = graph.AddPass("Forward pass");
		forwardPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			builder.BindShaderBinding(0, *skyboxShaderBinding);

			builder.BindIndexBuffer(*cubeMeshGfx->GetIndexBuffer(0));
			builder.BindVertexBuffer(0, *cubeMeshGfx->GetVertexBuffer(0));
			builder.BindPipeline(*skyboxPipeline);

			builder.DrawIndexed(Nz::SafeCast<Nz::UInt32>(cubeMeshGfx->GetIndexCount(0)));
		});
		forwardPass.SetExecutionCallback([&]
		{
			return (forwardEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		forwardPass.AddInput(lightOutput);
		forwardPass.AddOutput(lightOutput);
		forwardPass.SetDepthStencilInput(depthBuffer1);
		forwardPass.SetDepthStencilOutput(depthBuffer2);

		
		Nz::FramePass& occluderPass = graph.AddPass("Occluder pass");
		occluderPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			builder.BindShaderBinding(0, *skyboxShaderBinding);

			builder.BindIndexBuffer(*cubeMeshGfx->GetIndexBuffer(0));
			builder.BindVertexBuffer(0, *cubeMeshGfx->GetVertexBuffer(0));
			builder.BindPipeline(*skyboxPipeline);

			builder.DrawIndexed(Nz::SafeCast<Nz::UInt32>(cubeMeshGfx->GetIndexCount(0)));
		});

		occluderPass.AddOutput(occluderTexture);
		occluderPass.SetClearColor(0, Nz::Color::Black);
		occluderPass.SetDepthStencilInput(depthBuffer1);

		Nz::FramePass& godraysPass = graph.AddPass("Light scattering pass");
		godraysPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			builder.BindShaderBinding(0, *godRaysShaderBinding);

			builder.BindPipeline(*godraysPipeline);
			builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

			builder.Draw(3);
		});

		godraysPass.AddInput(occluderTexture);
		godraysPass.AddOutput(godRaysTexture);


		Nz::FramePass& bloomBrightPass = graph.AddPass("Bloom pass - extract bright pixels");
		bloomBrightPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			builder.BindShaderBinding(0, *bloomBrightShaderBinding);

			builder.BindPipeline(*bloomBrightPipeline);
			builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

			builder.Draw(3);
		});
		bloomBrightPass.SetExecutionCallback([&]
		{
			return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		bloomBrightPass.AddInput(lightOutput);
		bloomBrightPass.AddOutput(bloomBrightOutput);

		std::size_t bloomTextureIndex = 0;
		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
		{
			Nz::FramePass& bloomBlurPassHorizontal = graph.AddPass("Bloom pass - gaussian blur #" + std::to_string(i) + " - horizontal");
			bloomBlurPassHorizontal.SetCommandCallback([&, i](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
			{
				builder.SetScissor(renderArea);
				builder.SetViewport(renderArea);

				builder.BindShaderBinding(0, *gaussianBlurShaderBinding[i * 2 + 0]);
				builder.BindPipeline(*gaussianBlurPipeline);
				builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

				builder.Draw(3);
			});

			bloomBlurPassHorizontal.SetExecutionCallback([&]
			{
				return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
			});

			bloomBlurPassHorizontal.AddInput((i == 0) ? bloomBrightOutput : bloomTextures[bloomTextureIndex++]);
			bloomBlurPassHorizontal.AddOutput(bloomTextures[bloomTextureIndex]);
			
			Nz::FramePass& bloomBlurPassVertical = graph.AddPass("Bloom pass - gaussian blur #" + std::to_string(i) + " - vertical");
			bloomBlurPassVertical.SetCommandCallback([&, i](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
			{
				builder.SetScissor(renderArea);
				builder.SetViewport(renderArea);

				builder.BindShaderBinding(0, *gaussianBlurShaderBinding[i * 2 + 1]);
				builder.BindPipeline(*gaussianBlurPipeline);
				builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

				builder.Draw(3);
			});

			bloomBlurPassVertical.SetExecutionCallback([&]
			{
				return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
			});

			bloomBlurPassVertical.AddInput(bloomTextures[bloomTextureIndex++]);
			bloomBlurPassVertical.AddOutput(bloomTextures[bloomTextureIndex]);
		}

		Nz::FramePass& bloomBlendPass = graph.AddPass("Bloom pass - blend");
		bloomBlendPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);
			builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

			// Blend bloom
			builder.BindPipeline(*bloomBlendPipeline);
			for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			{
				builder.BindShaderBinding(0, *bloomBlendShaderBinding[i]);
				builder.Draw(3);
			}

			// God rays
			builder.BindShaderBinding(0, *godRaysBlitShaderBinding);
			builder.Draw(3);
		});

		bloomBlendPass.SetExecutionCallback([&]
		{
			return (bloomEnabled) ? Nz::FramePassExecution::Execute : Nz::FramePassExecution::Skip;
		});

		bloomBlendPass.AddInput(lightOutput);
		bloomBlendPass.AddInput(godRaysTexture);
		bloomBlendPass.SetReadInput(0, false);

		for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			bloomBlendPass.AddInput(bloomTextures[i * 2 + 1]);

		bloomBlendPass.AddOutput(bloomOutput);

		Nz::FramePass& toneMappingPass = graph.AddPass("Tone mapping");
		toneMappingPass.AddInput(bloomOutput);
		toneMappingPass.AddOutput(toneMappingOutput);
		toneMappingPass.SetCommandCallback([&](Nz::CommandBufferBuilder& builder, const Nz::Recti& renderArea)
		{
			builder.SetScissor(renderArea);
			builder.SetViewport(renderArea);

			builder.BindShaderBinding(0, *toneMappingShaderBinding);
			builder.BindPipeline(*toneMappingPipeline);
			builder.BindVertexBuffer(0, *fullscreenVertexBuffer);

			builder.Draw(3);
		});

		graph.AddBackbufferOutput(toneMappingOutput);

		return graph.Bake();
	}();

	Nz::Vector3f viewerPos = Nz::Vector3f::Backward() * 10.f + Nz::Vector3f::Up() * 3.f;

	Nz::EulerAnglesf camAngles(-30.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	std::size_t totalFrameCount = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	float elapsedTime = 0.f;
	Nz::UInt64 time = Nz::GetElapsedMicroseconds();

	auto ComputeLightAnimationSpeed = [](const Nz::Vector3f& position)
	{
		return position.GetLength() / 15.f;
	};

	auto AnimateLightPosition = [](const Nz::Vector3f& position, float rotationSpeed, float elapsedTime)
	{
		rotationSpeed *= 45.f;

		return Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, elapsedTime * rotationSpeed, 0.f)) * position;
	};

	auto AnimateLightDirection = [](const Nz::Vector3f& direction, float rotationSpeed, float elapsedTime)
	{
		rotationSpeed *= 90.f;

		return Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, elapsedTime * rotationSpeed, 0.f)) * direction;
	};

	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		if (lightAnimation)
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
					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX*sensitivity;
					camAngles.yaw.Normalize();

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camQuat = camAngles;
					break;
				}

				case Nz::WindowEventType::KeyPressed:
				{
					if (event.key.scancode == Nz::Keyboard::Scancode::Space)
					{
						float rotationSpeed = ComputeLightAnimationSpeed(viewerPos);

						auto& whiteLight = spotLights.emplace_back();
						whiteLight.color = Nz::Color(100, 100, 255);
						whiteLight.radius = 5.f;
						whiteLight.position = AnimateLightPosition(viewerPos, rotationSpeed, -elapsedTime);
						whiteLight.direction = AnimateLightDirection(camQuat * Nz::Vector3f::Forward(), rotationSpeed, -elapsedTime);

						lightUpdate = true;
					}
					else if (event.key.virtualKey == Nz::Keyboard::VKey::F)
						forwardEnabled = !forwardEnabled;
					else if (event.key.virtualKey == Nz::Keyboard::VKey::A)
						lightAnimation = !lightAnimation;
					else if (event.key.virtualKey == Nz::Keyboard::VKey::B)
						bloomEnabled = !bloomEnabled;
					else if (event.key.virtualKey == Nz::Keyboard::VKey::E)
						modelInstance1.UpdateWorldMatrix(Nz::Matrix4f::Transform(viewerPos, camQuat));

					break;
				}

				case Nz::WindowEventType::Resized:
				{
					Nz::Vector2ui windowSize = window.GetSize();
					viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f));
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

			viewerInstance.UpdateViewMatrix(Nz::Matrix4f::ViewMatrix(viewerPos, camQuat));
		}

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
			continue;

		if (bakedGraph.Resize(frame))
		{
			frame.PushForRelease(std::move(gbufferShaderBinding));

			gbufferShaderBinding = lightingPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			gbufferShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(colorTexture).get(),
						textureSampler.get()
					}
				},
				{
					2,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(normalTexture).get(),
						textureSampler.get()
					}
				},
				{
					3,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(positionTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(lightingShaderBindings));
			lightingShaderBindings.clear();

			for (std::size_t i = 0; i < MaxPointLight; ++i)
			{
				std::shared_ptr<Nz::ShaderBinding> lightingShaderBinding = lightingPipelineInfo.pipelineLayout->AllocateShaderBinding(1);
				lightingShaderBinding->Update({
					{
						0,
						Nz::ShaderBinding::UniformBufferBinding {
							lightUbo.get(),
							i * alignedSpotLightSize, spotLightOffsets.GetAlignedSize()
						}
					}
				});

				lightingShaderBindings.emplace_back(std::move(lightingShaderBinding));
			}

			frame.PushForRelease(std::move(bloomBrightShaderBinding));

			bloomBrightShaderBinding = fullscreenPipelineInfoViewer.pipelineLayout->AllocateShaderBinding(0);
			bloomBrightShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			std::size_t bloomTextureIndex = 0;
			for (std::size_t i = 0; i < BloomSubdivisionCount; ++i)
			{
				for (std::size_t j = 0; j < 2; ++j)
				{
					frame.PushForRelease(std::move(gaussianBlurShaderBinding[i * 2 + j]));

					gaussianBlurShaderBinding[i * 2 + j] = gaussianBlurPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
					gaussianBlurShaderBinding[i * 2 + j]->Update({
						{
							0,
							Nz::ShaderBinding::UniformBufferBinding {
								viewerInstance.GetViewerBuffer().get(),
								0, viewerInstance.GetViewerBuffer()->GetSize()
							}
						},
						{
							1,
							Nz::ShaderBinding::TextureBinding {
								bakedGraph.GetAttachmentTexture((i == 0 && j == 0) ? bloomBrightOutput : bloomTextures[bloomTextureIndex++]).get(),
								textureSampler.get()
							}
						},
						{
							2,
							Nz::ShaderBinding::UniformBufferBinding {
								gaussianBlurUbos[i * 2 + j].get(),
								0, gaussianBlurUbos[i * 2 + j]->GetSize()
							}
						}
					});
				}

				frame.PushForRelease(std::move(bloomBlendShaderBinding[i]));

				bloomBlendShaderBinding[i] = bloomBlendPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
				bloomBlendShaderBinding[i]->Update({
					{
						0,
						Nz::ShaderBinding::UniformBufferBinding {
							viewerInstance.GetViewerBuffer().get(),
							0, viewerInstance.GetViewerBuffer()->GetSize()
						}
					},
					/*{
						1,
						Nz::ShaderBinding::TextureBinding {
							bakedGraph.GetAttachmentTexture(lightOutput).get(),
							textureSampler.get()
						}
					},*/
					{
						2,
						Nz::ShaderBinding::TextureBinding {
							bakedGraph.GetAttachmentTexture(bloomTextures[i * 2 + 1]).get(),
							textureSampler.get()
						}
					}
				});
			}

			frame.PushForRelease(std::move(bloomBlitBinding));

			bloomBlitBinding = Nz::Graphics::Instance()->GetBlitPipelineLayout()->AllocateShaderBinding(0);
			bloomBlitBinding->Update({
				{
					0,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(bloomSkipBlit));

			bloomSkipBlit = fullscreenPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			bloomSkipBlit->Update({
				{
					0,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(godRaysShaderBinding));

			godRaysShaderBinding = godraysPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			godRaysShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::UniformBufferBinding {
						godRaysUBO.get(),
						0, godRaysUBO->GetSize()
					}
				},
				{
					2,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(occluderTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(toneMappingShaderBinding));

			toneMappingShaderBinding = fullscreenPipelineInfoViewer.pipelineLayout->AllocateShaderBinding(0);
			toneMappingShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				{
					1,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(bloomOutput).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(godRaysBlitShaderBinding));

			godRaysBlitShaderBinding = bloomBlendPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			godRaysBlitShaderBinding->Update({
				{
					0,
					Nz::ShaderBinding::UniformBufferBinding {
						viewerInstance.GetViewerBuffer().get(),
						0, viewerInstance.GetViewerBuffer()->GetSize()
					}
				},
				/*{
					1,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(lightOutput).get(),
						textureSampler.get()
					}
				},*/
				{
					2,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(godRaysTexture).get(),
						textureSampler.get()
					}
				}
			});

			frame.PushForRelease(std::move(finalBlitBinding));

			finalBlitBinding = fullscreenPipelineInfo.pipelineLayout->AllocateShaderBinding(0);
			finalBlitBinding->Update({
				{
					0,
					Nz::ShaderBinding::TextureBinding {
						bakedGraph.GetAttachmentTexture(toneMappingOutput).get(),
						textureSampler.get()
					}
				}
			});
		}

		Nz::UploadPool& uploadPool = frame.GetUploadPool();

		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow);
			{
				builder.PreTransferBarrier();

				modelInstance1.UpdateBuffers(uploadPool, builder);
				modelInstance2.UpdateBuffers(uploadPool, builder);
				planeInstance.UpdateBuffers(uploadPool, builder);

				viewerInstance.UpdateBuffers(uploadPool, builder);

				// Update light buffer
				if (!spotLights.empty() && (lightUpdate || lightAnimation))
				{
					auto& lightDataAllocation = uploadPool.Allocate(alignedSpotLightSize * spotLights.size());
					Nz::UInt8* lightDataPtr = static_cast<Nz::UInt8*>(lightDataAllocation.mappedPtr);

					for (const SpotLight& spotLight : spotLights)
					{
						float rotationSpeed = ComputeLightAnimationSpeed(spotLight.position);

						Nz::Vector3f position = AnimateLightPosition(spotLight.position, rotationSpeed, elapsedTime);
						Nz::Vector3f direction = AnimateLightDirection(spotLight.direction, rotationSpeed, elapsedTime);

						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, colorOffset) = Nz::Vector3f(spotLight.color.r / 255.f, spotLight.color.g / 255.f, spotLight.color.b / 255.f);
						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, positionOffset) = position;
						Nz::AccessByOffset<Nz::Vector3f&>(lightDataPtr, directionOffset) = direction;
						Nz::AccessByOffset<float&>(lightDataPtr, radiusOffset) = spotLight.radius;
						Nz::AccessByOffset<float&>(lightDataPtr, invRadiusOffset) = 1.f / spotLight.radius;
						Nz::AccessByOffset<float&>(lightDataPtr, innerAngleOffset) = spotLight.innerAngle.GetCos();
						Nz::AccessByOffset<float&>(lightDataPtr, outerAngleOffset) = spotLight.outerAngle.GetCos();

						float baseRadius = spotLight.radius * spotLight.outerAngle.GetTan() * 1.1f;
						Nz::Matrix4f transformMatrix = Nz::Matrix4f::Transform(position, Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), direction), Nz::Vector3f(baseRadius, baseRadius, spotLight.radius));
						Nz::AccessByOffset<Nz::Matrix4f&>(lightDataPtr, transformMatrixOffset) = transformMatrix;

						lightDataPtr += alignedSpotLightSize;
					}

					builder.CopyBuffer(lightDataAllocation, lightUbo.get());
				}

				// Update light scattering buffer
				{
					Nz::Vector4f pos(0.f, 200.f, 0.f, 1.f);
					pos = viewerInstance.GetViewMatrix() * pos;
					pos = viewerInstance.GetProjectionMatrix() * pos;
					pos /= pos.w;

					Nz::Vector2f& lightPosition = Nz::AccessByOffset<Nz::Vector2f&>(godRaysData.data(), gr_lightPositionOffset);
					lightPosition = Nz::Vector2f(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f);
					lightPosition.x = Nz::Clamp(std::abs(lightPosition.x), -0.5f, 1.5f);
					lightPosition.y = Nz::Clamp(std::abs(lightPosition.y), -0.5f, 1.5f);

					auto& lightScatteringAllocation = uploadPool.Allocate(godRaysData.size());
					Nz::UInt8* dataPtr = static_cast<Nz::UInt8*>(lightScatteringAllocation.mappedPtr);

					std::memcpy(dataPtr, godRaysData.data(), godRaysData.size());

					builder.CopyBuffer(lightScatteringAllocation, godRaysUBO.get());
				}

				matUpdate = spaceshipMatPass->Update(frame, builder) || matUpdate;
				matUpdate = planeMatPass->Update(frame, builder) || matUpdate;

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, Nz::QueueType::Transfer);

		bakedGraph.Execute(frame);

		const Nz::RenderTarget* windowRT = window.GetRenderTarget();
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			Nz::Recti windowRenderRect(0, 0, window.GetSize().x, window.GetSize().y);

			builder.TextureBarrier(Nz::PipelineStage::ColorOutput, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ColorWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::ColorOutput, Nz::TextureLayout::ColorInput, *bakedGraph.GetAttachmentTexture(toneMappingOutput));

			builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetFramebufferIndex()), windowRT->GetRenderPass(), windowRenderRect);
			{
				builder.BeginDebugRegion("Main window rendering", Nz::Color::Green);
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.BindShaderBinding(0, *finalBlitBinding);
					builder.BindPipeline(*fullscreenPipeline);
					builder.BindVertexBuffer(0, *fullscreenVertexBuffer);
					builder.Draw(3);
				}
				builder.EndDebugRegion();
			}
			builder.EndRenderPass();
		}, Nz::QueueType::Graphics);

		frame.Present();

		matUpdate = false;
		lightUpdate = false;

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
