#include "Bang/MetaFilesManager.h"

#include <vector>

#include "Bang/Application.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/File.h"
#include "Bang/Map.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Paths.h"
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

    Array<Path> files = directory.GetFiles(FindFlag::RECURSIVE);
    for (const Path &filepath : files)
    {
        MetaFilesManager::CreateMetaFileIfMissing(filepath);
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
    if (filepath.IsHiddenFile())
    {
        const Array<String> extensions = filepath.GetExtensions();
        for (const String &extension : extensions)
        {
            if (extension.BeginsWith(GetMetaExtension()))
            {
                return true;
            }
        }
    }
    return false;
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
    if (MetaFilesManager::IsMetaFile(metaFilepath))
    {
        MetaNode metaNode;
        metaNode.Import(metaFilepath);

        Path filepath = GetFilepath(metaFilepath);
        if (!filepath.IsFile())
        {
            filepath = filepath.WithHidden(true);
        }

        if (filepath.IsFile())
        {
            GUID guid = metaNode.Get<GUID>("GUID");
            RegisterFilepathGUID(filepath, guid);
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

void MetaFilesManager::RegisterFilepathGUID(const Path &filepath,
                                            const GUID &guid)
{
    MetaFilesManager *mfm = MetaFilesManager::GetInstance();

    auto it = mfm->m_GUIDToFilepath.Find(guid);
    if (it != mfm->m_GUIDToFilepath.End())
    {
        const Path &foundFilepath = it->second;
        if (foundFilepath != filepath)
        {
            Debug_Error(
                "Found conflicting GUID: " << guid << "(Files '" << filepath
                                           << "' and '"
                                           << mfm->m_GUIDToFilepath.Get(guid)
                                           << "'");
        }
    }
    else
    {
        if (!guid.IsEmpty() && !filepath.IsEmpty() &&
            !MetaFilesManager::IsMetaFile(filepath))
        {
            mfm->m_GUIDToFilepath.Add(guid, filepath);
            mfm->m_filepathToGUID.Add(filepath, guid);
        }
    }
}

GUID MetaFilesManager::RegisterFilepath(const Path &filepath)
{
    GUID guid = GUID::Empty();
    if (!MetaFilesManager::IsMetaFile(filepath))
    {
        const bool notRegisteredYet =
            (MetaFilesManager::GetGUID(filepath) == GUID::Empty());

        if (notRegisteredYet)
        {
            if (!Assets::IsEmbeddedAsset(filepath))
            {
                if (filepath.BeginsWith(Paths::GetProjectAssetsDir()))
                {
                    auto pathGUID =
                        MetaFilesManager::CreateMetaFileIfMissing(filepath);
                    guid = pathGUID.second;
                }
                else
                {
                    const Path metaFilepath =
                        MetaFilesManager::GetMetaFilepath(filepath);
                    guid = GetGUIDReadingMetaFilepath(metaFilepath);
                }

                if (guid == GUID::Empty())
                {
                    guid = GUIDManager::GetNewGUID();
                }

                RegisterFilepathGUID(filepath, guid);
            }
        }
    }
    return guid;
}

GUID MetaFilesManager::GetGUID(const Path &filepath)
{
    MetaFilesManager *mfm = MetaFilesManager::GetInstance();
    auto it = mfm->m_filepathToGUID.Find(filepath);
    if (it != mfm->m_filepathToGUID.End())
    {
        const GUID &guid = it->second;
        return guid;
    }
    else
    {
        if (!Assets::IsEmbeddedAsset(filepath))
        {
            if (!filepath.IsFile())
            {
                return GUID::Empty();
            }

            Path metaFilepath = GetMetaFilepath(filepath);
            GUID guid = GetGUIDReadingMetaFilepath(metaFilepath);
            if (!guid.IsEmpty())
            {
                mfm->m_filepathToGUID.Add(filepath, guid);
                return guid;
            }
        }
        else
        {
            Path parentResPath = filepath.GetDirectory();
            if (!parentResPath.IsFile())
            {
                return GUID::Empty();
            }

            Asset *parentRes = Assets::GetCached(parentResPath);
            if (parentRes)
            {
                if (Asset *embeddedRes =
                        parentRes->GetEmbeddedAsset(filepath.GetNameExt()))
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
    auto it = mfm->m_GUIDToFilepath.Find(guid);
    if (it != mfm->m_GUIDToFilepath.End())
    {
        const Path &filepath = it->second;
        return filepath;
    }
    else
    {
        if (Assets::IsEmbeddedAsset(guid))
        {
            Path parentPath =
                MetaFilesManager::GetFilepath(guid.WithoutEmbeddedAssetGUID());
            if (parentPath.IsFile())
            {
                AH<Asset> assetAH = Assets::LoadFromExtension(parentPath);
                if (assetAH)
                {
                    String name = assetAH.Get()->GetEmbeddedAssetName(
                        guid.GetEmbeddedAssetGUID());
                    Path path = parentPath.Append(name);
                    mfm->m_GUIDToFilepath.Add(guid, path);
                    return path;
                }
            }
        }
    }
    return Path::Empty();
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

GUID MetaFilesManager::GetGUIDReadingMetaFilepath(const Path &metaFilepath)
{
    GUID guid = GUID::Empty();
    if (metaFilepath.IsFile() && MetaFilesManager::IsMetaFile(metaFilepath))
    {
        MetaNode metaNode;
        metaNode.Import(metaFilepath);
        guid = metaNode.Get<GUID>("GUID");
    }
    return guid;
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
