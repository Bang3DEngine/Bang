#include "Bang/Serializable.h"

#include "Bang/Color.h"
#include "Bang/File.h"
#include "Bang/GUID.h"
#include "Bang/GUIDManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/ObjectPtr.h"
#include "Bang/Path.h"
#include "Bang/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

Serializable::Serializable()
{
    SetGUID(GUIDManager::GetNewGUID());
}

Serializable::~Serializable()
{
}

Serializable::Serializable(const Serializable &)
{
    // Don't copy GUID, intentionally left in blank
}

MetaNode Serializable::GetMeta() const
{
    MetaNode metaNode;
    ExportMeta(&metaNode);
    return metaNode;
}

String Serializable::GetSerializedString() const
{
    MetaNode metaNode;
    ExportMeta(&metaNode);
    return metaNode.ToString();
}

void Serializable::ImportMeta(const String &metaNodeString)
{
    MetaNode metaNode;
    metaNode.Import(metaNodeString);
    ImportMeta(metaNode);
}

void Serializable::ImportMeta(const MetaNode &metaNode)
{
    if (metaNode.Contains("GUID"))
    {
        SetGUID(metaNode.Get<GUID>("GUID"));
    }

    // Import reflected variables
    const ReflectStruct &reflStruct = GetReflectStruct();
    for (const ReflectVariable &reflVar : reflStruct.GetVariables())
    {
        const String &varName = reflVar.GetName();
        if (metaNode.Contains(varName))
        {
            if (auto reflVarSetter = reflVar.GetSetter())
            {
                switch (reflVar.GetVariant().GetType())
                {
                    case Variant::Type::FLOAT:
                        reflVarSetter(
                            Variant::FromFloat(metaNode.Get<float>(varName)));
                        break;
                    case Variant::Type::DOUBLE:
                        reflVarSetter(
                            Variant::FromDouble(metaNode.Get<double>(varName)));
                        break;
                    case Variant::Type::INT:
                        reflVarSetter(
                            Variant::FromInt(metaNode.Get<int>(varName)));
                        break;
                    case Variant::Type::UINT:
                        reflVarSetter(
                            Variant::FromUint(metaNode.Get<uint>(varName)));
                        break;
                    case Variant::Type::BOOL:
                        reflVarSetter(
                            Variant::FromBool(metaNode.Get<bool>(varName)));
                        break;
                    case Variant::Type::PATH:
                        reflVarSetter(
                            Variant::FromPath(metaNode.Get<Path>(varName)));
                        break;
                    case Variant::Type::STRING:
                        reflVarSetter(
                            Variant::FromString(metaNode.Get<String>(varName)));
                        break;
                    case Variant::Type::GUID:
                        reflVarSetter(
                            Variant::FromGUID(metaNode.Get<GUID>(varName)));
                        break;
                    case Variant::Type::COLOR:
                        reflVarSetter(
                            Variant::FromColor(metaNode.Get<Color>(varName)));
                        break;
                    case Variant::Type::VECTOR2:
                        reflVarSetter(Variant::FromVector2(
                            metaNode.Get<Vector2>(varName)));
                        break;
                    case Variant::Type::VECTOR3:
                        reflVarSetter(Variant::FromVector3(
                            metaNode.Get<Vector3>(varName)));
                        break;
                    case Variant::Type::VECTOR4:
                        reflVarSetter(Variant::FromVector4(
                            metaNode.Get<Vector4>(varName)));
                        break;
                    case Variant::Type::VECTOR2i:
                        reflVarSetter(Variant::FromVector2i(
                            metaNode.Get<Vector2i>(varName)));
                        break;
                    case Variant::Type::VECTOR3i:
                        reflVarSetter(Variant::FromVector3i(
                            metaNode.Get<Vector3i>(varName)));
                        break;
                    case Variant::Type::VECTOR4i:
                        reflVarSetter(Variant::FromVector4i(
                            metaNode.Get<Vector4i>(varName)));
                        break;
                    case Variant::Type::QUATERNION:
                        reflVarSetter(Variant::FromQuaternion(
                            metaNode.Get<Quaternion>(varName)));
                        break;
                    case Variant::Type::OBJECT_PTR:
                        reflVarSetter(Variant::FromObjectPtr(
                            metaNode.Get<ObjectPtr>(varName)));
                        break;
                    case Variant::Type::NONE: break;

                    default: ASSERT(false); break;
                }
            }
        }
    }
}

void Serializable::ExportMeta(MetaNode *metaNode) const
{
    metaNode->Set<GUID>("GUID", GetGUID());

    // Export reflected variables
    const ReflectStruct &reflStruct = GetReflectStruct();
    MetaNode reflMeta = reflStruct.GetMeta();
    metaNode->Import(reflMeta);

    metaNode->SetName(GetClassName());  // Set name after, to avoid overwrites
}

bool Serializable::ImportMetaFromFile(const Path &path)
{
    if (path.Exists())
    {
        String fileContents = File::GetContents(path);
        ImportMeta(fileContents);
        return true;
    }
    return false;
}

bool Serializable::ExportMetaToFile(const Path &path) const
{
    File::Write(path, GetSerializedString());
    return true;
}

HideFlags &Serializable::GetHideFlags()
{
    return m_hideFlags;
}

const HideFlags &Serializable::GetHideFlags() const
{
    return m_hideFlags;
}

void Serializable::CloneInto(ICloneable *cloneable, bool cloneGUID) const
{
    Serializable *clone = SCAST<Serializable *>(cloneable);

    // Clone reflected variables
    ReflectStruct thisReflStruct = GetReflectStruct();
    ReflectStruct cloneReflStruct = clone->GetReflectStruct();
    for (uint i = 0; i < thisReflStruct.GetVariables().Size(); ++i)
    {
        const ReflectVariable &thisReflVar = thisReflStruct.GetVariables()[i];
        const ReflectVariable &cloneReflVar = cloneReflStruct.GetVariables()[i];
        if (auto thisGetter = thisReflVar.GetGetter())
        {
            if (auto cloneSetter = cloneReflVar.GetSetter())
            {
                cloneSetter(thisGetter());
            }
        }
    }

    if (cloneGUID)
    {
        clone->SetGUID(GetGUID());
    }
}
