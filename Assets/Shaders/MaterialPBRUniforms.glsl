#ifndef MATERIALPBRUNIFORMS_GLSL
#define MATERIALPBRUNIFORMS_GLSL

uniform sampler2D B_BRDF_LUT;
uniform sampler2D B_RoughnessTexture;
uniform sampler2D B_MetalnessTexture;
uniform sampler2D B_NormalMapTexture;
uniform vec2      B_NormalMapUvOffset;
uniform float     B_MaterialRoughness;
uniform float     B_MaterialMetalness;
uniform vec2      B_NormalMapUvMultiply;
uniform bool      B_HasNormalMapTexture;
uniform float     B_NormalMapMultiplyFactor;

#endif


