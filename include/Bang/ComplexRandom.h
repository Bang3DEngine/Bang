#ifndef COMPLEXRANDOM_H
#define COMPLEXRANDOM_H

#include "Bang/BangDefines.h"

namespace Bang
{
enum class ComplexRandomType
{
    CONSTANT_VALUE,
    RANDOM_RANGE
};

class ComplexRandom
{
public:
    ComplexRandom();
    ComplexRandom(float constantValue);
    ComplexRandom(float minRangeValue, float maxRangeValue);
    ~ComplexRandom();

    float GenerateRandom() const;

    void SetConstantValue(float constantValue);
    void SetMinRangeValue(float minRangeValue);
    void SetMaxRangeValue(float maxRangeValue);
    void SetRangeValues(float minRangeValue, float maxRangeValue);
    void SetType(ComplexRandomType type);

    float GetConstantValue() const;
    float GetMinRangeValue() const;
    float GetMaxRangeValue() const;
    ComplexRandomType GetType() const;

    bool operator==(const ComplexRandom &rhs) const;
    bool operator!=(const ComplexRandom &rhs) const;

private:
    ComplexRandomType m_type = ComplexRandomType::RANDOM_RANGE;

    float m_constantValue = 1.0f;
    float m_minRangeValue = 0.0f;
    float m_maxRangeValue = 1.0f;
};
}

#endif  // COMPLEXRANDOM_H
