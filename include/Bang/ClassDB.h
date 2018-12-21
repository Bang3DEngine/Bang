#ifndef CLASSDB_H
#define CLASSDB_H

#include "Bang/Bang.h"
#include "Bang/ClassDBMacros.h"
#include "Bang/Map.h"
#include "Bang/String.h"

namespace Bang
{
class ClassDB
{
public:
    ClassDB();
    virtual ~ClassDB();

    static constexpr inline ClassIdType GetInvalidClassId();
    static ClassIdType GetClassIdBegin(const String &className);
    static ClassIdType GetClassIdEnd(const String &className);

    static constexpr inline bool IsSubClassByIds(ClassIdType baseClassIdBegin,
                                                 ClassIdType baseClassIdEnd,
                                                 ClassIdType subClassId);

    template <class TSubClass>
    static constexpr inline bool IsSubClass(ClassIdType baseClassIdBegin,
                                            ClassIdType baseClassIdEnd,
                                            const TSubClass *obj);

    template <class TBaseClass, class TSubClass>
    static constexpr inline bool IsSubClass(const TSubClass *obj);

    template <class T>
    static T *Create(const String &className);

    static void *Create(const String &className);

    void RegisterClasses();

    static ClassDB *GetInstance();

    CREATE_STATIC_CLASS_IDS()

private:
    Map<String, ClassIdType> m_classNameToClassIdBegin;
    Map<String, ClassIdType> m_classNameToClassIdEnd;
    Map<String, std::function<void *()>> m_classNameToConstructor;
};
}

#include "Bang/ClassDB.tcc"

#endif  // CLASSDB_H
