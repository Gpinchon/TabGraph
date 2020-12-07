R""(
layout(location = 0) out vec4	out_Buffer0;
layout(location = 1) out vec4	out_Buffer1;

uniform sampler2D	in_Buffer0;
uniform sampler2D	in_Buffer1;
uniform sampler2D	in_Buffer2;

in vec2	frag_UV;

float maxComponent(const in vec4 v) {
    return max(max(v.x, v.y), max(v.z, v.w));
}

void Composite()
{
    /*
    ivec2 C = ivec2(gl_FragCoord.xy);
    float  revealage = texelFetch(in_Buffer1, C, 0).r;
    if (revealage == 1.0) {
        // Save the blending and color texture fetch cost
        discard; 
    }
    vec4 accum     = texelFetch(in_Buffer0, C, 0);
    // Suppress overflow
    if (isinf(maxComponent(abs(accum)))) {
        accum.rgb = vec3(accum.a);
    }
    vec3 averageColor = accum.rgb / max(accum.a, 0.00001);
    // dst' =  (accum.rgb / accum.a) * (1 - revealage) + dst * revealage
    out_Buffer0 = vec4(averageColor, 1.0 - revealage);
    */
    vec4 accum = texelFetch(in_Buffer0, ivec2(gl_FragCoord.xy), 0);
    float r = texelFetch(in_Buffer1, ivec2(gl_FragCoord.xy), 0).r;
    out_Buffer0 = vec4(accum.rgb / clamp(accum.a, 6.1*1e-4, 6.55*1e5), r);
    out_Buffer1 = vec4(texelFetch(in_Buffer2, ivec2(gl_FragCoord.xy), 0).rgb, out_Buffer0.a);
}
)""