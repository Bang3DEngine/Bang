#include "Bang/ComplexRandom.h"

#include "Bang/Random.h"

using namespace Bang;

ComplexRandom::ComplexRandom()
{
}

ComplexRandom::ComplexRandom(float constantValue)
{
    SetType(ComplexRandomType::CONSTANT_VALUE);
    SetConstantValue(constantValue);
}

ComplexRandom::ComplexRandom(float minRangeValue, float maxRangeValue)
{
    SetType(ComplexRandomType::RANDOM_RANGE);
    SetMinRangeValue(minRangeValue);
    SetMaxRangeValue(maxRangeValue);
}

ComplexRandom::~ComplexRandom()
{
}

void ComplexRandom::SetType(ComplexRandomType type)
{
    m_type = type;
}

void ComplexRandom::SetConstantValue(float constantValue)
{
    m_constantValue = constantValue;
}

void ComplexRandom::SetMinRangeValue(float minRangeValue)
{
    m_minRangeValue = minRangeValue;
}

void ComplexRandom::SetMaxRangeValue(float maxRangeValue)
{
    m_maxRangeValue = maxRangeValue;
}

void ComplexRandom::SetRangeValues(float minRangeValue, float maxRangeValue)
{
    SetMinRangeValue(minRangeValue);
    SetMaxRangeValue(maxRangeValue);
}

float ComplexRandom::GetConstantValue() const
{
    return m_constantValue;
}

float ComplexRandom::GetMinRangeValue() const
{
    return m_minRangeValue;
}

float ComplexRandom::GetMaxRangeValue() const
{
    return m_maxRangeValue;
}

float ComplexRandom::GenerateRandom() const
{
    switch (GetType())
    {
        case ComplexRandomType::CONSTANT_VALUE:
            return GetConstantValue();
            break;

        case ComplexRandomType::RANDOM_RANGE:
            return Random::GetRange(GetMinRangeValue(), GetMaxRangeValue());
            break;
    }

    return -1.0f;
}

ComplexRandomType ComplexRandom::GetType() const
{
    return m_type;
}

bool ComplexRandom::operator==(const ComplexRandom &rhs) const
{
    return GetType() == rhs.GetType() &&
           GetConstantValue() == rhs.GetConstantValue() &&
           GetMinRangeValue() == rhs.GetMinRangeValue() &&
           GetMaxRangeValue() == rhs.GetMaxRangeValue();
}

bool ComplexRandom::operator!=(const ComplexRandom &rhs) const
{
    return !(*this == rhs);
}
