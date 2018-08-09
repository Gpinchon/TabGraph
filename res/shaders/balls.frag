#version 410
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 5

layout(location = 0) out lowp vec4	out_Color;
layout(location = 1) out lowp vec4	out_Bright;
layout(location = 2) out lowp vec4	out_Normal;
layout(location = 3) out vec4		out_Position;

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Bright;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform samplerCube	Environment.Diffuse;
uniform samplerCube	Environment.Irradiance;
uniform vec3		in_Resolution;
uniform float		in_Time;

in vec2				frag_UV;
in vec3				frag_Cube_UV;
in float			frag_CenterDepth;
in vec3				frag_WorldPosition;
in lowp vec3		frag_WorldNormal;

//out vec4			out_Color;

// More spheres. Created by Reinder Nijhoff 2013
// @reindernijhoff
//
// https://www.shadertoy.com/view/lsX3DH
//
// based on: http://www.iquilezles.org/www/articles/simplepathtracing/simplepathtracing.htm
//

#define MOTIONBLUR
#define DEPTHOFFIELD

#define CUBEMAPSIZE 256

#define SAMPLES 8
#define PATHDEPTH 4
#define TARGETFPS 60.

#define FOCUSDISTANCE 17.
#define FOCUSBLUR 0.25

#define RAYCASTSTEPS 20
#define RAYCASTSTEPSRECURSIVE 2

#define EPSILON 0.001
#define MAXDISTANCE 180.
#define GRIDSIZE 8.
#define GRIDSIZESMALL 5.9
#define MAXHEIGHT 10.
#define SPEED 0.5

float time;

//
// math functions
//

float hash( const float n ) {
	return fract(sin(n)*43758.54554213);
}
vec2 hash2( const float n ) {
	return fract(sin(vec2(n,n+1.))*vec2(43758.5453123));
}
vec2 hash2( const vec2 n ) {
	return fract(sin(vec2( n.x*n.y, n.x+n.y))*vec2(25.1459123,312.3490423));
}
vec3 hash3( const vec2 n ) {
	return fract(sin(vec3(n.x, n.y, n+2.0))*vec3(36.5453123,43.1459123,11234.3490423));
}
//
// intersection functions
//

float intersectPlane( const vec3 ro, const vec3 rd, const float height) {	
	if (rd.y==0.0) return 500.;	
	float d = -(ro.y - height)/rd.y;
	if( d > 0. ) {
		return d;
	}
	return 500.;
}

float intersectUnitSphere ( const vec3 ro, const vec3 rd, const vec3 sph ) {
	vec3  ds = ro - sph;
	float bs = dot( rd, ds );
	float cs = dot( ds, ds ) - 1.0;
	float ts = bs*bs - cs;

	if( ts > 0.0 ) {
		ts = -bs - sqrt( ts );
		if( ts > 0. ) {
			return ts;
		}
	}
	return 500.;
}

//
// Scene
//

void getSphereOffset( const vec2 grid, out vec2 center ) {
	center = (hash2( grid+vec2(43.12,1.23) ) - vec2(0.5) )*(GRIDSIZESMALL);
}
void getMovingSpherePosition( const vec2 grid, const vec2 sphereOffset, out vec3 center ) {
	// falling?
	float s = 0.1+hash( grid.x*1.23114+5.342+74.324231*grid.y );
	float t = 14.*s + time/s;
	
	float y =  s * MAXHEIGHT * abs( cos( t ) );
	vec2 offset = grid + sphereOffset;
	
	center = vec3( offset.x, y, offset.y ) + 0.5*vec3( GRIDSIZE, 2., GRIDSIZE );
}
void getSpherePosition( const vec2 grid, const vec2 sphereOffset, out vec3 center ) {
	vec2 offset = grid + sphereOffset;
	center = vec3( offset.x, 0., offset.y ) + 0.5*vec3( GRIDSIZE, 2., GRIDSIZE );
}
vec3 getSphereColor( const vec2 grid ) {
	return 0.8*normalize( hash3( grid+vec2(43.12*grid.y,12.23*grid.x) ) );
}

