#version 130

/* Variables for lighting */
uniform vec3 light_position;    // Light position in object space

/* Variables passed to the geometry shader */
varying out vec3 vN;    // Normal in eye space, not normalized
varying out vec3 vL;    // Light vector in eye space, not normalized
varying out vec3 vV;    // View vector in eye space, not normalized

//uniform int layer;
//uniform int max_layers;
uniform sampler2D tex;
void main(void)
{
    /* Geometry computation. */
    vec4 position = gl_Vertex;

    // Transform the vertex:
    gl_Position = /*gl_ModelViewProjectionMatrix **/ position;
    // Do not tranform the vertex here; we do that in the geometry shader!

    /* Lighting computation, in eye space. */

    // The position in eye space.
    vec3 pos = (gl_ModelViewMatrix * gl_Vertex).xyz;
    // The eye position in eye space is always (0,0,0).
    vec3 eye_pos = vec3(0.0);
    // The light position in eye space.
    vec3 light_pos = (gl_ModelViewMatrix * vec4(light_position, 1.0)).xyz;
    // The normal in eye space.
    vN = gl_NormalMatrix * gl_Normal;
    // The light vector: from vertex position to light position
    vL = light_pos - pos.xyz;
    // The view vector: from vertex position to eye position
    vV = eye_pos - pos.xyz;
}
