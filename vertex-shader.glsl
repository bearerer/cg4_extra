#version 330

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

/* Variables for lighting */
uniform vec3 light_position;    // Light position in object space

/* Variables passed to the fragment shader */
out vec3 vN;    // Normal in eye space, not normalized
out vec3 vT;    // Tangent in eye space, not normalized
out vec3 vB;    // Bitangent in eye space, not normalized
out vec3 vL;    // Light vector in eye space, not normalized
out vec3 vV;    // View vector in eye space, not normalized

void main(void)
{
    /* Geometry computation. */

    // Transform the vertex:
    gl_Position = vec4(in_pos, 1.0);

    /* Lighting computation, in eye space. */

    // The position in eye space.
//    vec3 pos = (gl_ModelViewMatrix * in_pos).xyz;
    vec3 pos = in_pos;
    // The eye position in eye space is always (0,0,0).
    vec3 eye_pos = vec3(0.0);
    // The light position in eye space.
//    vec3 light_pos = (gl_ModelViewMatrix * vec4(light_position, 1.0)).xyz;
    vec3 light_pos = light_position;
    // The normal in eye space.
//    vN = gl_NormalMatrix * gl_Normal;
//    vN = gl_NormalMatrix * in_normal;
    vN = in_normal;

    // The tangent in eye space.
    // Note: normally these would be precomputed and stored with the model.
    // This method here is just a hack. For example, it will fail if vN is
    // approximately parallel to (1 0 0).
    vT = cross(vN, vec3(1.0, 0.0, 0.0));
    // The bitangent in eye space.
    vB = cross(vN, vT);
    // The light vector: from vertex position to light position
    vL = light_pos - pos.xyz;
    // The view vector: from vertex position to eye position
    vV = eye_pos - pos.xyz;
}
