#ifndef STACKANDVALUE_H
#define STACKANDVALUE_H

#include <stack>

#include "Bang/Bang.h"

namespace Bang
{
template <class T>
struct StackAndValue
{
    T currentValue;
    std::stack<T> stack;
};
}

#endif  // STACKANDVALUE_H
