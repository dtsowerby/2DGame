#version 330 core
//https://www.shadertoy.com/view/Xd23Dh inigo quilez
in vec2 fragCoords;
out vec4 colour;

uniform float time;

vec3 hash3( vec2 p )
{
    vec3 q = vec3( dot(p,vec2(127.1,311.7)), 
				   dot(p,vec2(269.5,183.3)), 
				   dot(p,vec2(419.2,371.9)) );
	return fract(sin(q)*43758.5453);
}

float voronoise( in vec2 p, float u, float v )
{
	float k = 1.0+63.0*pow(1.0-v,6.0);

    vec2 i = floor(p);
    vec2 f = fract(p);
    
	vec2 a = vec2(0.0,0.0);
    for( int y=-2; y<=2; y++ )
    for( int x=-2; x<=2; x++ )
    {
        vec2  g = vec2( x, y );
		vec3  o = hash3( i + g )*vec3(u,u,1.0);
		vec2  d = g - f + o.xy;
		float w = pow( 1.0-smoothstep(0.0,1.414,length(d)), k );
		a += vec2(o.z*w,w);
    }
	
    return a.x/a.y;
}

void main()
{
    //vec2 scrollSpeed = vec2(sin(10.0*time), cos(10.0*time));
    vec2 uv = fragCoords / vec2(800.0, 600.0);

    vec2 p = 0.5 - 0.5*cos( vec2(10.0,12.0) );
	
	p = p*p*(3.0-2.0*p);
	p = p*p*(3.0-2.0*p);
	p = p*p*(3.0-2.0*p);

    float noiseValue = voronoise(8*uv, p.x, p.y);

    colour = vec4(sin(noiseValue+time) * 0.75, cos(noiseValue+time)*0.75, noiseValue, 1.0);
}
