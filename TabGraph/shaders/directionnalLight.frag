R""(
struct t_Light {
    vec3    Min;
    vec3    Max;
    vec3    Direction;
    vec3    Color;
    bool    Infinite;
#ifdef SHADOW
    mat4    Projection;
    sampler2DShadow Shadow;
#endif //SHADOW
};

uniform t_Light     Light;
uniform sampler2D   SpecularLUT;
uniform sampler2D   DefaultBRDFLUT;

vec2 ToUV(in vec3 n)
{       
    vec2 uv;
    
    uv.x = atan(-n.x, n.y);
    uv.x = (uv.x + PI / 2.0) / (PI * 2.0) + PI * (28.670 / 360.0);
    
    uv.y = acos(n.z) / PI;
    
    return uv;
}

void    Lighting()
{
    const vec3  V = normalize(Camera.Position - WorldPosition());
    const vec3  N = WorldNormal();
    const vec3  L = Light.Direction;
    const vec3  R = reflect(-V, N);
    const float NdV = max(0, dot(N, V));
    const float NdL = max(0, dot(N, L));
    const float alphaSqrt = sqrt(Alpha());
    //const vec2    brdf = texture(DefaultBRDFLUT, vec2(NdV, alphaSqrt)).xy;
    //const vec3    fresnel = F0() * brdf.x + brdf.y;
    const vec3  Specular = sampleLod(SpecularLUT, ToUV(R), Alpha()).rgb;// * fresnel;
    const float SpecularFactor = length(Specular);
    vec3        Diffuse = NdL * Light.Color;
    float       Attenuation = 1;
    #ifdef SHADOW
        float bias = 0.01 * tan(acos(NdL));
        Attenuation *= SampleShadowMap(clamp(bias, 0.0f, 0.02f), Light.Projection, Light.Shadow);
        if (Attenuation == 0)
            return ;
        Diffuse *= Attenuation;
    #endif //SHADOW
    if (all(equal(Diffuse, vec3(0)))) {
        return ;
    }
    bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
    bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
    if (!Light.Infinite && (isAboveMax || isUnderMin)) {
        return ;
    }
    out_0 = vec4(Diffuse, SpecularFactor * Attenuation);
}

)""