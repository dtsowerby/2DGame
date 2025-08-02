#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

// Post-processing parameters
uniform float vignetteStrength;
uniform float chromaticAberrationStrength;

void main()
{
    vec2 uv = TexCoords;
    vec3 color = vec3(0.0);
    
    // Chromatic Aberration
    float aberration = chromaticAberrationStrength;
    vec2 direction = uv - vec2(0.5);
    float dist = length(direction);
    direction = normalize(direction);
    
    // Sample red, green, and blue channels with slight offsets
    color.r = texture(screenTexture, uv + direction * aberration * dist * 2.0).r;
    color.g = texture(screenTexture, uv).g;
    color.b = texture(screenTexture, uv - direction * aberration * dist * 2.0).b;

    // Vignette Effect
    vec2 vignetteUV = uv - vec2(0.5);
    float vignette = 1.0 - dot(vignetteUV, vignetteUV) * vignetteStrength;
    vignette = clamp(vignette, 0.0, 1.0);
    
    // Apply vignette
    color *= vignette;
    
    // Add some film grain using time
    float grain = (fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 0.02;
    color += grain * sin(time * 10.0) * 0.5;
    
    FragColor = vec4(color, 1.0);
}
