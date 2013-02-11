uniform samplerCube cubeMap;
varying vec3 vTexCoord;

void main (void)
{
	if ( gl_FrontFacing ) {
		gl_FragColor = texture(cubeMap, vTexCoord);
	} else
   		gl_FragColor = vec4(0, 0, 0, 1);
} //