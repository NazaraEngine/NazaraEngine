#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/SampleTexture.hpp>
#include <NZSL/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<VecData>(4);

	UpdateOutput();
}

unsigned int SampleTexture::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void SampleTexture::UpdateOutput()
{
	PreviewValues& output = m_output->preview;

	if (!m_texture || !m_uv)
	{
		output = PreviewValues(1, 1);
		output.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));
		return;
	}

	const PreviewValues& texturePreview = m_texture->preview;

	std::size_t textureWidth = texturePreview.GetWidth();
	std::size_t textureHeight = texturePreview.GetHeight();

	const PreviewValues& uv = m_uv->preview;

	std::size_t uvWidth = uv.GetWidth();
	std::size_t uvHeight = uv.GetHeight();

	output = PreviewValues(uvWidth, uvHeight);

	for (std::size_t y = 0; y < uvHeight; ++y)
	{
		for (std::size_t x = 0; x < uvWidth; ++x)
		{
			nzsl::Vector4f32 uvValue = uv(x, y);

			if (textureWidth > 0 && textureHeight > 0)
				output(x, y) = texturePreview.Sample(uvValue.x(), uvValue.y());
			else
				output(x, y) = nzsl::Vector4f32(0.f, 0.f, 0.f, 1.f);
		}
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}

bool SampleTexture::ComputePreview(QPixmap& pixmap)
{
	if (!m_texture || !m_uv)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

nzsl::Ast::NodePtr SampleTexture::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(m_texture);
	assert(m_uv);
	assert(count == 2);
	assert(outputIndex == 0);

	std::vector<nzsl::Ast::ExpressionPtr> params;
	params.push_back(std::move(expressions[0]));
	params.push_back(std::move(expressions[1]));

	return nzsl::ShaderBuilder::Intrinsic(nzsl::Ast::IntrinsicType::TextureSampleImplicitLod, std::move(params));
}

auto SampleTexture::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0: return Texture2Data::Type();
				case 1: return VecData::Type();
			}

			assert(false);
			throw std::runtime_error("invalid port index");
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return VecData::Type();
		}

		default:
			assert(false);
			throw std::runtime_error("invalid PortType");
	}
}

QString SampleTexture::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0: return tr("Texture");
				case 1: return tr("UV");
			}

			assert(false);
			throw std::runtime_error("invalid port index");
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return tr("Sample");
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}
}

bool SampleTexture::portCaptionVisible(QtNodes::PortType /*portType*/, QtNodes::PortIndex /*portIndex*/) const
{
	return true;
}

std::shared_ptr<QtNodes::NodeData> SampleTexture::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	return m_output;
}

void SampleTexture::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	switch (index)
	{
		case 0:
		{
			if (value && value->type().id == Texture2Data::Type().id)
			{
				assert(dynamic_cast<Texture2Data*>(value.get()) != nullptr);
				m_texture = std::static_pointer_cast<Texture2Data>(value);
			}
			else
				m_texture.reset();

			break;
		}

		case 1:
		{
			if (value && value->type().id == VecData::Type().id)
			{
				assert(dynamic_cast<VecData*>(value.get()) != nullptr);
				m_uv = std::static_pointer_cast<VecData>(value);
			}
			else
				m_uv.reset();

			break;
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}

	UpdateOutput();
}

QtNodes::NodeValidationState SampleTexture::validationState() const
{
	if (!m_texture || !m_uv)
		return QtNodes::NodeValidationState::Error;

	if (m_uv->componentCount != 2)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString SampleTexture::validationMessage() const
{
	if (!m_texture)
		return "Missing texture";

	if (!m_uv)
		return "Missing uv";

	if (m_uv->componentCount != 2)
		return "Incompatible UV (expected 2, got " + QString::number(m_uv->componentCount) + ")";

	return QString();
}
