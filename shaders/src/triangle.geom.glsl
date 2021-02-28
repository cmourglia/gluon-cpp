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

layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

layout (location = 0) flat in uint in_index[];
// layout (location = 1) in vec2 in_position[];

layout (location = 0) flat out uint out_index;
layout (location = 1) out vec2 out_position;
layout (location = 2) flat out uint out_dropShadow;
layout (location = 3) flat out vec2 out_center;
layout (location = 4) flat out vec2 out_size;

void PassThrough(vec2 in_position, vec2 center, vec2 size)
{
    vec2 localSize = ((in_position - center) / size) * 0.5 + 0.5;
    vec2 translation = mix(vec2(-10.0), vec2(10.0), localSize);
    vec2 position = in_position + translation;

    vec4 viewPosition = frameInfos.view * vec4(position, 0, 1);

    gl_Position = frameInfos.proj * viewPosition;
    out_index = in_index[0];
    out_position = viewPosition.xy;
    out_dropShadow = 0;
    out_center = center;
    out_size = size;
    EmitVertex();
}

void DropShadow(vec2 in_position, vec2 center, vec2 size, vec2 offset)
{
    vec2 localSize = ((in_position - center) / size) * 0.5 + 0.5;
    vec2 translation = mix(vec2(-100.0), vec2(100.0), localSize);
    // vec2 translation = localSize;
    vec2 position = in_position + translation + offset;

    vec4 viewPosition = frameInfos.view * vec4(position, -0.1, 1);

    gl_Position = frameInfos.proj * viewPosition;
    out_index = in_index[0];
    out_position = viewPosition.xy;
    out_dropShadow = 1;
    out_center = center + offset;
    out_size = size;
    EmitVertex();
}

void main() {
    mat4 view = frameInfos.view;
    mat4 proj = frameInfos.proj;

    RectangleInfo rectangle = rectangles[uint(in_index[0])];

    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;

    vec2 center = vec2(rectangle.positionX, rectangle.positionY);
    vec2 size = vec2(rectangle.sizeX, rectangle.sizeY) * 0.5;

    float dropShadowScale = rectangle.dropShadowScale;

    if (dropShadowScale != 0)
    {
        vec2 dropShadowSize = size * dropShadowScale;
        vec2 dropShadowOffset = vec2(rectangle.dropShadowOffsetX, rectangle.dropShadowOffsetY);
        DropShadow(p0, center, dropShadowSize, dropShadowOffset);
        DropShadow(p1, center, dropShadowSize, dropShadowOffset);
        DropShadow(p2, center, dropShadowSize, dropShadowOffset);
        EndPrimitive();
    }

    PassThrough(p0, center, size);
    PassThrough(p1, center, size);
    PassThrough(p2, center, size);
    EndPrimitive();
}