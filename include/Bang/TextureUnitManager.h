#ifndef TEXTUREUNITMANAGER_H
#define TEXTUREUNITMANAGER_H

#include <queue>
#include <unordered_map>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/GL.h"
#include "Bang/IEventsDestroy.h"

namespace Bang
{
template <class>
class EventEmitter;
class IEventsDestroy;
class Texture;

class TextureUnitManager : public EventListener<IEventsDestroy>
{
public:
    using TexUnit = uint;
    TextureUnitManager();

    static void BindTextureToUnit(Texture *texture, TexUnit unit);
    static TexUnit BindTextureToUnit(Texture *texture);

    static uint GetMaxTextureUnits();
    static uint GetNumUsableTextureUnits();
    static void UnBindAllTexturesFromAllUnits();

    static uint GetUnitTextureIsBoundTo(Texture *texture);
    static GLId GetBoundTextureToUnit(GL::TextureTarget texTarget,
                                      GL::Enum textureUnit);
    static void PrintTextureUnits();
    static TextureUnitManager *GetActive();

private:
    uint m_numMaxTextureUnits = SCAST<uint>(-1);
    uint m_numUsableTextureUnits = SCAST<uint>(-1);
    TexUnit m_voidTexUnit = -1;

    uint m_timestampCounter = 0;
    std::queue<TexUnit> m_freeUnits;
    std::unordered_map<GLId, uint> m_timestampTexIdUsed;
    std::unordered_map<GLId, Texture *> m_textureIdToTexture;
    std::unordered_map<GLId, TexUnit> m_textureIdToBoundUnit;

    TexUnit MakeRoomAndGetAFreeTextureUnit();
    void UpdateStructuresForUsedTexture(Texture *texture, uint usedUnit);
    void UnTrackTexture(GLId textureId);
    void CheckBindingsValidity() const;

    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;
};
}  // namespace Bang

#endif  // TEXTUREUNITMANAGER_H
