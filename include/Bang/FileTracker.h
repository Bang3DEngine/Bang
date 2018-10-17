#ifndef FILETRACKER_H
#define FILETRACKER_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/Time.h"
#include "Bang/UMap.h"
#include "Bang/Path.h"
#include "Bang/USet.h"

NAMESPACE_BANG_BEGIN

FORWARD class IEventsFileTracker;
FORWARD class Path;
FORWARD class String;

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
    const USet<Path>& GetTrackedPaths() const;
    Array<Path> GetTrackedPathsWithExtensions(
                        const Array<String> &extensions) const;
    Array<Path> GetTrackedPathsWithLastExtension(
                        const Array<String> &extensions) const;

private:
    USet<Path> m_trackedPaths;
    USet<Path> m_pathsJustRecentlyTracked;
    UMap<Path, Time> m_pathsToTrackToModificationTime;
};

NAMESPACE_BANG_END

#endif // FILETRACKER_H

