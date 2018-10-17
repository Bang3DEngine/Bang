#include "Bang/MetaFilesManager.h"

#include <vector>

#include "Bang/Application.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/File.h"
#include "Bang/Map.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Resource.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/USet.h"
#include "Bang/USet.tcc"

using namespace Bang;

MetaFilesManager::MetaFilesManager()
{
}

MetaFilesManager::~MetaFilesManager()
{
}

void MetaFilesManager::CreateMissingMetaFiles(const Path &directory)
{
    // First load existing meta files, to avoid creating new meta files
    // with duplicated GUIDs.
    MetaFilesManager::LoadMetaFilepathGUIDs(directory);

    Array<Path> assetFiles = directory.GetFiles(FindFlag::RECURSIVE);

    USet<Path> files;
    files.Add(assetFiles.Begin(), assetFiles.End());

    for (const Path &filepath : files)
    {
        if (!IsMetaFile(filepath) && !HasMetaFile(filepath))
        {
            MetaFilesManager::CreateMetaFileIfMissing(filepath);
        }
    }
}

void MetaFilesManager::LoadMetaFilepathGUIDs(const Path &directory)
{
    Array<String> extensions = {GetMetaExtension()};
    Array<Path> metaFilepaths =
        directory.GetFiles(FindFlag::RECURSIVE_HIDDEN, extensions);

    // Remove alone .meta files
    for (const Path &metaFilepath : metaFilepaths)
    {
        if (IsMetaFile(metaFilepath) && !GetFilepath(metaFilepath).IsFile())
        {
            File::Remove(metaFilepath);
        }
    }

    // Load GUID's of meta files!
    for (const Path &metaFilepath : metaFilepaths)
    {
        RegisterMetaFilepath(metaFilepath);
    }
}

void MetaFilesManager::OnPathAdded(const Path &filepath)
{
    if (!MetaFilesManager::IsMetaFile(filepath))
    {
        std::pair<Path, GUID> pathToGUID = CreateMetaFileIfMissing(filepath);
        RegisterMetaFilepath(MetaFilesManager::GetMetaFilepath(filepath));
    }
}

std::pair<Path, GUID> MetaFilesManager::CreateMetaFileIfMissing(
    const Path &filepath)
{
    Path metaFilepath = GetMetaFilepath(filepath);
    GUID newGUID = GUID::Empty();
    if (!IsMetaFile(filepath) && !HasMetaFile(filepath))
    {
        MetaNode metaNode;
        newGUID = GUIDManager::GetNewGUID();
        metaNode.Set("GUID", newGUID);
        File::Write(metaFilepath, metaNode.ToString());
        MetaFilesManager::RegisterMetaFilepath(metaFilepath);
    }
    else
    {
        newGUID = GetGUID(filepath);
    }
    return std::make_pair(metaFilepath, newGUID);
}

bool MetaFilesManager::HasMetaFile(const Path &filepath)
{
    return GetMetaFilepath(filepath).Exists();
}

bool MetaFilesManager::IsMetaFile(const Path &filepath)
{
    return filepath.IsHiddenFile() && filepath.HasExtension(GetMetaExtension());
}

void MetaFilesManager::DuplicateMetaFile(const Path &filepath,
                                         const Path &dupFilepath)
{
    const Path dupMetaFilepath = GetMetaFilepath(dupFilepath);
    File::Remove(dupMetaFilepath);

    const GUID &newGUID = CreateMetaFileIfMissing(dupFilepath).second;
    const Path originalMetaFilepath = GetMetaFilepath(filepath);
    MetaNode originalMetaNode;
    originalMetaNode.Import(originalMetaFilepath);
    originalMetaNode.Set("GUID", newGUID);

    File::Write(dupMetaFilepath, originalMetaNode.ToString());
    RegisterMetaFilepath(dupMetaFilepath);
}

GUIDManager *MetaFilesManager::GetGUIDManager()
{
    return &(MetaFilesManager::GetInstance()->m_GUIDManager);
}

void MetaFilesManager::RegisterMetaFilepath(const Path &metaFilepath)
{
    if (IsMetaFile(metaFilepath))
    {
        MetaNode metaNode;
        metaNode.Import(metaFilepath);

        Path filepath = GetFilepath(metaFilepath);
        GUID guid = metaNode.Get<GUID>("GUID");
        if (!guid.IsEmpty())
        {
            MetaFilesManager *mfm = MetaFilesManager::GetInstance();
            // if (mfm->m_GUIDToFilepath.ContainsKey(guid) &&
            //     mfm->m_GUIDToFilepath.Get(guid) != filepath)
            // {
            //     Debug_Error("Found conflicting GUID: " << guid <<
            //                 "(Files '" << filepath << "' and '" <<
            //                 mfm->m_GUIDToFilepath.Get(guid) << "'");
            // }

            mfm->m_GUIDToFilepath.Add(guid, filepath);
            mfm->m_filepathToGUID.Add(filepath, guid);
        }
    }
}

