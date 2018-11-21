#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#include "Common.glsl"
#include "LightCommon.glsl"
#include "MaterialPBRUniforms.glsl"

uniform bool B_RenderingCubeBackFaces;
uniform float B_DensityThreshold;
uniform sampler2D B_CubeBackFacesColor;
uniform sampler3D B_Texture3D;
uniform vec3 B_Texture3DSize;
uniform vec3 B_Texture3DPOTSize;
uniform int B_NumSamples;

in vec3 B_FIn_ModelPosition;

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
    vec3 actualSizeRatio = (B_Texture3DSize / B_Texture3DPOTSize);
    if (B_RenderingCubeBackFaces)
    {
        vec4 color = vec4(B_FIn_ModelPosition + 0.5f, 1);
        B_GIn_Color = B_GIn_Albedo = color;
        B_GIn_Misc = B_GIn_Normal = vec4(0);
        return;
    }

    vec3 rayIn, rayOut;
    {
        rayIn = B_FIn_ModelPosition + 0.5f;
        vec2 screenUv = B_GetViewportUv();
        rayOut = texture2D(B_CubeBackFacesColor, screenUv).rgb;

        rayIn  *= actualSizeRatio;
        rayOut *= actualSizeRatio;

        vec3 rayInOutDiff = (rayOut - rayIn);
        float sqDistToTravel = dot(rayInOutDiff, rayInOutDiff);
        // if (sqDistToTravel <= 0.0f)
        // {
        //    discard;
        // }
    }
    vec3 rayDir = normalize(rayOut - rayIn);

    vec3 worldPosition;
    vec3 normal;

    bool draw = false;
    vec4 albedoColor = vec4(0,0,0,1);
    float numSteps = B_NumSamples;
    float sampleStep = 1.0f / numSteps;
    float currentSampleDist = 0.001f;
    vec3 voxelSize = (vec3(1.0f) / B_Texture3DPOTSize);
    vec3 margins = voxelSize * 0.0f;
    vec3 invMargins = 1.0f - margins;
    for (int i = 0; i < B_NumSamples; ++i)
    {
        vec3 currentSamplePoint = (rayIn + rayDir * currentSampleDist);
        {
            if (currentSamplePoint.x < margins.x || currentSamplePoint.x > invMargins.x ||
                currentSamplePoint.y < margins.y || currentSamplePoint.y > invMargins.y ||
                currentSamplePoint.z < margins.z || currentSamplePoint.z > invMargins.z)
            {
                discard;
            }
        }

        float currentDensity = GetDensityAt(currentSamplePoint);
        if (currentDensity <= B_DensityThreshold)
        {
            draw = true;

            // Get gradient
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

            worldPosition = (B_Model * vec4(B_FIn_ModelPosition +
                                            (rayDir * currentSampleDist), 1)).xyz;
            normal = normalize((B_Model * vec4(normalize(gradient), 0))).xyz;

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
        // B_GIn_Color = vec4(worldPosition, 1); receivesLighting = 0.0f;
        B_GIn_Albedo = albedoColor;
        B_GIn_Normal = vec4(normal * 0.5f + 0.5f, 0);
        B_GIn_Misc = vec4(receivesLighting,
                          pixelRoughness,
                          pixelMetalness,
                          0);

        vec4 worldPositionProj = (B_ProjectionView * vec4(worldPosition, 1));
        gl_FragDepth = (worldPositionProj.z / worldPositionProj.w) * 0.5f + 0.5f;
    }
    else
    {
        discard;
    }
}
