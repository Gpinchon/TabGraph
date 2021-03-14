R""(
vec2 Hammersley(uint Index, uint NumSamples, uvec2 Random)
{
    float E1 = fract( Index / float(NumSamples) + float( Random.x & 0xffff ) / (1<<16) );
    float E2 = float( bitfieldReverse(Index) ^ Random.y ) * 2.3283064365386963e-10;
    return vec2( E1, E2 );
}

uvec3 Rand3DPCG16(ivec3 p)
{
    uvec3 v = uvec3(p);
    v = v * 1664525u + 1013904223u;
    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;
    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;
    return v >> 16u;
}
)""