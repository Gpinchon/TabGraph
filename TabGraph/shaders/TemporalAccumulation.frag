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

#define SAMPLEKERNELSIZE 9

const ivec2 samplesOffset[SAMPLEKERNELSIZE] = ivec2[SAMPLEKERNELSIZE](
    ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),
    ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),
    ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1)
);

/**
 * @ref http://s3.amazonaws.com/arena-attachments/655504/c5c71c5507f0f8bf344252958254fb7d.pdf?1468341463
 */
vec3 clip_aabb(vec3 aabb_min, vec3 aabb_max, vec3 p, vec3 q)
{
	vec3 p_clip = 0.5 * (aabb_max + aabb_min);
	vec3 e_clip = 0.5 * (aabb_max - aabb_min);
	vec3 v_clip = q - p_clip;
	vec3 v_unit = v_clip.xyz / e_clip;
	vec3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));
	if (ma_unit > 1.0)
		return vec3(p_clip) + v_clip / ma_unit;
	else
		return q;// point inside aabb
}

uniform float gamma = 1;

/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void TemporalAccumulation()
{
    out_0 = vec4(0);
    const vec2 velocity = texelFetch(in_CurrentVelocity, ivec2(ScreenTexCoord() * textureSize(in_CurrentVelocity, 0)), 0).xy;
    vec3 m1 = vec3(0);
    vec3 m2 = vec3(0);
    vec3 realMinColor = vec3(1);
    vec3 realMaxColor = vec3(0);
	for (int i = 0; i < SAMPLEKERNELSIZE; ++i) {
        ivec2 uv = ivec2(ScreenTexCoord() * textureSize(in_CurrentColor, 0)) + samplesOffset[i];
        vec4 color = texelFetch(in_CurrentColor, uv, 0);
        //vec2 uv = ScreenTexCoord() + vec2(minMaxOffset[i] / textureSize(in_CurrentColor, 0));
        //vec4 color = textureLod(in_CurrentColor, uv, 0);
        if (i == 4)
            out_0 = color;
        m1 += color.rgb;
        m2 += color.rgb * color.rgb;
        realMinColor = min(realMinColor, color.rgb);
        realMaxColor = max(realMaxColor, color.rgb);
	}
    vec3 mu = m1 / SAMPLEKERNELSIZE;
    vec3 sigma = sqrt(m2 / SAMPLEKERNELSIZE - mu * mu);
    vec3 minColor = mu - gamma * sigma;
    vec3 maxColor = mu + gamma * sigma;
    minColor = clamp(minColor, realMinColor, realMaxColor);
    maxColor = clamp(maxColor, realMinColor, realMaxColor);
    vec2 uv = ScreenTexCoord() + velocity;
	vec4 historyColor = textureLod(in_renderHistory.color, uv, 0);
    //historyColor.rgb = clamp(historyColor.rgb, minColor, maxColor);
    historyColor.rgb = clip_aabb(minColor, maxColor, out_0.rgb, historyColor.rgb);
    float alpha = 0.9;
    //Use rolling average over time
    out_0 = (1 - alpha) * out_0 + alpha * historyColor;
}

)""
