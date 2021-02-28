#version 450

struct RectangleInfo
{
    float positionX, positionY;
    float sizeX, sizeY;
    float fillColorR, fillColorG, fillColorB, fillColorA;
    float radius;
    float borderColorR, borderColorG, borderColorB, borderColorA;
    float borderWidth;
    float dropShadowOffsetX, dropShadowOffsetY;
    float dropShadowColorR, dropShadowColorG, dropShadowColorB, dropShadowColorA;
    float dropShadowScale, dropShadowStrength;
};

layout (binding = 0) readonly buffer rectangleData
{
    RectangleInfo rectangles[];
};

layout (binding = 1) uniform FrameInfos
{
    mat4 view;
    mat4 proj;
    vec2 viewport;
} frameInfos;

layout (location = 0) in vec2 in_position;
layout (location = 1) in float in_index;

// layout (location = 0) flat out uint out_instanceID;
layout (location = 0) flat out uint out_index;

void main() {
    // gl_Position = frameInfos.proj * frameInfos.view * vec4(position, 0, 1);

    // out_position = vec2(frameInfos.view * vec4(position, 0, 1));
    // out_instanceID = gl_InstanceIndex;

    // gl_Position = frameInfos.proj * frameInfos.view * vec4(position, 0, 1);
    gl_Position = vec4(in_position, 0, 1);
    // out_position = vec2(frameInfos.view * vec4(position, 0, 1));
    out_index = uint(in_index);
}