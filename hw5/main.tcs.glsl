#version 410 core

layout( vertices = 25 ) out;

uniform float uOuter02, uOuter13, uInner0, uInner1;

void main(){
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	// set tessellation levels
	gl_TessLevelOuter[0] = uOuter02;
    gl_TessLevelOuter[1] = uOuter13;
    gl_TessLevelOuter[2] = uOuter02;
    gl_TessLevelOuter[3] = uOuter13;
    gl_TessLevelInner[0] = uInner0;
    gl_TessLevelInner[1] = uInner1;
}
