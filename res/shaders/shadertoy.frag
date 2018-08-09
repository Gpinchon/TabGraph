#version 410
#define M_PI 3.1415926535897932384626433832795

uniform lowp vec3		in_CamPos;
uniform lowp vec3		in_Albedo;
uniform lowp vec3		in_Emitting;
uniform lowp vec3		in_Resolution;
uniform lowp float		in_Time;
uniform lowp float		in_Alpha;

uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo = false;
uniform sampler2D	in_Texture_Emitting;
uniform bool		in_Use_Texture_Normal = false;
uniform sampler2D	in_Texture_Normal;

uniform samplerCube	Environment.Diffuse;
uniform samplerCube	Environment.Irradiance;
uniform sampler2D	in_Texture_BRDF;

in vec3			frag_WorldPosition;
in lowp vec3	frag_WorldNormal;
in lowp vec2	frag_Texcoord;

layout(location = 0) out lowp vec4	out_Color;
layout(location = 1) out lowp vec4	out_Bright;
layout(location = 2) out lowp vec4	out_Normal;
layout(location = 3) out vec4		out_Position;

float noise(vec3 p) //Thx to Las^Mercury
{
	vec3 i = floor(p);
	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
	a.xy = mix(a.xz, a.yw, f.y);
	return mix(a.x, a.y, f.z);
}

float sphere(vec3 p, vec4 spr)
{
	return length(spr.xyz-p) - spr.w;
}

float flame(vec3 p)
{
	float d = sphere(p*vec3(1.,.5,1.), vec4(.0,-1.,.0,1.));
	return d + (noise(p+vec3(.0,in_Time*2.,.0)) + noise(p*3.)*.5)*.25*(p.y) ;
}

float scene(vec3 p)
{
	return min(100.-length(p) , abs(flame(p)) );
}

vec4 raymarch(vec3 org, vec3 dir)
{
	float d = 0.0, glow = 0.0, eps = 0.02;
	vec3  p = org;
	bool glowed = false;
	
	for(int i=0; i<64; i++)
	{
		d = scene(p) + eps;
		p += d * dir;
		if( d>eps )
		{
			if(flame(p) < .0)
				glowed=true;
			if(glowed)
       			glow = float(i)/64.;
		}
	}
	return vec4(p,glow);
}


vec2 center = vec2(0.5,0.5);
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
