#version 120

uniform mat4 projection;
uniform mat4 modelview;
void main() 
{ 
  gl_Position = projection*modelview*gl_Vertex;
}