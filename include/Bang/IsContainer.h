#ifndef EVENTPROPAGATION_H
#define EVENTPROPAGATION_H

#include <list>
#include <map>
#include <set>
#include <type_traits>
#include <vector>

#include "Bang/Array.h"
#include "Bang/List.h"
#include "Bang/Map.h"
#include "Bang/Set.h"
#include "Bang/UMap.h"
#include "Bang/USet.h"

template <typename T>
struct IsContainer : std::false_type
{
};
template <typename... Args>
struct IsContainer<std::map<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<std::set<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<std::list<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<std::vector<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::Map<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::UMap<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::Set<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::USet<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::List<Args...>> : std::true_type
{
};
template <typename... Args>
struct IsContainer<Bang::Array<Args...>> : std::true_type
{
};

#endif  // EVENTPROPAGATION_H
