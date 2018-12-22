#define BANG_FRAGMENT

#include "Common.glsl"
#include "LightCommon.glsl"
#include "MaterialPBRUniforms.glsl"
#ifdef BANG_FORWARD_RENDERING
#include "ForwardLightCommon.glsl"
#endif

uniform bool B_RenderingCubeBackFaces;
uniform bool B_InvertNormals;
uniform float B_DensityThreshold;
uniform float B_SurfaceThickness;
uniform float B_AlphaMultiply;
uniform sampler2D B_CubeBackFacesColor;
uniform sampler2D B_TransferFunctionTexture;
uniform sampler3D B_Texture3D;
uniform vec3 B_Texture3DSize;
uniform vec3 B_Texture3DPOTSize;
uniform int B_NumSamples;
uniform vec3 B_RenderCubeMin;
uniform vec3 B_RenderCubeMax;

in vec3 B_FIn_ModelPosition;

#ifdef BANG_FORWARD_RENDERING

layout(location = 0) out vec4 B_GIn_Color;

#else

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Light;
layout(location = 3) out vec4 B_GIn_Normal;
layout(location = 4) out vec4 B_GIn_Misc;

#endif

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

        #ifdef BANG_FORWARD_RENDERING
        B_GIn_Color = color;
        #else
        B_GIn_Color = B_GIn_Albedo = color;
        B_GIn_Misc = B_GIn_Normal = vec4(0);
        #endif

        return;
    }

    vec3 rayIn, rayOut;
    {
        rayIn = B_FIn_ModelPosition + 0.5f;
        vec2 screenUv = B_GetViewportUv();
        rayOut = texture2D(B_CubeBackFacesColor, screenUv).rgb;

        rayIn  *= actualSizeRatio;
        rayOut *= actualSizeRatio;
    }
    vec3 rayDir = normalize(rayOut - rayIn);

    #ifdef BANG_FORWARD_RENDERING
    vec4 accumulatedTransferFunctionColor = vec4(0,0,0,0);
    vec4 accumulatedColor = vec4(0,0,0,0);
    #else
    vec3 worldPosition;
    vec3 normal;
    #endif

    bool draw = false;
    int numSamplesTaken = 0;
    float sampleStep = 1.0f / B_NumSamples;
    float rayLength = distance(rayIn, rayOut);
    float numSamples = rayLength / sampleStep;
    vec4 albedoColor = vec4(0,0,0,1);
    float numSteps = B_NumSamples;
    float currentSampleDist = 0.0f;
    vec3 voxelSize = (vec3(1.0f) / B_Texture3DPOTSize);
    vec3 currentSamplePoint = vec3(0);
    bool firstSurfaceFound = false;
    vec3 cubeMin = B_RenderCubeMin * actualSizeRatio;
    vec3 cubeMax = B_RenderCubeMax * actualSizeRatio;
    for (int i = 0; i < numSamples; ++i)
    {
        vec3 currentSamplePoint = (rayIn + rayDir * currentSampleDist);
        currentSampleDist += sampleStep;
        {
            if (currentSamplePoint.x < cubeMin.x || currentSamplePoint.x > cubeMax.x ||
                currentSamplePoint.y < cubeMin.y || currentSamplePoint.y > cubeMax.y ||
                currentSamplePoint.z < cubeMin.z || currentSamplePoint.z > cubeMax.z)
            {
                continue;
            }
        }

        #ifdef BANG_FORWARD_RENDERING
        float currentDensity = GetDensityAt(currentSamplePoint);
        vec4 currentTransferFunctionColor = texture2D(B_TransferFunctionTexture,
                                                      vec2(currentDensity, 0.5));


        // accumulatedTransferFunctionColor.rgb =
        //         (currentTransferFunctionColor.rgb * currentTransferFunctionColor.a) +
        //         (accumulatedTransferFunctionColor.rgb * (1.0f - currentTransferFunctionColor.a));
        // accumulatedTransferFunctionColor.a += currentTransferFunctionColor.a;

        // float stepAlpha = (1.0f - accumulatedTransferFunctionColor.a) * currentTransferFunctionColor.a;
        // accumulatedTransferFunctionColor.rgb += currentTransferFunctionColor.rgb *
        //                                         currentTransferFunctionColor.a;
        // accumulatedTransferFunctionColor.a += currentTransferFunctionColor.a;

        // Get voxel gradient
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

        // Light voxel if needed
        if (currentTransferFunctionColor.a > 0.1f) // && dot(gradient, gradient) > 0.05f) // && currentDensity > 0.1f)
        {
            if (!firstSurfaceFound)
            {
                vec3 worldPosition = (B_Model * vec4(B_FIn_ModelPosition +
                                                     (rayDir * currentSampleDist), 1)).xyz;
                vec4 worldPositionProj = (B_ProjectionView * vec4(worldPosition, 1));
                gl_FragDepth = (worldPositionProj.z / worldPositionProj.w) * 0.5f + 0.5f;
                firstSurfaceFound = true;
            }

            vec3 normal = normalize((B_Normal * vec4(normalize(gradient), 0))).xyz;

            vec4 albedoColor = vec4(currentTransferFunctionColor.rgb, 1);
            vec4 voxelColor = vec4(0, 0, 0, currentTransferFunctionColor.a);
            voxelColor.rgb = albedoColor.rgb;
            voxelColor.a = currentTransferFunctionColor.a * sampleStep * 15.0f * B_AlphaMultiply;
            // voxelColor.rgb /= voxelColor.a;

            float stepAlpha = (1.0f - accumulatedColor.a) * voxelColor.a;
            accumulatedColor.rgb += voxelColor.rgb * voxelColor.a;
            accumulatedColor.a += voxelColor.a;

            ++numSamplesTaken;
        }

        currentSampleDist += sampleStep;
        if (accumulatedColor.a >= 1.0f)
        {
            accumulatedColor.a = 1.0f;
            break;
        }

        #else

        float currentDensity = GetDensityAt(currentSamplePoint);
        if ( currentDensity >= B_DensityThreshold - B_SurfaceThickness &&
             currentDensity <= B_DensityThreshold + B_SurfaceThickness)
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
            normal = normalize((B_Normal * vec4(normalize(gradient), 0))).xyz;
            if (B_InvertNormals)
            {
                normal *= -1.0f;
            }

            albedoColor = B_MaterialAlbedoColor;
            break;
        }

        #endif
    }

    #ifdef BANG_FORWARD_RENDERING
    if (accumulatedColor.a < 0.01f)
    {
        discard;
    }

    B_GIn_Color = accumulatedColor;

    #else

    if (draw)
    {
        float receivesLighting = 1.0f;
        float pixelRoughness = B_MaterialRoughness;
        float pixelMetalness = B_MaterialMetalness;

        vec4 color = GetIBLAmbientColor(true,
                                        worldPosition, normal, albedoColor,
                                        pixelRoughness, pixelMetalness);

        B_GIn_Color = color;
        B_GIn_Albedo = albedoColor;
        B_GIn_Light = vec4(0);
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
    #endif
}
