#pragma once

#include <Gluon/Widgets/widget.h>

struct Image : public Widget
{
    enum class FitMode
    {
        Stretch,
        Fit,
        Crop,
    };

    static Widget* create()
    {
        return new Image{};
    }

    virtual ~Image();

    virtual void PostEvaluate() override;

    std::string image_url;
    glm::vec2   image_size = {0.0f, 0.0f};

    FitMode fit_mode = FitMode::Stretch;

    glm::vec4 image_tint = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
    ImageInfo image_info = {};

private:
    virtual void ParserPropertyInternal(parser::Node::Ptr node, const u32 node_hash) override;
    virtual void BuildRenderInfosInternal(beard::array<RectangleInfo>* result) override;
};
