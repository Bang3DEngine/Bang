#ifndef SCREEN_PASS_FRAG
#define SCREEN_PASS_FRAG

#define BANG_FRAGMENT
#include "Common.glsl"

in vec3 B_FIn_Position;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;

#endif
