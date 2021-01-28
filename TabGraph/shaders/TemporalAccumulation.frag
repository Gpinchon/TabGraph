R""(
struct RenderHistory {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    sampler2D color;
    sampler2D emissive;
};

uniform RenderHistory	    in_renderHistory;
uniform sampler2D			in_CurrentColor;
uniform sampler2D			in_CurrentEmissive;
uniform sampler2D           in_CurrentVelocity;

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
    // magic values are found by experimentation
    uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

#define GAUSSIANKERNALSIZE 9

const float gaussianKernel[9] = float[9](
    0.039436,	0.119713,	0.039436,
    0.119713,	0.363404,	0.119713,
    0.039436,	0.119713,	0.039436
);

const ivec2 gaussianOffset[9] = ivec2[9](
    ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),
    ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),
    ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1)
);

const ivec2 minMaxOffset[13] = ivec2[13](
                                    ivec2(0, -2),
                    ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),
    ivec2(-2,  0),	ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),	ivec2(2,  0),
                    ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1),
                                    ivec2(0,  2)
);

uniform int in_Stepwidth = 1;

void TemporalAccumulation()
{
    out_0 = texelFetch(in_CurrentColor, ivec2(ScreenTexCoord() * textureSize(in_CurrentColor, 0)), 0);
    if (texelFetch(Texture.Geometry.Depth, ivec2(ScreenTexCoord() * textureSize(Texture.Geometry.Depth, 0)), 0).r == 1)
        return;
    vec2 velocity = texelFetch(in_CurrentVelocity, ivec2(ScreenTexCoord() * textureSize(in_CurrentVelocity, 0)), 0).xy;
    velocity = vec2(
        abs(velocity.x) > 0.00025 ? velocity.x : 0,
        abs(velocity.y) > 0.00025 ? velocity.y : 0
    );
    float minLum = 1;
    float maxLum = 0;
	for (int i = 0; i < 13; ++i) {
        ivec2 uv = ivec2(ScreenTexCoord() * textureSize(in_CurrentColor, 0)) + minMaxOffset[i];
        float lum = Luminance(texelFetch(in_CurrentColor, uv, 0).rgb);
        minLum = min(lum, minLum);
        maxLum = max(lum, maxLum);
	}
    float samplesWeight = 0;
    vec4 colorSamples = vec4(0);
    for (int i = 0; i < GAUSSIANKERNALSIZE; ++i) {
        vec2 historyUV = ScreenTexCoord() + vec2(gaussianOffset[i] * in_Stepwidth) / textureSize(in_renderHistory.color, 0);
        vec4 historyColor = textureLod(in_renderHistory.color, historyUV + velocity, 0);
		float colorWeight = clamp(dot(historyColor, out_0), 0, 1);
        float totalWeight = gaussianKernel[i] * colorWeight;
        colorSamples += historyColor * totalWeight;
        samplesWeight += totalWeight;
    }
    if (samplesWeight == 0)
        return;
    vec4 historyColor = colorSamples / samplesWeight;
    float lum = Luminance(historyColor.rgb);
    if (lum > maxLum || lum < minLum)
			return;
	float alpha = clamp(samplesWeight, 0.f, 0.9f);
	out_0 = historyColor * alpha + (1 - alpha) * out_0;
}

)""
