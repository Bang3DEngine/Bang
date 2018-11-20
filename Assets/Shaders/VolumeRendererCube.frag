#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#include "Common.glsl"
#include "LightCommon.glsl"
#include "MaterialPBRUniforms.glsl"

uniform bool B_RenderingCubeBackFaces;
uniform float B_DensityThreshold;
uniform sampler2D B_CubeBackFacesColor;
uniform sampler3D B_Texture3D;

in vec3 B_FIn_Color;
in vec3 B_FIn_Position;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

float GetDensityAt(vec3 texPoint)
{
    return texture3D(B_Texture3D, texPoint).r;
}

void main()
{
    if (B_RenderingCubeBackFaces)
    {
        vec4 color = vec4(B_FIn_Color, 1);
        B_GIn_Color = B_GIn_Albedo = color;
        B_GIn_Misc = B_GIn_Normal = vec4(0);
        return;
    }

    vec3 rayIn, rayOut;
    {
        rayIn = B_FIn_Color;
        vec2 screenUv = B_GetViewportUv();
        rayOut = texture2D(B_CubeBackFacesColor, screenUv).rgb;

        vec3 rayInOutDiff = (rayOut - rayIn);
        float sqDistToTravel = dot(rayInOutDiff, rayInOutDiff);
        if (sqDistToTravel <= 0.001f)
        {
           discard;
        }
    }
    vec3 rayDir = normalize(rayOut - rayIn);

    vec3 worldPosition;
    vec3 normal;

    bool draw = false;
    vec4 albedoColor = vec4(0,0,0,1);
    float numSteps = 100.0f; // (1.0f / sampleStep);
    float sampleStep = 1.0f / numSteps;
    float currentSampleDist = 0.001f;
    for (int i = 0; i < numSteps; ++i)
    {
        vec3 currentSamplePoint = (rayIn + rayDir * currentSampleDist);
        {
            const float margins = 0.0f;
            const float invMargins = 1.0f - margins;
            if (currentSamplePoint.x < 0.0f || currentSamplePoint.x > invMargins ||
                currentSamplePoint.y < 0.0f || currentSamplePoint.y > invMargins ||
                currentSamplePoint.z < 0.0f || currentSamplePoint.z > invMargins)
            {
                discard;
            }
        }

        float currentDensity = GetDensityAt(currentSamplePoint);
        if (currentDensity <= B_DensityThreshold)
        {
            draw = true;
            albedoColor = vec4(0,1,0,1);

            // Get gradient
            vec3 voxelSize = vec3(1.0f / 33.0f);
            vec3 offset = voxelSize;
            vec3 offsetDensityBefore = vec3(
                        GetDensityAt(currentSamplePoint - voxelSize * vec3(1,0,0)),
                        GetDensityAt(currentSamplePoint - voxelSize * vec3(0,1,0)),
                        GetDensityAt(currentSamplePoint - voxelSize * vec3(0,0,1)));
            vec3 offsetDensityAfter = vec3(
                        GetDensityAt(currentSamplePoint + voxelSize * vec3(1,0,0)),
                        GetDensityAt(currentSamplePoint + voxelSize * vec3(0,1,0)),
                        GetDensityAt(currentSamplePoint + voxelSize * vec3(0,0,1)));
            vec3 gradient = (offsetDensityAfter - offsetDensityBefore);

            worldPosition = B_FIn_Position + (rayDir * currentSampleDist);
            normal = normalize((B_Normal * vec4(normalize(gradient), 0))).xyz;

            albedoColor = B_MaterialAlbedoColor;
            break;
        }
        currentSampleDist += sampleStep;
    }

    if (draw)
    {
        float receivesLighting = 1.0f;
        float pixelRoughness = B_MaterialRoughness;
        float pixelMetalness = B_MaterialMetalness;

        vec4 color = GetIBLAmbientColor(worldPosition, normal, albedoColor,
                                        pixelRoughness, pixelMetalness);

        B_GIn_Color = color;
        B_GIn_Albedo = albedoColor;
        B_GIn_Normal = vec4(normal * 0.5f + 0.5f, 0);
        B_GIn_Misc = vec4(receivesLighting,
                          pixelRoughness,
                          pixelMetalness,
                          0);

        vec4 worldPositionProj = (B_PVM * vec4(worldPosition, 1));
        gl_FragDepth = worldPositionProj.z / worldPositionProj.w;
    }
    else
    {
        discard;
    }
}
