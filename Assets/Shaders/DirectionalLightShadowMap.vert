#define BANG_NO_MAIN
#define ONLY_OUT_MODEL_POS_VEC4
#include "DefaultVertCommon.glsl"

void main()
{
    vec4 modelPos = GetModelVertexPosition();
    vec4 projPosition = GetProjectedVertexPosition(modelPos);

    gl_Position = projPosition;
}
