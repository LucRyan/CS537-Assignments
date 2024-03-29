attribute  vec4 vPosition;
attribute  vec4 vColor;
attribute  vec2 vTexCoord;

varying vec4 color;
varying vec2 texCoord;

uniform vec3 theta;

void main() 
{
    const float  DegreesToRadians = 3.14159265 / 180.0;
    
    vec3 c = cos( 7*DegreesToRadians * theta );
    vec3 s = sin( 7*DegreesToRadians * theta );

    mat4 rx = mat4( 1.0, 0.0,  0.0, 0.0,
		    0.0, c.x, -s.x, 0.0,
		    0.0, s.x,  c.x, 0.0,
		    0.0, 0.0,  0.0, 1.0);

    mat4 ry = mat4(   c.y, 0.0, s.y, 0.0,
		      0.0, 1.0, 0.0, 0.0,
		     -s.y, 0.0, c.y, 0.0,
		      0.0, 0.0, 0.0, 1.0 );

    
    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
		    s.z,  c.z, 0.0, 0.0,
		    0.0,  0.0, 1.0, 0.0,
		    0.0,  0.0, 0.0, 1.0 );

    color       = vColor;
    texCoord    = vTexCoord;
    gl_Position = rz * ry * rx * vPosition;
} 
