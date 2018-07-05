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

    void Update(bool forceCheckNow);

    void SetCheckFrequencySeconds(float checkFrequencySeconds);

    float GetCheckFrequencySeconds() const;

private:
    USet<Path> m_pathsJustRecentlyTracked;
    UMap<Path, double> m_pathsToTrackToModificationTime;
    float m_checkFrequencySeconds = 5.0f;
    double m_lastCheckTime = 0.0;

    bool NeedsCheck() const;
};

NAMESPACE_BANG_END

#endif // FILETRACKER_H

