#ifndef TEXTUREUNITMANAGER_H
#define TEXTUREUNITMANAGER_H

#include <queue>
#include <unordered_map>
#include <unordered_set>

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

    static TexUnit BindTextureToUnit(Texture *texture);

    static int GetMaxTextureUnits();
    static void UnBindAllTexturesFromAllUnits();

    static GLId GetBoundTextureToUnit(GL::TextureTarget texTarget,
                                      GL::Enum textureUnit);
    static void PrintTextureUnits();
    static TextureUnitManager* GetActive();

private:
    int m_numTextureUnits = -1;
    std::queue<TexUnit> m_usedUnitsQueue;
    std::unordered_set<GLId> m_textureIdsBoundToSomeUnit;
    std::unordered_map<GLId, TexUnit> m_textureIdToBoundUnit;

    void OnDestroyed(EventEmitter<IDestroyListener> *object) override;
};

NAMESPACE_BANG_END

#endif // TEXTUREUNITMANAGER_H
