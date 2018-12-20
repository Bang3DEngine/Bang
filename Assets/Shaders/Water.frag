#define BANG_FRAGMENT
#define BANG_FORWARD_RENDERING

#include "Common.glsl"
#include "LightCommon.glsl"
#include "ForwardLightCommon.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
in vec2 B_FIn_NormalMapUv;
in vec3 B_FIn_Tangent;
in mat3 B_TBN;

layout(location = 0) out vec4 B_GIn_Color;

float GetWaterHeight(vec2 positionInPlane)
{
    vec2 p = positionInPlane;
    const float HeightAmount = 0.2f;
    return (sin((p.x * 2.7f + p.y * 2.4f) + B_TimeSeconds * 0.7f) * 1.0f +
            cos((p.x * 0.2f + p.y * 6.0f) + B_TimeSeconds * 0.62f) * 0.5f +
            cos((p.x * 4.5f + p.y * 0.4f) + B_TimeSeconds * 2.0f) * 0.3f +
            sin((p.x * 3.0f + p.y * 5.0f) + B_TimeSeconds * 1.4f) * 0.7f) *
           HeightAmount;
}

void main()
{
    const float WaterRoughness = 0.0f;
    const float WaterMetalness = 0.3f;
    vec4 WaterColor = vec4(0.1f, 0.4f, 0.7f, 0.5f);

    const vec2 DeltaPos    = vec2(0.01);
    vec3 position          = B_FIn_Position.xyz;
    vec3 neighbor0Position = position + vec3(DeltaPos.x, 0, 0);
    vec3 neighbor1Position = position + vec3(0, 0, DeltaPos.y);

    float pixelHeight     = GetWaterHeight(position.xz);
    float neighbor0Height = GetWaterHeight(neighbor0Position.xz);
    float neighbor1Height = GetWaterHeight(neighbor1Position.xz);
    position          += vec3(0,     pixelHeight, 0);
    neighbor0Position += vec3(0, neighbor0Height, 0);
    neighbor1Position += vec3(0, neighbor1Height, 0);
    vec3 normal = -normalize( cross(neighbor0Position - position,
                                    neighbor1Position - position) );

    vec3 viewVector = normalize(position - B_Camera_WorldPos.xyz);
    vec3 reflectedViewVector = reflect(viewVector, normal);

    vec4 finalColor = GetIBLAmbientColor(true, position, normal, WaterColor,
                                         WaterRoughness, WaterMetalness);
    finalColor += vec4(GetForwardLightApport(position,
                                             normal,
                                             WaterColor.rgb,
                                             WaterRoughness,
                                             WaterMetalness), 0);
    // finalColor = texture(B_SkyBoxSpecular, reflectedViewVector);
    B_GIn_Color = finalColor;
}
