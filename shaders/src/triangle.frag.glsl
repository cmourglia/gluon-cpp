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

// layout (location = 0) flat in uint in_InstanceID;
layout (location = 0) flat in uint in_index;
layout (location = 1) in vec2 in_position;
layout (location = 2) flat in uint in_dropShadow;
layout (location = 3) flat in vec2 in_center;
layout (location = 4) flat in vec2 in_size;

layout (location = 0) out vec4 out_color;

float GetRectangleAlpha(vec2 pos, vec2 center, vec2 size, float radius)
{
    vec2 Q = abs(pos - center) - size + radius;
    return (min(max(Q.x, Q.y), 0.0) + length(max(Q, 0.0)) - radius);
}

// http://madebyevan.com/shaders/fast-rounded-rectangle-shadows/
// A standard Gaussian function, used for weighting samples
float Gaussian(float x, float sigma) {
  const float pi = 3.141592653589793;
  return exp(-(x * x) / (2.0 * sigma * sigma)) / (sqrt(2.0 * pi) * sigma);
}

// This approximates the error function, needed for the Gaussian integral
vec2 Erf(vec2 x) {
  vec2 s = sign(x), a = abs(x);
  x = 1.0 + (0.278393 + (0.230389 + 0.078108 * (a * a)) * a) * a;
  x *= x;
  return s - s / (x * x);
}

// Return the blurred mask along the x dimension
float RoundedBoxShadowX(float x, float y, float sigma, float corner, vec2 halfSize) {
  float delta = min(halfSize.y - corner - abs(y), 0.0);
  float curved = halfSize.x - corner + sqrt(max(0.0, corner * corner - delta * delta));
  vec2 integral = 0.5 + 0.5 * Erf((x + vec2(-curved, curved)) * (sqrt(0.5) / sigma));
  return integral.y - integral.x;
}

// Return the mask for the shadow of a box from lower to upper
float RoundedBoxShadow(vec2 lower, vec2 upper, vec2 point, float sigma, float corner) {
  // Center everything to make the math easier
  vec2 center = (lower + upper) * 0.5;
  vec2 halfSize = (upper - lower) * 0.5;
  point -= center;

  // The signal is only non-zero in a limited range, so don't waste samples
  float low = point.y - halfSize.y;
  float high = point.y + halfSize.y;
  float start = clamp(-3.0 * sigma, low, high);
  float end = clamp(3.0 * sigma, low, high);

  // Accumulate samples (we can get away with surprisingly few samples)
  float step = (end - start) / 4.0;
  float y = start + step * 0.5;
  float value = 0.0;
  for (int i = 0; i < 4; i++) {
    value += RoundedBoxShadowX(point.x, point.y - y, sigma, corner, halfSize) * Gaussian(y, sigma) * step;
    y += step;
  }

  return value;
}


void main() {
    RectangleInfo rectangle = rectangles[in_index];

    vec2 pos = in_position;
    vec2 center = in_center;
    vec2 size = in_size;

    float radius = clamp(rectangle.radius, 0, min(size.x, size.y));

    // vec2 position = in_position;
    // vec2 center = vec2(frameInfos.view * vec4(rectangle.positionX, rectangle.positionY, 0, 1));
    // vec2 size = vec2(rectangle.sizeX, rectangle.sizeY);
    // float radius = rectangle.radius;

    if (in_dropShadow > 0)
    {
        // float alpha = RoundedBoxShadow(center - size, center + size, pos, rectangle.dropShadowStrength, radius);
        float alpha = RoundedBoxShadow(center - size, center + size, pos, 75, radius);
        out_color = vec4(rectangle.dropShadowColorR, rectangle.dropShadowColorG, rectangle.dropShadowColorB, rectangle.dropShadowColorA * alpha);
    }
    else
    {
        vec3 fillColor = vec3(rectangle.fillColorR, rectangle.fillColorG, rectangle.fillColorB);
        float fillColorAlpha = rectangle.fillColorA;
        vec3 borderColor = vec3(rectangle.borderColorR, rectangle.borderColorG, rectangle.borderColorB);
        float borderColorAlpha = rectangle.borderColorA;
        float borderWidth = rectangle.borderWidth;

        float borderAA = 1;
        float alpha =  GetRectangleAlpha(pos, center, size, radius);
        float alphaBorder = GetRectangleAlpha(pos, center, size + borderWidth, radius == 0 ? 0 : radius + borderWidth);

        out_color.a = 1.0;

        vec3 color;

        if (alpha < 0) {
            color = fillColor;
            out_color.a *= fillColorAlpha;
        } else if (alpha < borderAA) {
            if (borderWidth == 0) {
                out_color.a = 1 - (alpha / borderAA);
                out_color.a *= fillColorAlpha;
                color = fillColor;
            } else {
                color = mix(fillColor, borderColor, smoothstep(0.0, 1.0, alpha / borderAA));
                out_color.a *= fillColorAlpha;
            }
        } else if (alphaBorder < 0) {
            // float a = smoothstep(0.0, 1250.0, alpha * size.x);
            // color = mix(vec3(1, 0, 0), vec3(0, 1, 0), a);
            color = borderColor;
            out_color.a *= borderColorAlpha;
        } else if (alphaBorder < borderAA) {
            color = borderColor;
            out_color.a = 1 - (alphaBorder / borderAA);
            out_color.a *= borderColorAlpha;
        } else {
            discard;
        }

        // out_color.rgb = clamp(vec3(alpha), 0, 1);
        // out_color.rgb = vec3(alpha * 0.5 + 10000000);
        // out_color.a = alpha / 100000;
        // alpha /= 100;
        // out_color.rgb = alpha < 0 ? vec3(alpha * -1) : vec3(alpha);
        // out_color.rgb = vec3(alpha);
        // out_color.a = 1;

        out_color.rgb = color;
    }
}