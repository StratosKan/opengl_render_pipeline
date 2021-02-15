#version 330 core
out vec4 fragColor;

vec3 Cyka(float t)
{
    vec3 r = t * 2.1 - vec3(1.8, 1.14, 0.3);
    return 0.99 - r * r;
}

uniform vec3 viewPos;

void main()
{
    vec2 uv = gl_FragCoord.xy / viewPos.xy;
    vec3 col0 = Cyka(uv.x);
    vec3 col1 = Cyka(uv.x * 2.0 - 0.5);
    vec3 col = vec3(0.0);
    if (uv.y > 0.05 && uv.y < 0.45)
        col = col1;
    if (uv.y < 0.95 && uv.y > 0.55)
        col = col0;
    fragColor = vec4(col, 1.0);
}