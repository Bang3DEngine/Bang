#define BANG_FRAGMENT
#include "Common.glsl"

const int RIGHT_DIR_IDX = 0;
const int LEFT_DIR_IDX  = 1;
const int TOP_DIR_IDX   = 2;
const int BOT_DIR_IDX   = 3;
const int FRONT_DIR_IDX = 4;
const int BACK_DIR_IDX  = 5;

uniform sampler2D B_FillCMFromTexture_TopTexture;
uniform sampler2D B_FillCMFromTexture_BotTexture;
uniform sampler2D B_FillCMFromTexture_LeftTexture;
uniform sampler2D B_FillCMFromTexture_RightTexture;
uniform sampler2D B_FillCMFromTexture_BackTexture;
uniform sampler2D B_FillCMFromTexture_FrontTexture;

in TFIn
{
    float B_Face;
    vec2 B_FIn_Uv;
};

out vec4 B_Out_Color;

void main()
{
    vec2 uv = B_FIn_Uv;
    uv.y = 1.0 - uv.y;

    int face = int(B_Face);
    switch (face)
    {
        case RIGHT_DIR_IDX:
            B_Out_Color = texture(B_FillCMFromTexture_RightTexture, uv);
        break;

        case LEFT_DIR_IDX:
            B_Out_Color = texture(B_FillCMFromTexture_LeftTexture, uv);
        break;

        case TOP_DIR_IDX:
        {
            float prevUvX = uv.x;
            uv.x = 1.0 - uv.y;
            uv.y = prevUvX;
            B_Out_Color = texture(B_FillCMFromTexture_TopTexture, uv);
        }
        break;

        case BOT_DIR_IDX:
        {
            float prevUvX = uv.x;
            uv.x = 1.0 - uv.y;
            uv.y = prevUvX;
            B_Out_Color = texture(B_FillCMFromTexture_BotTexture, uv);
        }
        break;

        case BACK_DIR_IDX:
            B_Out_Color = texture(B_FillCMFromTexture_BackTexture, uv);
        break;

        case FRONT_DIR_IDX:
            B_Out_Color = texture(B_FillCMFromTexture_FrontTexture, uv);
        break;
    }
}
