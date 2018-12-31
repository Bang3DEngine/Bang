#ifndef MATERIALPBRUNIFORMS_GLSL
#define MATERIALPBRUNIFORMS_GLSL

#ifdef BANG_UNIFORMS_MATERIAL_PBR
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
uniform bool      B_MaterialReceivesLighting;
#endif

#endif


