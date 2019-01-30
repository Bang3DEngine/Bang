#ifndef OCTREE_H
#define OCTREE_H

#include <array>
#include <functional>

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Bang.h"

namespace Bang
{
template <class T>
class Octree
{
public:
    Octree();
    ~Octree();

    void SetAABox(const AABox &aabox);
    uint Fill(const Array<T> &elements, uint maxDepth);

    using ClassifyFunction =
        std::function<bool(const AABox &nodeBox, const T &octreeElement)>;
    void SetClassifyFunction(
        std::function<bool(const AABox &nodeBox, const T &octreeElement)>
            classifyFunction);

    int GetDepth() const;
    const AABox GetAABox() const;
    const Array<T> &GetElements() const;
    Array<T> GetElementsRecursive() const;
    const std::array<Octree *, 8> &GetChildren() const;
    Array<const Octree *> GetChildrenAtLevel(
        uint level,
        bool includeEarlyPrunedInPreviousLevels) const;

private:
    Array<T> m_leafElements;  // Only filled if we are leaf node
    AABox m_aaBox;            // AABox of this octree
    ClassifyFunction m_classifyFunction;

    std::array<Octree *, 8> m_children;  // Octree children
};
}

#include "Bang/Octree.tcc"

#endif  // OCTREE_H
