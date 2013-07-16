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

uniform float branchHeight;
uniform float branchThickness;
uniform vec3 light_position;    // Light position in object space

void emit_vertex(vec4 pos, vec3 normal, vec3 light, vec3 view)
{
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    gN = normal;
    gL = light;
    gV = view;
    EmitVertex();
}

void emit_vertex_auto(vec4 pos, vec3 normal){
    vec3 eye_pos = vec3(0.0); // The eye position in eye space is always (0,0,0)
    vec3 light_pos = (gl_ModelViewMatrix * vec4(light_position, 1.0)).xyz;

    gl_Position = gl_ModelViewProjectionMatrix * pos;
    gN = normal;
    gL = light_pos - pos.xyz;
    gV = eye_pos - pos.xyz;

    EmitVertex();
}

void drawBranch(vec4 a, vec4 b, vec4 c){
    vec4 ac = 0.5 * (a + c);
    vec4 M = branchHeight * b + (1.0 - branchHeight) * ac;

    vec4 d = branchThickness * M + (1.0 - branchThickness) * a;
    vec4 e = branchThickness * M + (1.0 - branchThickness) * b;
    vec4 f = branchThickness * M + (1.0 - branchThickness) * c;

    vec3 gNorm = normalize(cross((e - d).xyz, (f - d).xyz));

//    vec4 g = M + 0.3333 * vec4(vN[0] + vN[1] + vN[2], 0.0) * length(M - ac);
    vec4 g = M + vec4(gNorm, 0.0) * branchHeight * length(M - ac);

    vec3 dNorm = normalize(cross((g - e).xyz, (f - e).xyz));
    vec3 eNorm = normalize(cross((g - f).xyz, (d - f).xyz));
    vec3 fNorm = normalize(cross((g - d).xyz, (e - d).xyz));

    emit_vertex_auto(d, dNorm);
    emit_vertex_auto(g, gNorm);
    emit_vertex_auto(f, fNorm);
    EndPrimitive();

    emit_vertex_auto(f, fNorm);
    emit_vertex_auto(g, gNorm);
    emit_vertex_auto(e, eNorm);
    EndPrimitive();

    emit_vertex_auto(e, eNorm);
    emit_vertex_auto(g, gNorm);
    emit_vertex_auto(d, dNorm);
    EndPrimitive();

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

    drawBranch(v0, v1, v2);
}
