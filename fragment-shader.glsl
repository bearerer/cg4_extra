//uniform sampler2D tex;
//uniform float ripple_offset;

//void main()
//{
//    vec2 tc = gl_TexCoord[0].xy;
//    tc.x += 0.005 * sin(ripple_offset + tc.y * 100.0);
//    gl_FragColor = vec4(1,0,0,1);//texture2D(tex, tc);
//}

#version 120

/* Lighting model to use:
 * 0 = Isotropic Blinn-Phong
 * 1 = Cook-Torrance
 * 2 = Anisotropic Blinn-Phong */
uniform int model;

/* Variables for lighting (all models) */
uniform vec3 light_color;       // Light color
uniform vec3 surface_color;     // Surface color
uniform float kd;               // Diffuse coefficient
uniform float ks;               // Specular coefficient
/* Variables for Blinn-Phong lighting (isotropic and anisotropic) */
uniform float shininess;        // Shininess
/* Variables for Cook-Torrance lighting */
uniform float r;                // Roughness
/* Variables for anisotropic Blinn-Phong lighting */
uniform float a;                // alpha_x
uniform float b;                // alpha_y

/* Variables from the vertex shader */
in vec3 gN;    // Normal in eye space, not normalized
in vec3 vT;    // Tangent in eye space, not normalized
in vec3 vB;    // Bitangent in eye space, not normalized
in vec3 gL;    // Light vector in eye space, not normalized
in vec3 gV;    // View vector in eye space, not normalized

/* Isotropic Blinn-Phong lighting */
vec3 blinn_phong(vec3 N, vec3 L, vec3 V, vec3 H)
{
    vec3 diffuse = kd * light_color * dot(L, N);
    vec3 specular = ks * light_color * pow(dot(H, N), shininess);
    return diffuse + specular;
}

float fresnel(vec3 N, vec3 V){
        float F = 1.0 - dot(N, V);
        F *= F;
        F *= F;
        return F;
}

float distribution (vec3 N, vec3 H){
        float nh = dot(N, H);
        float exponent = - ((1 - nh * nh) / (r * r * nh * nh));
        float nenner = 4 * r * r * nh * nh * nh * nh;
        return exp(exponent) / nenner;
}

float self_shadowing(vec3 N, vec3 H, vec3 V, vec3 L){
        float dnh = dot(N, H);
        float dnv = dot(N, V);
        float dvh = dot(V, H);
        float dnl = dot(N, L);
        float term1 = 2.0 * dnh * dnv / dvh;
        float term2 = 2.0 * dnh * dnl / dvh;
        return min(1.0, min(term1, term2));
}

/* Cook-Torrance lighting */
vec3 cook_torrance(vec3 N, vec3 L, vec3 V, vec3 H)
{
        float F = fresnel(N, V);
        float D = distribution(N, H);
        float G = self_shadowing(N, H, V, L);
        vec3 L_D = light_color * kd * dot(N, L);
        vec3 L_S = light_color * ks * (F * D * G) / dot(N, V);

        return L_D + L_S;
}

/* Anisotropic Blinn-Phong lighting */
vec3 anisotropic_blinn_phong(vec3 N, vec3 T, vec3 B, vec3 L, vec3 V, vec3 H)
{
        float dth = dot (T, H);
        float dbh = dot (B, H);
        vec3 diffuse = kd * light_color * dot(L, N);
        vec3 specular = ks * light_color * max (0.0, pow (sqrt (
                1.0
                - (dth * dth / a)
                - (dbh * dbh / b)
        ), shininess));
        return diffuse + specular;
}

void main(void)
{
    // Normalize the input from the vertex shader
    vec3 N = normalize(gN);
    vec3 T = normalize(vT);
    vec3 B = normalize(vB);
    vec3 L = normalize(gL);
    vec3 V = normalize(gV);
    vec3 H = normalize(L + V);

    vec3 color;
    if (model == 0) {
        color = blinn_phong(N, L, V, H);
    } else if (model == 1) {
        color = cook_torrance(N, L, V, H);
    } else {
        color = anisotropic_blinn_phong(N, T, B, L, V, H);
    }

    // Resulting color at this fragment:
    gl_FragColor = vec4(surface_color * color, 1.0);
}