vec3 sundir = normalize( vec3(-1.0,0.8,0.2) );

vec3 getBackgroundColor( const vec3 ro, const vec3 rd ) {	
	return vec3( 0.8, 0.9, 1.0 ) * (1.8 * (rd.y+0.5) );
}

// code duplication because the for-loop requires a const
vec3 traceRec( const vec3 ro, const vec3 rd, out vec3 intersection, out vec3 normal, out float dist, out int material) {
	dist = MAXDISTANCE;
	float distcheck;
	
	vec3 sphereCenter, col;

	material = 0;	
	col = getBackgroundColor(ro, rd);
	
	if( (distcheck = intersectPlane( ro,  rd, 0.)) < MAXDISTANCE ) {
		dist = distcheck;
		material = 1;
		normal = vec3( 0., 1., 0. );
		col = vec3( 0.5 );
	}
	
	// trace grid
	vec3 pos = floor(ro/GRIDSIZE)*GRIDSIZE;
	vec3 ri = 1.0/rd;
	vec3 rs = sign(rd) * GRIDSIZE;
	vec3 dis = (pos-ro + 0.5  * GRIDSIZE + rs*0.5) * ri;
	vec3 mm = vec3(0.0);
	vec2 offset;
	
	for( int i=0; i<RAYCASTSTEPSRECURSIVE; i++ ) {
		if( material == 2 ) break; {		
			getSphereOffset( pos.xz, offset );			
			
			getMovingSpherePosition( pos.xz, -offset, sphereCenter );			
			if( (distcheck = intersectUnitSphere( ro, rd, sphereCenter )) < dist ) {
				dist = distcheck;
				normal = normalize((ro+rd*dist)-sphereCenter);
				col = getSphereColor(pos.xz);
				material = 2;
			}
			
			getSpherePosition( pos.xz, offset, sphereCenter );
			if( (distcheck = intersectUnitSphere( ro, rd, sphereCenter )) < dist ) {
				dist = distcheck;
				normal = normalize((ro+rd*dist)-sphereCenter);
				col = getSphereColor(pos.xz+vec2(1.,2.));
				material = 2;
			}
			
			mm = step(dis.xyz, dis.zyx);
			dis += mm * rs * ri;
			pos += mm * rs;	
		}
	}
	
	intersection = ro+rd*dist;
	
	return col;
}


vec3 trace(const vec3 ro, const vec3 rd, out vec3 intersection, out vec3 normal, out float dist, out int material) {
	dist = MAXDISTANCE;
	float distcheck;
	
	vec3 sphereCenter, col, normalcheck;
	
	material = 0;
	col = getBackgroundColor(ro, rd);
	
	if( (distcheck = intersectPlane( ro,  rd, 0.)) < MAXDISTANCE ) {
		dist = distcheck;
		material = 1;
		normal = vec3( 0., 1., 0. );
		col = vec3( 0.5 );
	} 
	
	// trace grid
	vec3 pos = floor(ro/GRIDSIZE)*GRIDSIZE;
	vec3 ri = 1.0/rd;
	vec3 rs = sign(rd) * GRIDSIZE;
	vec3 dis = (pos-ro + 0.5  * GRIDSIZE + rs*0.5) * ri;
	vec3 mm = vec3(0.0);
	vec2 offset;
		
	for( int i=0; i<RAYCASTSTEPS; i++ )	{
		if( material == 2 ||  distance( ro.xz, pos.xz ) > dist+GRIDSIZE ) break; {
			getSphereOffset( pos.xz, offset );
			
			getMovingSpherePosition( pos.xz, -offset, sphereCenter );			
			if( (distcheck = intersectUnitSphere( ro, rd, sphereCenter )) < dist ) {
				dist = distcheck;
				normal = normalize((ro+rd*dist)-sphereCenter);
				col = getSphereColor(pos.xz);
				material = 2;
			}
			
			getSpherePosition( pos.xz, offset, sphereCenter );
			if( (distcheck = intersectUnitSphere( ro, rd, sphereCenter )) < dist ) {
				dist = distcheck;
				normal = normalize((ro+rd*dist)-sphereCenter);
				col = getSphereColor(pos.xz+vec2(1.,2.));
				material = 2;
			}		
			mm = step(dis.xyz, dis.zyx);
			dis += mm * rs * ri;
			pos += mm * rs;		
		}
	}
	
	intersection = ro+rd*dist;
	
	return col;
}

