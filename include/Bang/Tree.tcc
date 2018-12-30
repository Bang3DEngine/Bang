#pragma once

#include "Bang/Tree.h"

namespace Bang
{
template <class T>
Tree<T>::Tree()
{
}

template <class T>
Tree<T>::~Tree()
{
    SetParent(nullptr);
    Clear();
}

template <class T>
Tree<T> *Tree<T>::AddChild()
{
    return AddChild(T(), GetChildren().Size());
}

template <class T>
Tree<T> *Tree<T>::AddChild(const T &data)
{
    return AddChild(data, GetChildren().Size());
}

template <class T>
Tree<T> *Tree<T>::AddChild(const T &data, int index)
{
    Tree<T> *childTree = new Tree<T>();
    childTree->SetParent(this, index);
    childTree->SetData(data);
    return childTree;
}

template <class T>
void Tree<T>::SetData(const T &data)
{
    m_data = data;
}

template <class T>
void Tree<T>::SetParent(Tree<T> *parentTree)
{
    if (parentTree)
    {
        SetParent(parentTree, parentTree->GetChildren().Size());
    }
    else
    {
        SetParent(nullptr, -1);
    }
}

template <class T>
void Tree<T>::SetParent(Tree<T> *parentTree, int index)
{
    if (GetParent() != parentTree)
    {
        if (GetParent())
        {
            GetParent()->m_subTrees.Remove(this);
        }

        p_parent = parentTree;
        if (GetParent())
        {
            GetParent()->m_subTrees.Insert(this, index);
        }
    }
    else if (GetParent())  // Position change inside same parent
    {
        int oldIndex = GetParent()->GetChildren().IndexOf(this);
        ASSERT(oldIndex >= 0);
        if (oldIndex != index)
        {
            int newIndex = (oldIndex < index) ? (index - 1) : index;
            GetParent()->GetChildren().Remove(this);
            GetParent()->GetChildren().Insert(this, newIndex);
        }
    }
}

template <class T>
void Tree<T>::Clear()
{
    while (!m_subTrees.IsEmpty())
    {
        Tree<T> *child = m_subTrees.Back();
        m_subTrees.PopBack();
        delete child;
    }
}

template <class T>
int Tree<T>::GetDepth() const
{
    return GetParent() ? (GetParent()->GetDepth() + 1) : 0;
}

template <class T>
List<Tree<T> *> &Tree<T>::GetChildren()
{
    return m_subTrees;
}

template <class T>
const List<Tree<T> *> &Tree<T>::GetChildren() const
{
    return m_subTrees;
}

template <class T>
List<Tree<T> *> Tree<T>::GetChildrenRecursive() const
{
    List<Tree<T> *> result = GetChildren();
    for (Tree<T> *child : GetChildren())
    {
        result.PushBack(child->GetChildrenRecursive());
    }
    return result;
}

template <class T>
Tree<T> *Tree<T>::GetParent() const
{
    return p_parent;
}

template <class T>
T &Tree<T>::GetData()
{
    return m_data;
}

template <class T>
const T &Tree<T>::GetData() const
{
    return m_data;
}

template <class T>
Tree<T> *Tree<T>::GetDeepCopy() const
{
    Tree<T> *cpy = new Tree<T>();
    cpy->SetData(m_data);
    for (const Tree<T> *child : GetChildren())
    {
        Tree<T> *childCpy = child->GetDeepCopy();
        childCpy->SetParent(cpy, 0);
    }
    return cpy;
}
}
