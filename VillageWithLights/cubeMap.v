varying vec3 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection;

void main(void)
{
   	vec4 vCoords = vec4(reflect(gl_Vertex, gl_Normal), 0.0);
   	vCoords = gl_ModelViewMatrixInverse * vCoords;
   	vTexCoord = normalize(vCoords.xyz);
    
	gl_Position = Projection*ModelView*gl_Vertex;
} //