#include "Bang/Serializable.h"

#include "Bang/Color.h"
#include "Bang/File.h"
#include "Bang/GUID.h"
#include "Bang/GUIDManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
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
    ReflectStruct reflStruct = GetReflectionInfo();
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
                    case Variant::Type::BOOL:
                        reflVarSetter(
                            Variant::FromBool(metaNode.Get<bool>(varName)));
                        break;
                    case Variant::Type::STRING:
                        reflVarSetter(
                            Variant::FromString(metaNode.Get<String>(varName)));
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
                    case Variant::Type::QUATERNION:
                        reflVarSetter(Variant::FromQuaternion(
                            metaNode.Get<Quaternion>(varName)));
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
    metaNode->SetName(GetClassName());
    metaNode->Set<GUID>("GUID", GetGUID());

    // Export reflected variables
    ReflectStruct reflStruct = GetReflectionInfo();
    for (const ReflectVariable &reflVar : reflStruct.GetVariables())
    {
        if (auto reflVarGetter = reflVar.GetGetter())
        {
            metaNode->Set(reflVar.GetName(), reflVarGetter());
        }
    }
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

void Serializable::CloneInto(ICloneable *cloneable) const
{
    Serializable *clone = SCAST<Serializable *>(cloneable);

    // Clone reflected variables
    ReflectStruct thisReflStruct = GetReflectionInfo();
    ReflectStruct cloneReflStruct = clone->GetReflectionInfo();
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
}
