#include "Bang/TextureUnitManager.h"

#include "Bang/GL.h"
#include "Bang/Texture.h"
#include "Bang/GEngine.h"

USING_NAMESPACE_BANG

TextureUnitManager::TextureUnitManager()
{
    c_numTextureUnits = GL::GetInteger(GL::MaxTextureImageUnits);
}

TextureUnitManager::TexUnit TextureUnitManager::BindTexture(
                                GL::TextureTarget texTarget, const GLId texId)
{
    TextureUnitManager *tm = GEngine::GetActive()->GetTextureUnitManager();

    TexUnit unitToUse = 0;
    TexUnitMap::Iterator it = tm->m_textureIdToUnit.Find(texId);
    if (false && it != tm->m_textureIdToUnit.End())
    {
        // It was still bound, reuse it (just return where it is)
        unitToUse = it->second;
    }
    else // We need to bind the new texture to the oldest used texture unit
    {
        if (tm->m_usedUnits.size() < tm->c_numTextureUnits)
        {
            unitToUse = tm->m_usedUnits.size();
        }
        else
        {   // Free oldest texture unit
            unitToUse = tm->m_usedUnits.front();
            tm->m_usedUnits.pop();
            tm->m_textureIdToUnit.RemoveValues(unitToUse);
        }
        tm->m_usedUnits.push(unitToUse);
        tm->m_textureIdToUnit.Add(texId, unitToUse);

        GL::ActiveTexture(GL_TEXTURE0 + unitToUse);
        GL::Bind(SCAST<GL::BindTarget>(texTarget), texId);

        // Debug_Peek(tm->m_usedUnits);
        // Debug_Peek(tm->m_textureIdToUnit);

        // ASSERT(tm->m_usedUnits.size() == tm->m_textureIdToUnit.Size());
        ASSERT(tm->m_usedUnits.size() <= tm->c_numTextureUnits);
    }

    return unitToUse;
}

TextureUnitManager::TexUnit TextureUnitManager::BindTexture(Texture *tex)
{
    if (EventEmitter<IDestroyListener>* ee = DCAST<EventEmitter<IDestroyListener>*>(tex))
    {
        TextureUnitManager *tm = GEngine::GetActive()->GetTextureUnitManager();
        ee->RegisterListener(tm);
    }

    return TextureUnitManager::BindTexture(tex->GetTextureTarget(), tex->GetGLId());
}

void TextureUnitManager::UnBindTexture(Texture *tex)
{
    if (EventEmitter<IDestroyListener>* ee = DCAST<EventEmitter<IDestroyListener>*>(tex))
    {
        TextureUnitManager *tm = GEngine::GetActive()->GetTextureUnitManager();
        ee->UnRegisterListener(tm);
    }

    TextureUnitManager::UnBindTexture(tex->GetTextureTarget(), tex->GetGLId());
}

void TextureUnitManager::UnBindTexture(GL::TextureTarget texTarget, GLId textureId)
{
    GEngine *gp = GEngine::GetActive();
    TextureUnitManager *tm = gp->GetTextureUnitManager();

    TexUnitMap::Iterator it = tm->m_textureIdToUnit.Find(textureId);
    if (it != tm->m_textureIdToUnit.End())
    {
        const TexUnit unit = it->second;
        tm->m_textureIdToUnit.Remove(textureId);

        GL::ActiveTexture(GL_TEXTURE0 + unit);
        GL::UnBind(SCAST<GL::BindTarget>(texTarget));
    }
}

void TextureUnitManager::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    Texture *tex = DCAST<Texture*>(object);
    TextureUnitManager::UnBindTexture(tex);
}
