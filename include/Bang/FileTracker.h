#ifndef FILETRACKER_H
#define FILETRACKER_H

#include "Bang/UMap.h"
#include "Bang/Path.h"
#include "Bang/EventEmitter.h"

NAMESPACE_BANG_BEGIN

class IFileTrackerListener
{
    EVENTLISTENER(IFileTrackerListener);

public:
    virtual void OnPathAdded(const Path &addedPath)       {}
    virtual void OnPathModified(const Path &modifiedPath) {}
    virtual void OnPathRemoved(const Path &removedPath)   {}
};



class FileTracker : public EventEmitter<IFileTrackerListener>
{
public:
	FileTracker();
	virtual ~FileTracker();

    void TrackPath(const Path &path);
    void UnTrackPath(const Path &path);
    void Clear();

    void Update(bool forceCheckNow);

    void SetCheckFrequencySeconds(float checkFrequencySeconds);

    float GetCheckFrequencySeconds() const;

private:
    UMap<Path, uint64_t> m_pathsToTrackToModificationTime;
    float m_checkFrequencySeconds = 5.0f;
    uint64_t m_lastCheckTime = 0.0;

    bool NeedsCheck() const;
};

NAMESPACE_BANG_END

#endif // FILETRACKER_H

