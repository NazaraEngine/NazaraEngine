#include <ShaderNode/Widgets/CodeOutputWidget.hpp>
#include <NZSL/GlslWriter.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Ast/TransformerExecutor.hpp>
#include <NZSL/Ast/Transformations/ConstantPropagationTransformer.hpp>
#include <NZSL/Ast/Transformations/EliminateUnusedTransformer.hpp>
#include <NZSL/Ast/Transformations/ResolveTransformer.hpp>
#include <NZSL/Ast/Option.hpp>
#include <NZSL/SpirvWriter.hpp>
#include <NZSL/SpirV/SpirvPrinter.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

enum class OutputLanguage
{
	GLSL,
	NZSL,
	SpirV
};

CodeOutputWidget::CodeOutputWidget(const ShaderGraph& shaderGraph) :
m_shaderGraph(shaderGraph)
{
	m_textOutput = new QTextEdit;

	m_outputLang = new QComboBox;
	m_outputLang->addItem("GLSL", int(OutputLanguage::GLSL));
	m_outputLang->addItem("NZSL", int(OutputLanguage::NZSL));
	m_outputLang->addItem("SPIR-V", int(OutputLanguage::SpirV));
	connect(m_outputLang, qOverload<int>(&QComboBox::currentIndexChanged), [this](int)
	{
		Refresh();
	});

	m_optimisationCheckbox = new QCheckBox;
	m_optimisationCheckbox->setText("Enable optimization");
	connect(m_optimisationCheckbox, &QCheckBox::stateChanged, [this](int)
	{
		Refresh();
	});

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addWidget(m_textOutput);
	verticalLayout->addWidget(m_outputLang);
	verticalLayout->addWidget(m_optimisationCheckbox);

	setLayout(verticalLayout);

	Refresh();
}

void CodeOutputWidget::Refresh()
{
	try
	{
		nzsl::BackendParameters backendParameters;

		for (std::size_t i = 0; i < m_shaderGraph.GetOptionCount(); ++i)
		{
			const auto& option = m_shaderGraph.GetOption(i);
			backendParameters.optionValues[nzsl::Ast::HashOption(option.name)] = m_shaderGraph.IsOptionEnabled(i);
		}

		nzsl::Ast::ModulePtr shaderModule = m_shaderGraph.ToModule();

		if (m_optimisationCheckbox->isChecked())
		{
			nzsl::Ast::TransformerContext context;
			context.optionValues = backendParameters.optionValues;

			nzsl::Ast::TransformerExecutor executor;
			executor.AddPass<nzsl::Ast::ResolveTransformer>();
			executor.AddPass<nzsl::Ast::ConstantPropagationTransformer>();

			executor.Transform(*shaderModule);

			nzsl::Ast::EliminateUnusedPass(*shaderModule);
		}

		std::string codeOutput;
		OutputLanguage outputLang = static_cast<OutputLanguage>(m_outputLang->currentIndex());
		switch (outputLang)
		{
			case OutputLanguage::GLSL:
			{
				nzsl::GlslWriter::Parameters shaderParameters;
				for (const auto& buffer : m_shaderGraph.GetBuffers())
					shaderParameters.bindingMapping.emplace(Nz::UInt64(buffer.setIndex) << 32 | Nz::UInt64(buffer.bindingIndex), shaderParameters.bindingMapping.size());

				for (const auto& texture : m_shaderGraph.GetTextures())
					shaderParameters.bindingMapping.emplace(Nz::UInt64(texture.setIndex) << 32 | Nz::UInt64(texture.bindingIndex), shaderParameters.bindingMapping.size());

				nzsl::GlslWriter writer;
				nzsl::GlslWriter::Output output = writer.Generate(ShaderGraph::ToShaderStageType(m_shaderGraph.GetType()), *shaderModule, backendParameters, shaderParameters);
				codeOutput = std::move(output.code);
				break;
			}

			case OutputLanguage::NZSL:
			{
				nzsl::LangWriter writer;
				codeOutput = writer.Generate(*shaderModule);
				break;
			}

			case OutputLanguage::SpirV:
			{
				nzsl::SpirvWriter writer;
				std::vector<std::uint32_t> spirv = writer.Generate(*shaderModule, backendParameters);

				nzsl::SpirvPrinter printer;
				codeOutput = printer.Print(spirv.data(), spirv.size());
				break;
			}
		}

		int scrollValue = m_textOutput->verticalScrollBar()->value();
		m_textOutput->setText(QString::fromStdString(codeOutput));
		m_textOutput->verticalScrollBar()->setValue(scrollValue);
	}
	catch (const std::exception& e)
	{
		m_textOutput->setText("Generation failed: " + QString::fromStdString(e.what()));
	}
}
