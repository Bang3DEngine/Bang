#include "Bang/ImportFilesManager.h"

#include "Bang/Set.h"
#include "Bang/File.h"
#include "Bang/List.h"
#include "Bang/USet.h"
#include "Bang/Debug.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Application.h"
#include "Bang/XMLNodeReader.h"
#include "Bang/ResourceHandle.h"

USING_NAMESPACE_BANG

ImportFilesManager::ImportFilesManager()
{
}

ImportFilesManager::~ImportFilesManager()
{
}

void ImportFilesManager::CreateMissingImportFiles(const Path &directory)
{
    // First load existing importFiles, to avoid creating new import files
    // with duplicated GUIDs.
    ImportFilesManager::LoadImportFilepathGUIDs(directory);

    List<Path> assetFile = directory.GetFiles(Path::FindFlag::RECURSIVE);

    USet<Path> files;
    files.Add(assetFile.Begin(), assetFile.End());

    for (const Path &filepath : files)
    {
        if (!IsImportFile(filepath) && !HasImportFile(filepath))
        {
            ImportFilesManager::CreateImportFileIfMissing(filepath);
        }
    }
}

void ImportFilesManager::LoadImportFilepathGUIDs(const Path &directory)
{
    Array<String> extensions = {GetImportExtension()};
    List<Path> importFilepaths = directory.GetFiles(Path::FindFlag::RECURSIVE_HIDDEN,
                                                    extensions);

    // Remove alone .import files
    for (const Path &importFilepath : importFilepaths)
    {
        if ( IsImportFile(importFilepath) &&
            !GetFilepath(importFilepath).IsFile() )
        {
            File::Remove(importFilepath);
        }
    }

    // Load GUID's of import files!
    for (const Path &importFilepath : importFilepaths)
    {
        RegisterImportFilepath(importFilepath);
    }
}

std::pair<Path, GUID> ImportFilesManager::CreateImportFileIfMissing(const Path &filepath)
{
    Path importFilepath = GetImportFilepath(filepath);
    GUID newGUID = GUID::Empty();
    if ( !IsImportFile(filepath) && !HasImportFile(filepath) )
    {
        XMLNode xmlInfo;
        newGUID = GUIDManager::GetNewGUID();
        xmlInfo.Set("GUID", newGUID);
        File::Write(importFilepath, xmlInfo.ToString());
        ImportFilesManager::RegisterImportFilepath(importFilepath);
    }
    else
    {
        newGUID = GetGUID(filepath);
    }
    return std::make_pair(importFilepath, newGUID);
}

bool ImportFilesManager::HasImportFile(const Path &filepath)
{
    return GetImportFilepath(filepath).Exists();
}

bool ImportFilesManager::IsImportFile(const Path &filepath)
{
    return filepath.IsHiddenFile() &&
           filepath.HasExtension( GetImportExtension() );
}

void ImportFilesManager::DuplicateImportFile(const Path &filepath,
                                             const Path &dupFilepath)
{
    const Path dupImportFilepath = GetImportFilepath(dupFilepath);
    File::Remove(dupImportFilepath);

    const GUID& newGUID = CreateImportFileIfMissing(dupFilepath).second;
    const Path originalImportFilepath = GetImportFilepath(filepath);
    XMLNode originalXML = XMLNodeReader::FromFile(originalImportFilepath);
    originalXML.Set("GUID", newGUID);

    File::Write(dupImportFilepath, originalXML.ToString());
    RegisterImportFilepath(dupImportFilepath);
}

GUIDManager* ImportFilesManager::GetGUIDManager()
{
    return &(ImportFilesManager::GetInstance()->m_GUIDManager);
}

void ImportFilesManager::RegisterImportFilepath(const Path &importFilepath)
{
    if (!IsImportFile(importFilepath)) { return; }
    XMLNode info = XMLNodeReader::FromFile(importFilepath);

    Path filepath = GetFilepath(importFilepath);
    GUID guid = info.Get<GUID>("GUID");
    ImportFilesManager *ifm = ImportFilesManager::GetInstance();
    if (ifm->m_GUIDToFilepath.ContainsKey(guid) &&
        ifm->m_GUIDToFilepath.Get(guid) != filepath)
    {
        Debug_Error("Found conflicting GUID: " << guid <<
                    "(Files '" << filepath << "' and '" <<
                    ifm->m_GUIDToFilepath.Get(guid) << "'");
    }

    ifm->m_GUIDToFilepath.Add(guid, filepath);
    ifm->m_filepathToGUID.Add(filepath, guid);
}

