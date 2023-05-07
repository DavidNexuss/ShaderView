#version 330
out vec3 color;
uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform sampler2D iChannel0;
uniform float divisor;

void main()
{
    vec2 uv = (gl_FragCoord.xy / iResolution);
    uv.y = 1 - uv.y;
    color = texture2D(iChannel0, uv).xyz / divisor;
}
