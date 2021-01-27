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

vec2 disk[64] = vec2[64](
    vec2(-0.613392, 0.617481),
    vec2(0.170019, -0.040254),
    vec2(-0.299417, 0.791925),
    vec2(0.645680, 0.493210),
    vec2(-0.651784, 0.717887),
    vec2(0.421003, 0.027070),
    vec2(-0.817194, -0.271096),
    vec2(-0.705374, -0.668203),
    vec2(0.977050, -0.108615),
    vec2(0.063326, 0.142369),
    vec2(0.203528, 0.214331),
    vec2(-0.667531, 0.326090),
    vec2(-0.098422, -0.295755),
    vec2(-0.885922, 0.215369),
    vec2(0.566637, 0.605213),
    vec2(0.039766, -0.396100),
    vec2(0.751946, 0.453352),
    vec2(0.078707, -0.715323),
    vec2(-0.075838, -0.529344),
    vec2(0.724479, -0.580798),
    vec2(0.222999, -0.215125),
    vec2(-0.467574, -0.405438),
    vec2(-0.248268, -0.814753),
    vec2(0.354411, -0.887570),
    vec2(0.175817, 0.382366),
    vec2(0.487472, -0.063082),
    vec2(-0.084078, 0.898312),
    vec2(0.488876, -0.783441),
    vec2(0.470016, 0.217933),
    vec2(-0.696890, -0.549791),
    vec2(-0.149693, 0.605762),
    vec2(0.034211, 0.979980),
    vec2(0.503098, -0.308878),
    vec2(-0.016205, -0.872921),
    vec2(0.385784, -0.393902),
    vec2(-0.146886, -0.859249),
    vec2(0.643361, 0.164098),
    vec2(0.634388, -0.049471),
    vec2(-0.688894, 0.007843),
    vec2(0.464034, -0.188818),
    vec2(-0.440840, 0.137486),
    vec2(0.364483, 0.511704),
    vec2(0.034028, 0.325968),
    vec2(0.099094, -0.308023),
    vec2(0.693960, -0.366253),
    vec2(0.678884, -0.204688),
    vec2(0.001801, 0.780328),
    vec2(0.145177, -0.898984),
    vec2(0.062655, -0.611866),
    vec2(0.315226, -0.604297),
    vec2(-0.780145, 0.486251),
    vec2(-0.371868, 0.882138),
    vec2(0.200476, 0.494430),
    vec2(-0.494552, -0.711051),
    vec2(0.612476, 0.705252),
    vec2(-0.578845, -0.768792),
    vec2(-0.772454, -0.090976),
    vec2(0.504440, 0.372295),
    vec2(0.155736, 0.065157),
    vec2(0.391522, 0.849605),
    vec2(-0.620106, -0.328104),
    vec2(0.789239, -0.419965),
    vec2(-0.545396, 0.538133),
    vec2(-0.178564, -0.596057)
);

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
    // magic values are found by experimentation
    uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

#define SAMPLERADIUS 2.5f
#define SAMPLENBR 16
#define GatherTexture GatherTexture16

vec4[16]  GatherTexture16(sampler2D tex, vec2 uv) {
    vec2 texelSize = 1.f / vec2(textureSize(tex, 0));
    int offset = int(InterleavedGradientNoise(uv * textureSize(tex, 0), FrameNumber % 8) * 64);
    return vec4[16](
        texture(tex, uv),
        texture(tex, uv + (disk[(offset + 0) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 1) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 2) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 3) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 4) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 5) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 6) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 7) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 8) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 9) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 10) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 11) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 12) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 13) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 14) % 64] * texelSize) * SAMPLERADIUS)
    );
}

