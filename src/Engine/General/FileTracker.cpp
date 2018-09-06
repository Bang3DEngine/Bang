#include "Bang/FileTracker.h"

#include "Bang/Time.h"
#include "Bang/UMap.h"
#include "Bang/List.h"
#include "Bang/Debug.h"
#include "Bang/Extensions.h"

USING_NAMESPACE_BANG

FileTracker::FileTracker()
{
}

FileTracker::~FileTracker()
{
}

void FileTracker::TrackPath(const Path &path)
{
    if (path.IsDir())
    {
        Array<Path> dirSubPaths = path.GetSubPaths(Path::FindFlag::RECURSIVE_HIDDEN);
        for (const Path &subPath : dirSubPaths)
        {
            TrackPath(subPath);
        }
    }

    if (path.Exists())
    {
        bool wasBeingTracked = (m_pathsToTrackToModificationTime.ContainsKey(path));
        Time::TimeT modTime = path.GetModificationTimeSeconds();
        m_pathsToTrackToModificationTime.Add(path, modTime);
        m_trackedPaths.Add(path);
        if (!wasBeingTracked)
        {
            m_pathsJustRecentlyTracked.Add(path);
            EventEmitter<IEventsFileTracker>::
               PropagateToListeners(&IEventsFileTracker::OnPathAdded, path);
        }
    }
}

void FileTracker::UnTrackPath(const Path &path)
{
    if (path.IsDir())
    {
        Array<Path> subpaths = path.GetSubPaths(Path::FindFlag::RECURSIVE_HIDDEN);
        for (const Path &subpath : subpaths)
        {
            UnTrackPath(subpath);
        }
    }
    m_trackedPaths.Remove(path);
    m_pathsJustRecentlyTracked.Remove(path);
    m_pathsToTrackToModificationTime.Remove(path);
}

void FileTracker::Clear()
{
    m_trackedPaths.Clear();
    m_pathsJustRecentlyTracked.Clear();
    m_pathsToTrackToModificationTime.Clear();
}

void FileTracker::CheckFiles()
{
    const UMap<Path, Time::TimeT> previousPathsToTrack =
                                    m_pathsToTrackToModificationTime;

    // Check for removed paths
    for (const auto &previousPathToModTime : previousPathsToTrack)
    {
        const Path &previousPath = previousPathToModTime.first;
        if (!previousPath.Exists())
        {
            EventEmitter<IEventsFileTracker>::
               PropagateToListeners(&IEventsFileTracker::OnPathRemoved,
                                    previousPath);
            UnTrackPath(previousPath);
        }
    }

    m_pathsJustRecentlyTracked.Clear(); // Clear just tracked paths

    // Check for new paths and add them to track
    for (const auto &pathToModTime : previousPathsToTrack)
    {
        const Path &path = pathToModTime.first;
        TrackPath(path);
    }

    // Check for modified paths
    for (const auto &pathToModTime : m_pathsToTrackToModificationTime)
    {
        const Path &path = pathToModTime.first;
        bool isNewPath = (m_pathsJustRecentlyTracked.Contains(path));
        if (!isNewPath)
        {
            ASSERT(previousPathsToTrack.ContainsKey(path));

            const Time::TimeT prevModTime = previousPathsToTrack.Get(path);
            const Time::TimeT newModTime  = path.GetModificationTimeSeconds();
            if (newModTime != prevModTime)
            {
                m_pathsToTrackToModificationTime.Add(path, newModTime);

                EventEmitter<IEventsFileTracker>::
                   PropagateToListeners(&IEventsFileTracker::OnPathModified,
                                        path);
            }
        }
    }
}

const USet<Path> &FileTracker::GetTrackedPaths() const
{
    return m_trackedPaths;
}

Array<Path> FileTracker::GetTrackedPathsWithExtensions(
                                    const Array<String> &extensions) const
{
    Array<Path> paths;
    for (const Path &trackedPath : GetTrackedPaths())
    {
        if (Extensions::Equals(trackedPath.GetExtension(), extensions))
        {
            paths.PushBack(trackedPath);
        }
    }
    return paths;
}

Array<Path> FileTracker::GetTrackedPathsWithLastExtension(
                                    const Array<String> &extensions) const
{
    Array<Path> paths;
    for (const Path &trackedPath : GetTrackedPaths())
    {
        if (Extensions::Equals(trackedPath.GetLastExtension(), extensions))
        {
            paths.PushBack(trackedPath);
        }
    }
    return paths;
}
