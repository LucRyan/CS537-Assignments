
// per-fragment interpolated values from the vertex shader
varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

void main() 
{ 
// Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    
//-----------------------------------------------------------
//light0    
    vec3 L0 = normalize(fL);
    vec3 H0 = normalize( L0 + E );
    vec4 ambient0 = AmbientProduct;
    float Kd0 = max(dot(L0, N), 0.0);
    vec4 diffuse0 = Kd0*DiffuseProduct;
    float Ks0 = pow(max(dot(N, H0), 0.0), Shininess);
    vec4 specular0 = Ks0*SpecularProduct;
    // discard the specular highlight if the light's behind the vertex
    if( dot(L0, N) < 0.0 ) {
		specular0 = vec4(0.0, 0.0, 0.0, 1.0);
    }
//-----------------------------------------------------------
    gl_FragColor = ambient0 + diffuse0 + specular0;
    gl_FragColor.a = 0.5;
} 

