#version 120
#extension GL_EXT_geometry_shader4 : enable
//#extension GL_EXT_gpu_shader4: enable

/* Variables from the vertex shader */
varying in vec3 vN[];   // Normal in eye space, not normalized
varying in vec3 vL[];   // Light vector in eye space, not normalized
varying in vec3 vV[];   // View vector in eye space, not normalized
/* Variables passed to the fragment shader */
varying out vec3 gN;    // Normal in eye space, not normalized
varying out vec3 gL;    // Light vector in eye space, not normalized
varying out vec3 gV;    // View vector in eye space, not normalized

void emit_vertex(vec4 pos, vec3 normal, vec3 light, vec3 view)
{
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    gN = normal;
    gL = light;
    gV = view;
    EmitVertex();
}

void main(void)
{
    vec4 v0 = gl_PositionIn[0];
    vec4 v1 = gl_PositionIn[1];
    vec4 v2 = gl_PositionIn[2];

    // Emit original triangle
    emit_vertex(v0, vN[0], vL[0], vV[0]);
    emit_vertex(v1, vN[1], vL[1], vV[1]);
    emit_vertex(v2, vN[2], vL[2], vV[2]);
    EndPrimitive();

    emit_vertex(v0 + vec4(0.f, 1.f, 0.f, 0.0), vN[0], vL[0], vV[0]);
    emit_vertex(v0 + vec4(1.f, 1.f, 0.f, 0.0), vN[0], vL[0], vV[0]);
    emit_vertex(v0 + vec4(0.f, 1.2f, 0.f, 0.0), vN[0], vL[0], vV[0]);
    EndPrimitive();

//    vec4 c = (v0 + v1 + v2) / 3.0;
//    vec3 n = normalize(cross((v1 - v0).xyz, (v2 - v0).xyz));
//    vec4 s = c + vec4(spike_length * n, 0.0);
//    vec4 b0 = c + base_length * (c - v0);
//    vec4 b1 = c + base_length * (c - v1);
//    vec4 b2 = c + base_length * (c - v2);
//    emit_vertex(c,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(s,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(b0, vec3(0.0), vec3(0.0), vec3(0.0));
//    EndPrimitive();
//    emit_vertex(c,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(s,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(b1, vec3(0.0), vec3(0.0), vec3(0.0));
//    EndPrimitive();
//    emit_vertex(c,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(s,  vec3(0.0), vec3(0.0), vec3(0.0));
//    emit_vertex(b2, vec3(0.0), vec3(0.0), vec3(0.0));
//    EndPrimitive();

}
