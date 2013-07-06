uniform sampler2D tex;
uniform float ripple_offset;

void main()
{
    vec2 tc = gl_TexCoord[0].xy;
    tc.x += 0.005 * sin(ripple_offset + tc.y * 100.0);
    gl_FragColor = texture2D(tex, tc);
}
