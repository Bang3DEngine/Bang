#ifndef AUDIOPARAMS_H
#define AUDIOPARAMS_H

#include "Bang/Vector3.h"

namespace Bang
{
struct AudioParams
{
    Vector3 position = Vector3::Zero();
    float volume = 1.0f;
    float delay = 0.0f;
    float pitch = 1.0f;
    float range = 1000.0f;
    bool looping = false;

    AudioParams(const Vector3 &_position = Vector3::Zero(),
                float _volume = 1.0f,
                float _delay = 0.0f,
                float _pitch = 1.0f,
                float _range = 1000.0f,
                bool _looping = false)
        : position(_position),
          volume(_volume),
          delay(_delay),
          pitch(_pitch),
          range(_range),
          looping(_looping)
    {
    }
};
}

#endif  // AUDIOPARAMS_H
