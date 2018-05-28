#ifndef STACKANDVALUE_H
#define STACKANDVALUE_H

#include <stack>

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

template <class T>
struct StackAndValue
{
    T currentValue;
    std::stack<T> stack;
};

NAMESPACE_BANG_END

#endif // STACKANDVALUE_H
