#ifndef KEY_H
#define KEY_H

#include "Bang/Bang.h"
#include "Bang/Flags.h"

#include BANG_SDL2_INCLUDE(SDL.h)
#include <SDL2/SDL.h>

NAMESPACE_BANG_BEGIN

enum class Key
{
    NONE   = 0,
    RETURN = SDLK_RETURN, ENTER = SDLK_RETURN, INSERT = SDLK_INSERT,
    DELETE = SDLK_DELETE, PAUSE = SDLK_PAUSE, PRINT = SDLK_PRINTSCREEN,
    CLEAR = SDLK_CLEAR, HOME = SDLK_HOME,
    END = SDLK_END, LEFT = SDLK_LEFT, UP = SDLK_UP,
    RIGHT = SDLK_RIGHT, DOWN = SDLK_DOWN, PAGEUP = SDLK_PAGEUP,
    PAGEDOWN = SDLK_PAGEDOWN, LSHIFT = SDLK_LSHIFT, RSHIFT = SDLK_RSHIFT,
    LCTRL = SDLK_LCTRL, RCTRL = SDLK_RCTRL,
    LALT = SDLK_LALT, RALT = SDLK_RALT,
    TAB = SDLK_TAB, TABKP = SDLK_KP_TAB,
    CAPSLOCK = SDLK_CAPSLOCK, NUMLOCK = SDLK_NUMLOCKCLEAR,
    SCROLLLOCK = SDLK_SCROLLLOCK,
    F1 = SDLK_F1, F2 = SDLK_F2, F3 = SDLK_F3,
    F4 = SDLK_F4, F5 = SDLK_F5, F6 = SDLK_F6,
    F7 = SDLK_F7, F8 = SDLK_F8, F9 = SDLK_F9,
    F10 = SDLK_F10, F11 = SDLK_F11, F12 = SDLK_F12,
    F13 = SDLK_F13, F14 = SDLK_F14, F15 = SDLK_F15,
    F16 = SDLK_F16, F17 = SDLK_F17, F18 = SDLK_F18,
    F19 = SDLK_F19, F20 = SDLK_F20, F21 = SDLK_F21,
    F22 = SDLK_F22, F23 = SDLK_F23, F24 = SDLK_F24,
    LSUPER = SDLK_LGUI, RSUPER = SDLK_RGUI, MENU = SDLK_MENU,
    HELP = SDLK_HELP, SPACE = SDLK_SPACE,
    EXCLAM = SDLK_EXCLAIM, QUOTEDBL = SDLK_QUOTEDBL,
    NUMBERSIGN = SDLK_LESS,
    DOLLAR = SDLK_DOLLAR, PERCENT = SDLK_PERCENT,
    AMPERSAND = SDLK_AMPERSAND,
    PARENLEFT = SDLK_LEFTPAREN, PARENRIGHT = SDLK_RIGHTPAREN,
    ASTERISK = SDLK_ASTERISK, PLUS = SDLK_PLUS, COMMA = SDLK_COMMA,
    MINUS = SDLK_MINUS, PERIOD = SDLK_PERIOD, SLASH = SDLK_SLASH,
    NUM0 = SDLK_0, NUM1 = SDLK_1, NUM2 = SDLK_2, NUM3 = SDLK_3,
    NUM4 = SDLK_4, NUM5 = SDLK_5, NUM6 = SDLK_6, NUM7 = SDLK_7,
    NUM8 = SDLK_8, NUM9 = SDLK_9, COLON = SDLK_COLON,
    SEMICOLON = SDLK_SEMICOLON,
    LESS = SDLK_LESS, EQUAL = SDLK_EQUALS, GREATER = SDLK_GREATER,
    QUESTION = SDLK_QUESTION,
    AT = SDLK_AT,
    A = SDLK_a, B = SDLK_b, C = SDLK_c, D = SDLK_d, E = SDLK_e,
    F = SDLK_f, G = SDLK_g, H = SDLK_h, I = SDLK_i, J = SDLK_j, K = SDLK_k,
    L = SDLK_l, M = SDLK_m, N = SDLK_n, O = SDLK_o, P = SDLK_p, Q = SDLK_q,
    R = SDLK_r, S = SDLK_s, T = SDLK_t, U = SDLK_u, V = SDLK_v, W = SDLK_w,
    X = SDLK_x, Y = SDLK_y, Z = SDLK_z,
    LBRACKET = SDLK_LEFTBRACKET, BACKSLASH = SDLK_BACKSLASH,
    RBRACKET = SDLK_RIGHTBRACKET,
    UNDERSCORE = SDLK_UNDERSCORE, QUOTELEFT = SDLK_QUOTE,
    LPAREN = SDLK_LEFTPAREN, RPAREN = SDLK_RIGHTPAREN,
    BACKSPACE = SDLK_BACKSPACE
};

enum class KeyModifier
{
    NONE    = SDL_Keymod::KMOD_NONE,
    LCTRL   = SDL_Keymod::KMOD_LCTRL,
    RCTRL   = SDL_Keymod::KMOD_RCTRL,
    LALT    = SDL_Keymod::KMOD_LALT,
    RALT    = SDL_Keymod::KMOD_RALT,
    LSHIFT  = SDL_Keymod::KMOD_LSHIFT,
    RSHIFT  = SDL_Keymod::KMOD_RSHIFT,
    LGUI    = SDL_Keymod::KMOD_LGUI,
    RGUI    = SDL_Keymod::KMOD_RGUI,
    CAPS    = SDL_Keymod::KMOD_CAPS,
    NUM     = SDL_Keymod::KMOD_NUM,
    MODE    = SDL_Keymod::KMOD_MODE,
    DEFAULT = NONE
};
CREATE_FLAGS(KeyModifiers, KeyModifier);

NAMESPACE_BANG_END

#endif // KEY_H

