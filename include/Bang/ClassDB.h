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

    static ClassIdType GetClassIdBegin(const String &className);
    static ClassIdType GetClassIdEnd(const String &className);

    void RegisterClasses();

    static ClassDB *GetInstance();

    CREATE_STATIC_CLASS_IDS()

private:
    Map<String, ClassIdType> m_classNameToClassIdBegin;
    Map<String, ClassIdType> m_classNameToClassIdEnd;
};
}

#endif  // CLASSDB_H
