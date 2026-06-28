#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform vec3 sunDir;
uniform float time;
uniform vec3 fogColor;

uniform vec3 zenith;
uniform vec3 horizon;
uniform vec3 ground;

vec3 proceduralSky(vec3 dir) {
    float y = normalize(dir).y;

    vec3 skyColor;
    if (y > 0.0) {
        float t = pow(y, 0.4);
        skyColor = mix(horizon, zenith, t);
    } else {
        float t = pow(-y, 0.6);
        skyColor = mix(horizon, ground, t);
    }

    vec3 sun = normalize(-sunDir);
    float sunDot = dot(normalize(dir), sun);
    float sunDisc = smoothstep(0.999, 0.9999, sunDot);
    float sunGlow = pow(max(sunDot, 0.0), 64.0) * 0.3;
    float sunHalo = pow(max(sunDot, 0.0), 8.0) * 0.08;

    vec3 sunColor = vec3(1.0, 0.95, 0.8);
    skyColor += sunColor * (sunDisc * 2.0 + sunGlow + sunHalo);

    float horizonGlow = pow(1.0 - abs(y), 8.0) * 0.15;
    skyColor += vec3(0.4, 0.45, 0.5) * horizonGlow;

    return skyColor;
}

void main() {
    vec3 dir = normalize(texCoords);
    vec3 color = proceduralSky(dir);

    float dist = length(texCoords);
    float fogFactor = clamp((300.0 - dist) / 300.0, 0.0, 1.0);
    color = mix(fogColor, color, fogFactor);

    FragColor = vec4(color, 1.0);
}
