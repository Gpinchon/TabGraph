R""(
struct RenderHistory {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    sampler2D color;
    sampler2D emissive;
};

uniform RenderHistory[HISTORYSIZE]	in_renderHistory;
uniform sampler2D			in_CurrentColor;
uniform sampler2D			in_CurrentEmissive;
//uniform uint                FrameNumber;

vec4	LastWorldToView(in vec3 position, in int index)
{
    return in_renderHistory[index].viewMatrix * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position, in int index)
{
    return in_renderHistory[index].projectionMatrix * position;
}

/** Returns the World position and the scene depth in world units */
vec4	LastWorldToScreen(in vec3 position, in int index)
{
    return ClipToScreen(LastViewToClip(LastWorldToView(position, index), index));
}

vec3	GetLastUVz(in vec3 UVz, in int index)
{
    vec3 worldPos = ScreenToWorld(UVz.xy, UVz.z);
    UVz = LastWorldToScreen(worldPos, index).xyz;
    return UVz;
}

vec3	GetLastUVz(in vec2 UV, in float z, in int index)
{
    return GetLastUVz(vec3(UV, z), index);
}


/*vec4[4] GatherTexture(sampler2D tex, vec2 uv) {
    vec4 x = textureGather(tex, uv, 0);
    vec4 y = textureGather(tex, uv, 1);
    vec4 z = textureGather(tex, uv, 2);
    vec4 w = textureGather(tex, uv, 3);
    return vec4[4](
        vec4(x[0], y[0], z[0], w[0]),
        vec4(x[1], y[1], z[1], w[1]),
        vec4(x[2], y[2], z[2], w[2]),
        vec4(x[3], y[3], z[3], w[3])
    );
}*/

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

vec4[16]  GatherTexture16(sampler2D tex, vec2 uv) {
    vec2 texelSize = 1.f / vec2(textureSize(tex, 0));
    int offset = int(InterleavedGradientNoise(uv * textureSize(tex, 0), FrameNumber % 8) * 64);
    return vec4[16](
        texture(tex, uv),
        texture(tex, uv + (disk[(offset + 0) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 1) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 2) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 3) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 4) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 5) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 6) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 7) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 8) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 9) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 10) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 11) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 12) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 13) % 64] * texelSize) * 2),
        texture(tex, uv + (disk[(offset + 14) % 64] * texelSize) * 2)
    );
}

void TemporalAccumulation()
{
    float samplesWeight = 0;
    float thisDepth = texture(Texture.Geometry.Depth, ScreenTexCoord()).r;
    vec4[16] thisColor = GatherTexture16(in_CurrentColor, ScreenTexCoord());
    vec2 lastCoord = GetLastUVz(ScreenTexCoord(), thisDepth, 0).xy;
    vec4 minColor = vec4(10000);
    vec4 maxColor = vec4(-10000);
    for (int j = 0; j < 16; ++j) {
        minColor = min(minColor, thisColor[j]);
        maxColor = max(maxColor, thisColor[j]);
    }
    out_0 = thisColor[0];//texture(in_CurrentColor, ScreenTexCoord(), 0);
    //out_0 = (minColor + maxColor) * 0.5;
    if (thisDepth == 1 || lastCoord.x < 0 || lastCoord.x > 1 || lastCoord.y < 0 || lastCoord.y > 1)
        return;
    for(int i = 0; i < HISTORYSIZE; ++i)
    {
        vec4 sample_0 =
        //smartDeNoise(in_renderHistory[i].color, lastCoord, 7, 3, 0.2);
        texture(in_renderHistory[i].color, lastCoord);
        if (any(greaterThan(sample_0, maxColor)) || any(lessThan(sample_0, minColor)))
            continue;
        //Use rolling average
        float currentWeight = mix(0.9, 0.1, i / float(HISTORYSIZE));
		//float currentWeight = (1 - i / float(HISTORYSIZE)) * 0.9;
		//float currentWeight = 2/float(1 + HISTORYSIZE);
		out_0 = currentWeight * sample_0 + (1 - currentWeight) * out_0;
    }
}

)""
