#ifndef TEXTUREUNITMANAGER_H
#define TEXTUREUNITMANAGER_H

#include <queue>

#include "Bang/GL.h"
#include "Bang/Map.h"
#include "Bang/IDestroyListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture;

class TextureUnitManager : public IDestroyListener
{
public:
    using TexUnit = uint;
    TextureUnitManager();

    // Returns the texture unit it has been bound to
    static TexUnit BindTexture(Texture *tex);
    static TexUnit BindTexture(GL::TextureTarget texTarget, GLId textureId);
    static void UnBindTexture(Texture *tex);
    static void UnBindTexture(GL::TextureTarget texTarget, GLId textureId);

private:
    using TexUnitMap = Map<const GLId, TexUnit>;

    TexUnitMap m_textureIdToUnit;
    std::queue<TexUnit> m_usedUnits; // Ordered in time
    int c_numTextureUnits = 0;

    void OnDestroyed(EventEmitter<IDestroyListener> *object) override;
};

NAMESPACE_BANG_END

#endif // TEXTUREUNITMANAGER_H
