#version 120

/* Variables for lighting */
uniform vec3 light_position;    // Light position in object space

attribute vec4 InVertex;  //w will be set to 1.0 automatically
attribute vec3 InNormal;

/* Variables passed to the fragment shader */
varying out vec3 vN;    // Normal in eye space, not normalized
varying out vec3 vT;    // Tangent in eye space, not normalized
varying out vec3 vB;    // Bitangent in eye space, not normalized
varying out vec3 vL;    // Light vector in eye space, not normalized
varying out vec3 vV;    // View vector in eye space, not normalized

void main(void)
{
    /* Geometry computation. */

    // Transform the vertex:
//    gl_Position = gl_Vertex;
    gl_Position = InVertex;

    /* Lighting computation, in eye space. */

    // The position in eye space.
    vec3 pos = (gl_ModelViewMatrix * gl_Vertex).xyz;
    // The eye position in eye space is always (0,0,0).
    vec3 eye_pos = vec3(0.0);
    // The light position in eye space.
    vec3 light_pos = (gl_ModelViewMatrix * vec4(light_position, 1.0)).xyz;
    // The normal in eye space.
//    vN = gl_NormalMatrix * gl_Normal;
    vN = gl_NormalMatrix * InNormal;

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
