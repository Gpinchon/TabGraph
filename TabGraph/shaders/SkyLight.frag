R""(
struct t_Light {
    vec3    Min;
    vec3    Max;
    bool    Infinite;
#ifdef SHADOW
    mat4    Projection;
    sampler2DShadow Shadow;
#endif //SHADOW
};

uniform t_Light Light;

void    Lighting()
{
	const vec3  V = normalize(Camera.Position - WorldPosition());
    const vec3  N = WorldNormal();
    const vec3  R = reflect(-V, N);
    const float NdV = max(0, dot(N, V));
    const float alphaSqrt = sqrt(Alpha());
    vec3        Diffuse = vec3(SampleSH(N));
    float		SpecularFactor = 0;
    float       Attenuation = 1;
    #ifdef SHADOW
        float bias = 0.01 * tan(acos(NdotL));
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