vec2 rv2;

vec3 cosWeightedRandomHemisphereDirection2( const vec3 n ) {
	vec3  uu = normalize( cross( n, vec3(0.0,1.0,1.0) ) );
	vec3  vv = cross( uu, n );
	
	float ra = sqrt(rv2.y);
	float rx = ra*cos(6.2831*rv2.x); 
	float ry = ra*sin(6.2831*rv2.x);
	float rz = sqrt( 1.0-rv2.y );
	vec3  rr = vec3( rx*uu + ry*vv + rz*n );

    return normalize( rr );
}


void	main() {
	time = in_Time;
    vec2 q = gl_FragCoord.xy/in_Resolution.xy;
	vec2 p = -1.0+2.0*q;
	p.x *= in_Resolution.x/in_Resolution.y;
	
	vec3 col = vec3( 0. );
	
	// raytrace
	int material;
	vec3 normal, intersection;
	float dist;
	float seed = time+(p.x+in_Resolution.x*p.y)*1.51269341231;
	
	for( int j=0; j<SAMPLES; j++ ) {
		float fj = float(j);
		
#ifdef MOTIONBLUR
		time = in_Time + fj/(float(SAMPLES)*TARGETFPS);
#endif
		
		rv2 = hash2( 24.4316544311*fj+time+seed );
		
		vec2 pt = p+rv2/(0.5*in_Resolution.xy);
				
		// camera	
		vec3 ro = vec3( cos( 0.232*time) * 10., 6.+3.*cos(0.3*time), GRIDSIZE*(time/SPEED) );
		vec3 ta = ro + vec3( -sin( 0.232*time) * 10., -2.0+cos(0.23*time), 10.0 );
		
		float roll = -0.15*sin(0.5*time);
		
		// camera tx
		vec3 cw = normalize( ta-ro );
		vec3 cp = vec3( sin(roll), cos(roll),0.0 );
		vec3 cu = normalize( cross(cw,cp) );
		vec3 cv = normalize( cross(cu,cw) );
	
#ifdef DEPTHOFFIELD
    // create ray with depth of field
		const float fov = 3.0;
		
        vec3 er = normalize( vec3( pt.xy, fov ) );
        vec3 rd = er.x*cu + er.y*cv + er.z*cw;

        vec3 go = FOCUSBLUR*vec3( (rv2-vec2(0.5))*2., 0.0 );
        vec3 gd = normalize( er*FOCUSDISTANCE - go );
		
        ro += go.x*cu + go.y*cv;
        rd += gd.x*cu + gd.y*cv;
		rd = normalize(rd);
#else
		vec3 rd = normalize( pt.x*cu + pt.y*cv + 1.5*cw );		
#endif			
		vec3 colsample = vec3( 1. );
		
		// first hit
		rv2 = hash2( (rv2.x*2.4543263+rv2.y)*(time+1.) );
		colsample *= trace(ro, rd, intersection, normal, dist, material);

		// bounces
		for( int i=0; i<(PATHDEPTH-1); i++ ) {
			if( material != 0 ) {
				rd = cosWeightedRandomHemisphereDirection2( normal );
				ro = intersection + EPSILON*rd;
						
				rv2 = hash2( (rv2.x*2.4543263+rv2.y)*(time+1.)+(float(i+1)*.23) );
						
				colsample *= traceRec(ro, rd, intersection, normal, dist, material);
			}
		}	
		if( material == 0 ) {			
			col += colsample;	
		}
	}
	col  /= float(SAMPLES);
	
	col = pow( col, vec3(0.7) );	
	col = clamp(col, 0.0, 1.0);
	// contrast	
 //   col = clamp( col*0.7 + 0.3*col*col*(3.0-2.0*col), 0., 1.); 
	   
	// vigneting
	col *= 0.25+0.75*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );
	

	out_Color = vec4( col,1.0);
	out_Bright = vec4(0);
	out_Normal = vec4(frag_WorldNormal, 1);
	out_Position = vec4(frag_WorldPosition, 1);
	out_Color = vec4( col,1.0);
	out_Bright = vec4(0);
}