vec4[32]  GatherTexture32(sampler2D tex, vec2 uv) {
    vec2 texelSize = 1.f / vec2(textureSize(tex, 0));
    int offset = int(InterleavedGradientNoise(uv * textureSize(tex, 0), FrameNumber % 8) * 64);
    return vec4[32](
        texture(tex, uv),
        texture(tex, uv + (disk[(offset + 0) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 1) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 2) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 3) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 4) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 5) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 6) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 7) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 8) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 9) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 10) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 11) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 12) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 13) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 14) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 15) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 16) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 17) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 18) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 19) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 20) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 21) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 22) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 23) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 24) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 25) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 26) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 27) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 28) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 29) % 64] * texelSize) * SAMPLERADIUS),
        texture(tex, uv + (disk[(offset + 30) % 64] * texelSize) * SAMPLERADIUS)
    );
}

#define GAUSSIANKERNALSIZE 25

float gaussianKernel[25] = float[25](
    0.003765,	0.015019,	0.023792,	0.015019,	0.003765,
    0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
    0.023792,	0.094907,	0.150342,	0.094907,	0.023792,
    0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
    0.003765,	0.015019,	0.023792,	0.015019,	0.003765
);

ivec2 gaussianOffset[25] = ivec2[25](
    ivec2(-2, -2),	ivec2(-1, -2),	ivec2(0, -2),	ivec2(1, -2),	ivec2(2, -2),
    ivec2(-2, -1),	ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),	ivec2(2, -1),
    ivec2(-2,  0),	ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),	ivec2(2,  0),
    ivec2(-2,  1),	ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1),	ivec2(2,  1),
    ivec2(-2,  2),	ivec2(-1,  2),	ivec2(0,  2),	ivec2(1,  2),	ivec2(2,  2)
);

//ivec2 gaussianOffset[25] = ivec2[25](
//    ivec2( 0,  0),	ivec2( 0,  0),	ivec2(0, -2),	ivec2(0,  0),	ivec2(0,  0),
//    ivec2( 0,  0),	ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),	ivec2(0,  0),
//    ivec2(-2,  0),	ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),	ivec2(2,  0),
//    ivec2( 0,  0),	ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1),	ivec2(0,  0),
//    ivec2( 0,  0),	ivec2( 0,  0),	ivec2(0,  2),	ivec2(0,  0),	ivec2(0,  0)
//);

uniform float color_phi = 1.f;
uniform float depth_phi = 1.f;
uniform int in_Stepwidth = 1;

void TemporalAccumulation()
{
    vec2 velocity = texelFetch(in_CurrentVelocity, ivec2(ScreenTexCoord() * textureSize(in_CurrentVelocity, 0)), 0).xy;
    const float depth = texelFetch(Texture.Geometry.Depth, ivec2(ScreenTexCoord() * textureSize(Texture.Geometry.Depth, 0)), 0).r;
    out_0 = texelFetch(in_CurrentColor, ivec2(ScreenTexCoord() * textureSize(in_CurrentColor, 0)), 0);
    if (depth == 1)
        return;
    velocity = any(greaterThan(abs(velocity), vec2(0.00025))) ? velocity : vec2(0);
    float samplesWeight = 0;
    vec4 colorSamples = vec4(0);
    vec4 minColor = vec4(1);
    vec4 maxColor = vec4(0);
    for (int i = 0; i < GAUSSIANKERNALSIZE; ++i) {
        ivec2 uv = ivec2((ScreenTexCoord()) * textureSize(in_CurrentColor, 0)) + gaussianOffset[i];
        vec4 color = texelFetch(in_CurrentColor, uv, 0);
        minColor = min(color, minColor);
        maxColor = max(color, maxColor);
        vec2 historyUV = ScreenTexCoord() + gaussianOffset[i] / textureSize(in_renderHistory.color, 0);
        vec4 historyColor = texture(in_renderHistory.color, historyUV + velocity);
        float colorWeight = clamp(dot(historyColor, out_0), 0, 1);
        float totalWeight = gaussianKernel[i] * colorWeight;
        colorSamples += historyColor * totalWeight;
        samplesWeight += totalWeight;
    }
    vec4 historyColor = samplesWeight > 0 ? colorSamples / samplesWeight : vec4(0);
    if (any(lessThan(historyColor, minColor)) || any(greaterThan(historyColor, maxColor)))
		return;
	float alpha = min(0.9, samplesWeight);
	out_0 = historyColor * alpha + (1 - alpha) * out_0;
}

)""
