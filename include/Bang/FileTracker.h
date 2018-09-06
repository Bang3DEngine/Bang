#ifndef FILETRACKER_H
#define FILETRACKER_H

#include "Bang/UMap.h"
#include "Bang/USet.h"
#include "Bang/Path.h"
#include "Bang/Time.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsFileTracker.h"

NAMESPACE_BANG_BEGIN

class FileTracker : public EventEmitter<IEventsFileTracker>
{
public:
	FileTracker();
	virtual ~FileTracker();

    void TrackPath(const Path &path);
    void UnTrackPath(const Path &path);
    void Clear();

    void CheckFiles();

    const USet<Path>& GetTrackedPaths() const;
    Array<Path> GetTrackedPathsWithExtensions(
                        const Array<String> &extensions) const;
    Array<Path> GetTrackedPathsWithLastExtension(
                        const Array<String> &extensions) const;

private:
    USet<Path> m_trackedPaths;
    USet<Path> m_pathsJustRecentlyTracked;
    UMap<Path, Time::TimeT> m_pathsToTrackToModificationTime;
};

NAMESPACE_BANG_END

#endif // FILETRACKER_H

