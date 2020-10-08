float wave(float a)
{
    return (sin(a) + 1.0) * 0.5;
}

float path(float a)
{
    return (wave(12.0 - iTime * (83.0 + a) * .02) + 
        		 wave(21.0 - iTime * (97.0 + a) * .02) +
        		 wave(54.0 - iTime * (61.0 + a) * .02)) / 3.0;
}

vec4 particle(float a,vec2 uv)
{
    vec2 m = vec2(path(71.0 + a*3.0),path(89.0 - a*2.0)) - 0.5;
    float dst = (uv.x - m.x) * (uv.x - m.x) + (uv.y - m.y) * (uv.y - m.y);
    dst = sqrt(dst);
    dst *= 20.0;
    return vec4(vec3(
        wave((iTime*(83.0 + a*5.0) - 5.0) * 0.02),
        wave((iTime*(97.0 + a*7.0) - 2.0) * 0.02),
        wave((iTime*(61.0 + a*11.0) - 5.0) * 0.02)) * vec3(1.0  / dst),1.0);
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy - 0.5;
    //vec2 m = iMouse.xy / iResolution.xy - 0.5;
	fragColor = particle(2.0,uv);
	fragColor += particle(4.0,uv);
	fragColor += particle(-4.0,uv);
	fragColor += particle(11.0,uv);
}
