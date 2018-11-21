#version 410 core

layout(quads, equal_spacing, ccw) in;

out vec2 TexCoord;

void main() {
	vec4 p00 = gl_in[ 0].gl_Position;
    vec4 p10 = gl_in[ 1].gl_Position;
    vec4 p20 = gl_in[ 2].gl_Position;
    vec4 p30 = gl_in[ 3].gl_Position;
    vec4 p40 = gl_in[ 4].gl_Position;
    vec4 p01 = gl_in[ 5].gl_Position;
    vec4 p11 = gl_in[ 6].gl_Position;
    vec4 p21 = gl_in[ 7].gl_Position;
    vec4 p31 = gl_in[ 8].gl_Position;
    vec4 p41 = gl_in[ 9].gl_Position;
    vec4 p02 = gl_in[10].gl_Position;
    vec4 p12 = gl_in[11].gl_Position;
    vec4 p22 = gl_in[12].gl_Position;
    vec4 p32 = gl_in[13].gl_Position;
    vec4 p42 = gl_in[14].gl_Position;
    vec4 p03 = gl_in[15].gl_Position;
    vec4 p13 = gl_in[16].gl_Position;
    vec4 p23 = gl_in[17].gl_Position;
    vec4 p33 = gl_in[18].gl_Position;
    vec4 p43 = gl_in[19].gl_Position;
    vec4 p04 = gl_in[20].gl_Position;
    vec4 p14 = gl_in[21].gl_Position;
    vec4 p24 = gl_in[22].gl_Position;
    vec4 p34 = gl_in[23].gl_Position;
    vec4 p44 = gl_in[24].gl_Position;

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    TexCoord = vec2(u, v);
    // Computing the Position, given a u and v
	// the basis functions:
    float bu0 = (1.-u) * (1.-u) * (1.-u) * (1.-u);
    float bu1 = 4. * u * (1.-u) * (1.-u) * (1.-u);
    float bu2 = 6. * u * u * (1.-u) * (1.-u);
    float bu3 = 4* u * u * u * (1.-u);
    float bu4 = u * u * u * u;
    float bv0 = (1.-v) * (1.-v) * (1.-v) * (1.-v);
    float bv1 = 4. * v * (1.-v) * (1.-v) * (1.-v);
    float bv2 = 6. * v * v * (1.-v) * (1.-v);
    float bv3 = 4* v * v * v * (1.-v);
    float bv4 = v * v * v * v;
    // finally, we get to compute something:
    gl_Position = bu0 * ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 + bv4*p04 ) + bu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 + bv4*p14 ) + bu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 + bv4*p24 ) + bu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 + bv4*p34 ) + bu4 * ( bv0*p40 + bv1*p41 + bv2*p42 + bv3*p43 + bv4*p44 );
}