/* vec2 center = vec2(0.5,0.5);
float speed = 0.035;

void main()
{
    float invAr = in_Resolution.y / in_Resolution.x;

    vec2 uv = gl_FragCoord.xy / in_Resolution.xy;
		
	vec3 col = vec4(uv,0.5+0.5*sin(in_Time),1.0).xyz;
   
     vec3 texcol;
			
	float x = (center.x-uv.x);
	float y = (center.y-uv.y) *invAr;
		
	//float r = -sqrt(x*x + y*y); //uncoment this line to symmetric ripples
	float r = -(x*x + y*y);
	float z = 1.0 + 0.5*sin((r+in_Time*speed)/0.013);
	
	texcol.x = z;
	texcol.y = z;
	texcol.z = z;
	
	out_Color = vec4(col*texcol, 1.0);
}
 */
/* uniform float		gaussian_kernel[] = float[KERNEL_SIZE](
	0.06136, 0.24477, 0.38774, 0.24477, 0.06136
);

void main()
{
	vec3	normal = normalize(texture(in_Texture_Normal, frag_UV).xyz);
	vec3	position = texture(in_Texture_Position, frag_UV).xyz;
	float	depth = texture(in_Texture_Depth, frag_UV).r;
	float	dof = min(5, 10.f * abs(frag_CenterDepth - depth));
	vec4	env = textureLod(Environment.Diffuse, frag_Cube_UV, 10.f * abs(frag_CenterDepth - 1));
	vec4	color = textureLod(in_Texture_Color, frag_UV, dof);
	float	sampledist = 2.5;
	vec3	finalColor = vec3(0);
	float	occlusion = 0.f;
	if (depth != 1)
	{
		for (int i = 0; i < KERNEL_SIZE; i++) 
		{
			for (int j = 0; j < KERNEL_SIZE; j++) 
			{
				float	weight = gaussian_kernel[i] * gaussian_kernel[j];
				vec2	index = vec2(float(i - KERNEL_SIZE / 2.f), float(j - KERNEL_SIZE / 2.f));
				vec2	sampleUV = frag_UV + index * sampledist / textureSize(in_Texture_Position, 0);
				vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
				if (texture(in_Texture_Depth, sampleUV).r <= depth)
				{
					vec3	V = samplePosition - position;
					float	D = length(V);
					float	bias = 0.25;
					float	factor = max(0, dot(normal, normalize(V)));
					float	angle = max(0, factor - bias);
					occlusion += (angle * (1.f / (1.f + D))) * weight;
				}
			}
		}
	}
	occlusion *= (1 - dof / 5.f);
	finalColor = texture(in_Texture_Bright, frag_UV).rgb + color.rgb * color.a * (1 - occlusion);
	out_Color = (env) * max(0, 1 - color.a);
	out_Color += vec4(finalColor, 1);
	//out_Color.rgb = pow(out_Color.rgb, vec3(1 / 2.2));
	//out_Color = vec4(vec3(1 - occlusion), 1);
}
 */