#pragma once

#include "parser.h"
#include "shuntingyard.h"

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <string>

struct MuWidget
{
	using Ptr = std::shared_ptr<MuWidget>;

	virtual ~MuWidget() = default;

	virtual void Deserialize(Parser::Node::Ptr node) final;
	virtual void BuildRenderInfos(std::vector<RectangleInfo>* result) final;

	virtual void Evaluate() final;
	virtual void Touch() final;

	virtual void PostEvaluate() { }

	std::vector<MuWidget::Ptr> children;
	MuWidget*                  parent = nullptr;

	std::string id = "";

	glm::vec2 pos  = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);

	std::unordered_set<std::string>             dependencyIds;
	std::unordered_map<u32, std::vector<Token>> geometryExpressions;

	std::vector<MuWidget*> dependencies;
	std::vector<MuWidget*> dependants;
	bool                   dirty = true;

	std::vector<std::pair<f32*, ShuntingYard::Expression>> evaluators;

	static std::vector<MuWidget::Ptr> widgetMap;

private:
	virtual void ParseProperty(Parser::Node::Ptr node) final;
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) = 0;

	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) = 0;
};

typedef std::shared_ptr<MuWidget> WidgetFactory(void);

struct MuWindow : public MuWidget
{
	static MuWidget::Ptr Create()
	{
		return std::make_shared<MuWindow>();
	}

	std::string title           = "Gluon";
	glm::vec4   backgroundColor = MuColor::Chocolate;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};

struct MuRectangle : public MuWidget
{
	static MuWidget::Ptr Create()
	{
		return std::make_shared<MuRectangle>();
	}

	glm::vec4 fillColor;
	glm::vec4 borderColor;

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};

struct NSVGimage;
struct MuImage : public MuWidget
{
	enum class FitMode
	{
		Stretch,
		Fit,
		Crop,
	};

	static MuWidget::Ptr Create()
	{
		return std::make_shared<MuImage>();
	}

	virtual ~MuImage();

	virtual void PostEvaluate() override;

	std::string imageUrl;

	FitMode fitMode = FitMode::Stretch;

	glm::vec4 imageTint = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	ImageInfo imageInfo = {};

private:
	virtual void ParsePropertyInternal(Parser::Node::Ptr node, const u32 nodeHash) override;
	virtual void BuildRenderInfosInternal(std::vector<RectangleInfo>* result) override;
};

MuWidget* GetWidgetById(MuWidget* rootWidget, const std::string& name);
void      BuildDependencyGraph(MuWidget* rootWidget, MuWidget* currentWidget);
void      BuildExpressionEvaluators(MuWidget* rootWidget, MuWidget* currentWidget);
