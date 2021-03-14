R""(
#define SHSAMPLES 16
uniform vec3 SphericalHarmonics[SHSAMPLES] = vec3[16](0);
uniform vec3 SHCartesianShapes[SHSAMPLES] = vec3[16](vec3(0));

vec3   SampleSH(in vec3 N)
{
    vec3 v = vec3(0);
    for (int i = 0; i < SHSAMPLES; ++i)
        v += dot(N, SHCartesianShapes[i]) * SphericalHarmonics[i];
    return max(vec3(0), v);
}
)""