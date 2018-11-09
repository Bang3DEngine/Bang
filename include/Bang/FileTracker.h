#ifndef FILETRACKER_H
#define FILETRACKER_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsFileTracker.h"
#include "Bang/Path.h"
#include "Bang/Time.h"
#include "Bang/UMap.h"
#include "Bang/USet.h"

namespace Bang
{
class Path;
class String;

class FileTracker : public EventEmitter<IEventsFileTracker>
{
public:
    FileTracker();
    virtual ~FileTracker() override;

    void TrackPath(const Path &path);
    void UnTrackPath(const Path &path);
    void Clear();

    void CheckFiles();

    Time GetModificationTime(const Path &path) const;
    const USet<Path> &GetTrackedPaths() const;
    Array<Path> GetTrackedPathsWithExtensions(
        const Array<String> &extensions) const;
    Array<Path> GetTrackedPathsWithLastExtension(
        const Array<String> &extensions) const;

private:
    USet<Path> m_trackedPaths;
    USet<Path> m_pathsJustRecentlyTracked;
    UMap<Path, Time> m_pathsToTrackToModificationTime;
};
}  // namespace Bang

#endif  // FILETRACKER_H
