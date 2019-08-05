R""(
uniform sampler2D   in_Texture_Color;

in vec2             frag_UV;

void main()
{
    ivec2 ActualUV = ivec2(textureSize(in_Texture_Color, 0) * frag_UV);
    ivec2 SampleUV[] = ivec2[4] (
    	min(ActualUV + ivec2(0, 0), textureSize(in_Texture_Color, 0) - 1),
    	min(ActualUV + ivec2(1, 0), textureSize(in_Texture_Color, 0) - 1),
    	min(ActualUV + ivec2(1, 1), textureSize(in_Texture_Color, 0) - 1),
    	min(ActualUV + ivec2(0, 1), textureSize(in_Texture_Color, 0) - 1)
    );
    float Depth0 = texelFetch(in_Texture_Color, SampleUV[0], 0).r;
    float Depth1 = texelFetch(in_Texture_Color, SampleUV[1], 0).r;
    float Depth2 = texelFetch(in_Texture_Color, SampleUV[2], 0).r;
    float Depth3 = texelFetch(in_Texture_Color, SampleUV[3], 0).r;
    gl_FragDepth = max(max(Depth0, Depth1), max(Depth2, Depth3));
}

)""