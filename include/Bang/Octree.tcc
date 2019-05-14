#pragma once

#include <array>
#include <queue>

#include "Bang/Debug.h"
#include "Bang/Octree.h"
#include "BangMath/Vector3.h"

using namespace Bang;

template <class T>
Octree<T>::Octree()
{
    for (int i = 0; i < 8; ++i)
    {
        m_children[i] = nullptr;
    }
}

template <class T>
Octree<T>::~Octree()
{
    for (int i = 0; i < 8; ++i)
    {
        Octree *childOct = m_children[i];
        if (childOct)
        {
            delete childOct;
        }
    }
}

template <class T>
void Octree<T>::SetAABox(const AABox &aabox)
{
    m_aaBox = aabox;
    m_leafElements.Clear();
}

#include "DebugRenderer.h"
template <class T>
uint Octree<T>::Fill(const Array<T> &elements, uint maxDepth)
{
    // Returns the number of contained elements after filling
    if (!m_classifyFunction || maxDepth < 0)
    {
        return -1;
    }

    // Get elements inside me
    Array<T> containedElements;
    for (const T &element : elements)
    {
        if (m_classifyFunction(GetAABox(), element))
        {
            // Debug_Peek(element);
            containedElements.PushBack(element);
        }
    }

    if (maxDepth > 0)
    {
        // Keep subdividing.
        if (containedElements.Size() > 1)
        {
            // Create and fill children if we contain more than one element.
            const Vector3 minPoint = GetAABox().GetMin();
            const Vector3 size = GetAABox().GetSize();
            const Vector3 &mp = minPoint;
            const Vector3 hs = size / 2.0f;

            std::array<Vector3, 8> sizeDirs = {{Vector3(0, 0, 0),
                                                Vector3(0, 0, 1),
                                                Vector3(0, 1, 0),
                                                Vector3(0, 1, 1),
                                                Vector3(1, 0, 0),
                                                Vector3(1, 0, 1),
                                                Vector3(1, 1, 0),
                                                Vector3(1, 1, 1)}};
            for (int i = 0; i < 8; ++i)
            {
                const Vector3 &sizeDir = sizeDirs[i];

                Octree *childOctree = new Octree<T>();
                childOctree->SetAABox(
                    AABox::FromPointAndSize(mp + hs * sizeDir, hs));
                int childContainedElements =
                    childOctree->Fill(containedElements, maxDepth - 1);

                if (childContainedElements > 0)
                {
                    m_children[i] = childOctree;
                }
                else
                {
                    delete childOctree;
                }
            }
        }
        else if (containedElements.Size() == 1)
        {
            // Otherwise, we only have one element. Add element and dont
            // keep subdividing.
            m_leafElements.PushBack(containedElements.Front());
        }
    }
    else
    {
        // We arrived to the octree max depth. Just store all the remaining
        // elements in this node
        m_leafElements.PushBack(containedElements);
    }

    return containedElements.Size();
}

template <class T>
void Octree<T>::SetClassifyFunction(ClassifyFunction classifyFunction)
{
    m_classifyFunction = classifyFunction;
}

template <class T>
int Octree<T>::GetDepth() const
{
    int childrenMaxDepth = 0;
    for (Octree *oct : GetChildren())
    {
        if (oct)
        {
            childrenMaxDepth = Math::Max(childrenMaxDepth, oct->GetDepth());
        }
    }
    return childrenMaxDepth + 1;
}
template <class T>
const AABox Octree<T>::GetAABox() const
{
    return m_aaBox;
}

template <class T>
Array<T> Octree<T>::GetElementsRecursive() const
{
    Array<T> elements = GetElements();
    for (const Octree *child : GetChildren())
    {
        if (child)
        {
            elements.PushBack(child->GetElementsRecursive());
        }
    }
    return elements;
}

template <class T>
const Array<T> &Octree<T>::GetElements() const
{
    return m_leafElements;
}

template <class T>
const std::array<Octree<T> *, 8> &Octree<T>::GetChildren() const
{
    return m_children;
}

template <class T>
Array<const Octree<T> *> Octree<T>::GetChildrenAtLevel(
    uint level,
    bool includeEarlyPrunedInPreviousLevels) const
{
    using OctLevelPair = std::pair<uint, const Octree *>;

    Array<const Octree *> childrenAtLevelResult;

    std::queue<OctLevelPair> queuedOctLevelPairs;
    queuedOctLevelPairs.push(std::make_pair(0, this));

    while (!queuedOctLevelPairs.empty())
    {
        const OctLevelPair &currentOctLevelPair = queuedOctLevelPairs.front();
        const Octree *currentOctree = currentOctLevelPair.second;
        uint currentLevel = currentOctLevelPair.first;
        queuedOctLevelPairs.pop();

        if (currentLevel == level)
        {
            childrenAtLevelResult.PushBack(currentOctree);
        }
        else
        {
            if (includeEarlyPrunedInPreviousLevels &&
                currentOctree->GetElements().Size() == 1)
            {
                childrenAtLevelResult.PushBack(currentOctree);
            }

            for (const Octree *currentChild : currentOctree->GetChildren())
            {
                if (currentChild)
                {
                    queuedOctLevelPairs.push(
                        std::make_pair(currentLevel + 1, currentChild));
                }
            }
        }
    }
    return childrenAtLevelResult;
}
