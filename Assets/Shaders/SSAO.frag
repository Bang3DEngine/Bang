#include "ScreenPass.frag"

#define MAX_RANDOM_OFFSETS 128

uniform float B_SSAORadius    = 1.0f;
uniform vec2 B_RandomAxesUvMultiply;
uniform sampler2D B_RandomAxes;
uniform int B_NumRandomOffsets;
uniform vec3 B_RandomHemisphereOffsetsArray[MAX_RANDOM_OFFSETS];

void main()
{
    float depth   = B_SampleDepth();
    if (depth > 0.99999f) { B_GIn_Color = vec4( vec3(0), 1 ); return; }

    vec3 normal   = normalize( B_SampleNormal() );
    vec3 worldPos = B_ComputeWorldPosition(depth);

    // Get random rotation vector
    vec2 vpUv = B_GetViewportUv(); // Get viewport uv
    vec3 randomAxes = texture(B_RandomAxes, vpUv * B_RandomAxesUvMultiply).xyz;
    randomAxes.xyz = (randomAxes.xyz * 2.0f - 1.0f); // Map (0,1) to (-1,1)

    // Create basis to orient offset to normal, and rotated with randomAxes
    // Tangent facing normal hemisphere
    vec3 tangent = normalize( cross(randomAxes, normal) );
    vec3 bitangent = normalize( cross(normal, tangent) ); // Bitangent by cross
    mat3 rotateToNormal = mat3(tangent, bitangent, normal); // Basis to rotate

    float occlusionSum      = 0.0;
    float totalOcclusionSum = 0.0;
    float SSAORadius     = B_SSAORadius * (1.0f + 1.0f / (depth*depth));
    float SSAORadiusSQ   = (SSAORadius * SSAORadius);
    for (int i = 0; i < B_NumRandomOffsets; ++i) // Sample around!
    {
        // Get random hemisphere offset
        vec3 randomOffset = B_RandomHemisphereOffsetsArray[i];
        randomOffset = rotateToNormal * randomOffset;
        randomOffset = randomOffset * SSAORadius;
        vec3 sampleWorldPos = worldPos + randomOffset;

        // Get depth in depth buffer
        vec4 projectedPos = B_ProjectionView * vec4(sampleWorldPos, 1);
        projectedPos.xyz /= projectedPos.w;
        projectedPos.xyz = projectedPos.xyz * 0.5 + 0.5;
        vec2 gbufferDepthUv = projectedPos.xy;
        float gbufferDepth = B_SampleDepth(gbufferDepthUv);

        // Check if the sampled point is inside the sphere
        vec3 gbufferWorldPos = B_ComputeWorldPosition(gbufferDepth, gbufferDepthUv);
        vec3 sampleVsGBufferSampleDist = (sampleWorldPos - gbufferWorldPos);
        float sqDistSampleVsGBufferSample = dot(sampleVsGBufferSampleDist,
                                                sampleVsGBufferSampleDist);
        float isInsideSphere = 1.0 - step(SSAORadiusSQ,
                                          sqDistSampleVsGBufferSample);

        // Multiply occlusion by the dot of the normals (if they are very parallel,
        // then the occlusion will be very low
        vec3 sampleNormal = B_SampleNormal(gbufferDepthUv);
        float normalApport = dot(sampleNormal, normal) * 0.5f + 0.5f;

        // Finally determine if we are occluded or not for this sample
        float sampleWorldPosDepth = projectedPos.z;
        float deltaDepth = (sampleWorldPosDepth - gbufferDepth);
        float Bias       = 0.00001; // 0.000001;
        float occluded   = step(Bias, deltaDepth);
        float distanceFactor = dot(randomOffset, randomOffset);
        distanceFactor = ( (1.0 / pow((distanceFactor + 0.1), 3) ) ) / 2.0;
        float occlusionApport = abs(occluded * distanceFactor * normalApport * isInsideSphere);

        occlusionSum += occlusionApport; // Accumulate occlusion
        totalOcclusionSum += distanceFactor;
    }

    float occlusion = 0.0f;
    if (totalOcclusionSum > 0.00001)
    {
        occlusion = occlusionSum;
        // occlusion = (occlusionSum / totalOcclusionSum);
    }

    B_GIn_Color = vec4( vec3(occlusion), 1 );
}

