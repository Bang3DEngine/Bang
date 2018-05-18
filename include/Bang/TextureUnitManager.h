#ifndef TEXTUREUNITMANAGER_H
#define TEXTUREUNITMANAGER_H

#include <queue>
#include <stack>
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

    static void BindTextureToUnit(Texture *texture, TexUnit unit);
    static TexUnit BindTextureToUnit(Texture *texture);

    static uint GetMaxTextureUnits();
    static uint GetNumUsableTextureUnits();
    static void UnBindAllTexturesFromAllUnits();

    static uint GetUnitTextureIsBoundTo(Texture *texture);
    static GLId GetBoundTextureToUnit(GL::TextureTarget texTarget,
                                      GL::Enum textureUnit);
    static void PrintTextureUnits();
    static TextureUnitManager* GetActive();


private:
    uint m_numMaxTextureUnits    = -1;
    uint m_numUsableTextureUnits = -1;
    TexUnit m_voidTexUnit        = -1;

    uint m_timestampCounter = 0;
    std::queue<TexUnit> m_freeUnits;
    std::unordered_map<GLId, uint> m_timestampTexIdUsed;
    std::unordered_map<GLId, Texture*> m_textureIdToTexture;
    std::unordered_map<GLId, TexUnit> m_textureIdToBoundUnit;

    TexUnit MakeRoomAndGetAFreeTextureUnit();
    void UpdateStructuresForUsedTexture(Texture *texture, uint usedUnit);
    void UnTrackTexture(GLId textureId);
    void CheckBindingsValidity() const;

    void OnDestroyed(EventEmitter<IDestroyListener> *object) override;
};

NAMESPACE_BANG_END

#endif // TEXTUREUNITMANAGER_H