void ImportFilesManager::UnRegisterImportFilepath(const Path &importFilepath)
{
    ImportFilesManager *ifm = ImportFilesManager::GetInstance();
    Path filepath = GetFilepath(importFilepath);
    ifm->m_GUIDToFilepath.RemoveValues(filepath);
    ifm->m_filepathToGUID.Remove(filepath);
}

GUID ImportFilesManager::GetGUID(const Path& filepath)
{
    ImportFilesManager *ifm = ImportFilesManager::GetInstance();
    if (ifm->m_filepathToGUID.ContainsKey(filepath))
    {
        return ifm->m_filepathToGUID.Get(filepath);
    }
    else
    {
        if ( !Resources::IsEmbeddedResource(filepath) )
        {
            Path importFilepath = GetImportFilepath(filepath);
            if (importFilepath.IsFile())
            {
                XMLNode xmlNode = XMLNodeReader::FromFile(importFilepath);
                GUID guid = xmlNode.Get<GUID>("GUID");
                ifm->m_filepathToGUID.Add(filepath, guid);
                return guid;
            }
        }
        else
        {
            Path parentResPath = filepath.GetDirectory();
            Resource *parentRes = Resources::GetCached(parentResPath);
            if (parentRes)
            {
                if (Resource *embeddedRes = parentRes->GetEmbeddedResource(
                                                        filepath.GetNameExt()))
                {
                    return embeddedRes->GetGUID();
                }
            }
        }
    }
    return GUID::Empty();
}

Path ImportFilesManager::GetFilepath(const GUID &guid)
{
    ImportFilesManager *ifm = ImportFilesManager::GetInstance();
    if (ifm->m_GUIDToFilepath.ContainsKey(guid))
    {
        Path path = ifm->m_GUIDToFilepath.Get(guid);
        return path;
    }
    else
    {
        if (Resources::IsEmbeddedResource(guid))
        {
            Path parentPath = ImportFilesManager::GetFilepath(
                                    guid.WithoutEmbeddedResourceGUID());
            if (parentPath.IsFile())
            {
                RH<Resource> resRH = Resources::LoadFromExtension(parentPath);
                if (resRH)
                {
                    String name = resRH.Get()->GetEmbeddedResourceName(
                                                guid.GetEmbeddedResourceGUID() );
                    return parentPath.Append(name);
                }
            }
        }
    }
    return Path::Empty;
}

Path ImportFilesManager::GetFilepath(const Path &importFilepath)
{
    Path filepath = importFilepath.WithHidden(false);

    String strPath = filepath.GetAbsolute();
    if (strPath.BeginsWith(".")) { strPath.Remove(0, 1); }

    String ending = "." + GetImportExtension();
    if (strPath.EndsWith(ending))
    {
        strPath.Remove(strPath.Size() - ending.Size(), strPath.Size());
    }

    return Path(strPath);
}

Path ImportFilesManager::GetImportFilepath(const Path &filepath)
{
    return filepath.AppendExtension(
                ImportFilesManager::GetImportExtension() ).WithHidden(true);
}

Path ImportFilesManager::GetImportFilepath(const GUID &guid)
{
    return ImportFilesManager::GetImportFilepath(
                                    ImportFilesManager::GetFilepath(guid) );
}

void ImportFilesManager::OnFilepathRenamed(const Path &oldPath,
                                           const Path &newPath)
{
    Path oldImportFilepath = ImportFilesManager::GetImportFilepath(oldPath);
    ImportFilesManager::UnRegisterImportFilepath(oldImportFilepath);

    Path newImportFilepath = ImportFilesManager::GetImportFilepath(newPath);

    File::Rename(oldImportFilepath, newImportFilepath);
    ImportFilesManager::RegisterImportFilepath(newImportFilepath);
}

ImportFilesManager *ImportFilesManager::GetInstance()
{
    return Application::GetInstance()->GetImportFilesManager();
}

String ImportFilesManager::GetImportExtension()
{
    return "import";
}