void MetaFilesManager::UnRegisterMetaFilepath(const Path &metaFilepath)
{
    MetaFilesManager *mfm = MetaFilesManager::GetInstance();
    Path filepath = GetFilepath(metaFilepath);
    mfm->m_GUIDToFilepath.RemoveValues(filepath);
    mfm->m_filepathToGUID.Remove(filepath);
}

GUID MetaFilesManager::GetGUID(const Path &filepath)
{
    MetaFilesManager *mfm = MetaFilesManager::GetInstance();
    if (mfm->m_filepathToGUID.ContainsKey(filepath))
    {
        return mfm->m_filepathToGUID.Get(filepath);
    }
    else
    {
        if (!Resources::IsEmbeddedResource(filepath))
        {
            Path metaFilepath = GetMetaFilepath(filepath);
            if (metaFilepath.IsFile())
            {
                MetaNode metaNode;
                metaNode.Import(metaFilepath);
                GUID guid = metaNode.Get<GUID>("GUID");
                mfm->m_filepathToGUID.Add(filepath, guid);
                return guid;
            }
        }
        else
        {
            Path parentResPath = filepath.GetDirectory();
            Resource *parentRes = Resources::GetCached(parentResPath);
            if (parentRes)
            {
                if (Resource *embeddedRes =
                        parentRes->GetEmbeddedResource(filepath.GetNameExt()))
                {
                    return embeddedRes->GetGUID();
                }
            }
        }
    }
    return GUID::Empty();
}

Path MetaFilesManager::GetFilepath(const GUID &guid)
{
    MetaFilesManager *mfm = MetaFilesManager::GetInstance();
    if (mfm->m_GUIDToFilepath.ContainsKey(guid))
    {
        const Path &path = mfm->m_GUIDToFilepath.Get(guid);
        return path;
    }
    else
    {
        if (Resources::IsEmbeddedResource(guid))
        {
            Path parentPath = MetaFilesManager::GetFilepath(
                guid.WithoutEmbeddedResourceGUID());
            if (parentPath.IsFile())
            {
                RH<Resource> resRH = Resources::LoadFromExtension(parentPath);
                if (resRH)
                {
                    String name = resRH.Get()->GetEmbeddedResourceName(
                        guid.GetEmbeddedResourceGUID());
                    Path path = parentPath.Append(name);
                    mfm->m_GUIDToFilepath.Add(guid, path);
                    return path;
                }
            }
        }
    }
    return Path::Empty;
}

Path MetaFilesManager::GetFilepath(const Path &metaFilepath)
{
    Path filepath = metaFilepath.WithHidden(false);

    String strPath = filepath.GetAbsolute();
    if (strPath.BeginsWith("."))
    {
        strPath.Remove(0, 1);
    }

    String ending = "." + GetMetaExtension();
    if (strPath.EndsWith(ending))
    {
        strPath.Remove(strPath.Size() - ending.Size(), strPath.Size());
    }

    return Path(strPath);
}

Path MetaFilesManager::GetMetaFilepath(const Path &filepath)
{
    return filepath.AppendExtension(MetaFilesManager::GetMetaExtension())
        .WithHidden(true);
}

Path MetaFilesManager::GetMetaFilepath(const GUID &guid)
{
    return MetaFilesManager::GetMetaFilepath(
        MetaFilesManager::GetFilepath(guid));
}

void MetaFilesManager::OnFilepathRenamed(const Path &oldPath,
                                         const Path &newPath)
{
    Path oldMetaFilepath = MetaFilesManager::GetMetaFilepath(oldPath);
    MetaFilesManager::UnRegisterMetaFilepath(oldMetaFilepath);

    Path newMetaFilepath = MetaFilesManager::GetMetaFilepath(newPath);

    File::Rename(oldMetaFilepath, newMetaFilepath);
    MetaFilesManager::RegisterMetaFilepath(newMetaFilepath);
}

MetaFilesManager *MetaFilesManager::GetInstance()
{
    return Application::GetInstance()->GetMetaFilesManager();
}

String MetaFilesManager::GetMetaExtension()
{
    return "meta";
}
