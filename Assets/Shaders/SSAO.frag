#include "PP.frag"

#define MAX_RANDOM_OFFSETS 128

uniform float B_SSAORadius    = 1.0f;
uniform float B_SSAOIntensity = 1.0f;
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
    vec3 viewPos  = (B_View * vec4(worldPos, 1)).xyz;

    vec2 vpUv = B_GetViewportUv(); // Get viewport uvs

    // Get random rotation vector
    vec3 randomAxes = texture(B_RandomAxes,
                              vpUv * B_RandomAxesUvMultiply).xyz;
    randomAxes.xyz = (randomAxes.xyz * 2.0f - 1.0f); // Map (0,1) to (-1,1)

    // Create basis to orient offset to normal, and rotated with randomAxes
    // Tangent facing normal hemisphere
    vec3 tangent = normalize( cross(randomAxes, normal) );
    vec3 bitangent = normalize( cross(normal, tangent) ); // Bitangent by cross
    mat3 rotateToNormal = mat3(tangent, bitangent, normal); // Basis to rotate

    float occlusionSum   = 0.0;
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
        float gbufferDepth = ( B_SampleDepth(gbufferDepthUv) );

        // Check if the sampled point is inside the sphere
        vec3 gbufferWorldPos = B_ComputeWorldPosition(gbufferDepth, gbufferDepthUv);
        vec3 sampleVsGBufferSampleDist = (sampleWorldPos - gbufferWorldPos);
        float sqDistSampleVsGBufferSample = dot(sampleVsGBufferSampleDist,
                                                sampleVsGBufferSampleDist);
        float isInsideSphere = 1.0 - smoothstep(0.0, SSAORadiusSQ,
                                                sqDistSampleVsGBufferSample); // ( sqDistSampleVsGBufferSample <= SSAORadiusSQ ) ? 1.0 : 0.0;

        // Multiply occlusion by the dot of the normals (if they are very parallel,
        // then the occlusion will be very low
        vec3 sampleNormal = B_SampleNormal(gbufferDepthUv);
        float normalApport = max(0.0, dot(sampleNormal, normal));

        // Finally determine if we are occluded or not for this sample
        float sampleWorldPosDepth = projectedPos.z;
        float deltaDepth = (sampleWorldPosDepth - gbufferDepth);
        float Bias       = 0.0001 / (depth*depth); // 0.000001;
        float occluded   = step(Bias, deltaDepth); // smoothstep(Bias, 1.0, deltaDepth); // step(Bias, deltaDepth);
        float occlusionApport = occluded * normalApport * isInsideSphere;

        occlusionSum   += occlusionApport; // Accumulate occlusion
    }
    float occlusion = (occlusionSum / B_NumRandomOffsets);
    occlusion *= B_SSAOIntensity; // pow(occlusion, 1.0/B_SSAOIntensity);

    B_GIn_Color = vec4( vec3(occlusion), 1 );
}
