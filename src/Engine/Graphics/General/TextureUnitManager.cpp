#include "Bang/TextureUnitManager.h"

#include "Bang/GL.h"
#include "Bang/Texture.h"
#include "Bang/GEngine.h"

USING_NAMESPACE_BANG

TextureUnitManager::TextureUnitManager()
{
    m_numTextureUnits = GL::GetInteger(GL::MaxTextureImageUnits);
}

TextureUnitManager::TexUnit
TextureUnitManager::BindTextureToUnit(Texture *texture)
{
    TextureUnitManager *tm = TextureUnitManager::GetActive();

    const bool textureIsBoundToSomeUnit =
            tm->m_textureIdsBoundToSomeUnit.count(texture->GetGLId()) == 1;
    if (textureIsBoundToSomeUnit)
    {
        // Texture was bound to a unit, return the unit it was bound to
        TexUnit prevTexUnit = tm->m_textureIdToBoundUnit[texture->GetGLId()];
        return prevTexUnit;
    }
    else
    {
        // We will have to bind the texture to some unit. Get the least used
        TexUnit freeUnit;
        if (tm->m_usedUnitsQueue.size() < tm->m_numTextureUnits)
        {
            // If there is enough space, allocate to free unit
            freeUnit = tm->m_usedUnitsQueue.size();
        }
        else
        {
            // Otherwise, we will have to make some room.
            // Unallocate one unit so that we can put the new texture there
            TexUnit oldestUsedUnit = tm->m_usedUnitsQueue.front();
            freeUnit = oldestUsedUnit;
            tm->m_usedUnitsQueue.pop();
            tm->m_textureIdsBoundToSomeUnit.erase(texture->GetGLId());
            texture->EventEmitter<IDestroyListener>::UnRegisterListener(tm);
        }

        // Bind to texture unit
        GL::ActiveTexture(GL_TEXTURE0 + freeUnit);
        texture->Bind();

        tm->m_textureIdsBoundToSomeUnit.insert(texture->GetGLId());
        tm->m_textureIdToBoundUnit[texture->GetGLId()] = freeUnit;
        tm->m_usedUnitsQueue.push(freeUnit);  // Add to queue

        texture->EventEmitter<IDestroyListener>::RegisterListener(tm);
        return freeUnit;
    }
}


int TextureUnitManager::GetMaxTextureUnits()
{
    return TextureUnitManager::GetActive()->m_numTextureUnits;
}

void TextureUnitManager::UnBindAllTexturesFromAllUnits()
{
    const int MaxTexUnits = TextureUnitManager::GetMaxTextureUnits();
    for (int unit = 0; unit < MaxTexUnits; ++unit)
    {
        GL::ActiveTexture(GL_TEXTURE0 + unit);
        GL::UnBind(GL::BindTarget::Texture1D);
        GL::UnBind(GL::BindTarget::Texture2D);
        GL::UnBind(GL::BindTarget::Texture3D);
        GL::UnBind(GL::BindTarget::TextureCubeMap);
    }
}

GLId TextureUnitManager::GetBoundTextureToUnit(GL::TextureTarget texTarget,
                                               GL::Enum textureUnit)
{
    ASSERT(textureUnit >= GL_TEXTURE0);

    GL::ActiveTexture(textureUnit);
    GLId texId = GL::GetInteger(SCAST<GL::Enum>(texTarget));
    return texId;
}

void TextureUnitManager::PrintTextureUnits()
{
    Debug_Log("===============================");
    const int NumTextureUnits = GL::GetInteger(GL::MaxTextureImageUnits);
    for (int i = 0; i < NumTextureUnits; ++i)
    {
        Debug_Log("Texture unit " << i << " ---:");
        GL::ActiveTexture(GL_TEXTURE0 + i);
        GLId tex1DId = GL::GetInteger(GL::Enum::TextureBinding1D);
        GLId tex2DId = GL::GetInteger(GL::Enum::TextureBinding2D);
        GLId tex3DId = GL::GetInteger(GL::Enum::TextureBinding3D);
        GLId texCMId = GL::GetInteger(GL::Enum::TextureBindingCubeMap);
        Debug_Log("  Texture_1D:      " << tex1DId);
        Debug_Log("  Texture_2D:      " << tex2DId);
        Debug_Log("  Texture_3D:      " << tex3DId);
        Debug_Log("  Texture_CubeMap: " << texCMId);

        uint boundToThisUnit = 0;
        if (tex1DId > 0) { ++boundToThisUnit; }
        if (tex2DId > 0) { ++boundToThisUnit; }
        if (tex3DId > 0) { ++boundToThisUnit; }
        if (texCMId > 0) { ++boundToThisUnit; }
        if (boundToThisUnit > 1)
        {
            Debug_Error("More than one texture bound to the same unit !!!!");
        }
        Debug_Log("-----------------------------");
    }
    Debug_Log("===============================");
}

TextureUnitManager *TextureUnitManager::GetActive()
{
    return GEngine::GetActive()->GetTextureUnitManager();
}

void TextureUnitManager::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    Texture *tex = DCAST<Texture*>(object);
    m_textureIdToBoundUnit.erase(tex->GetGLId());
    m_textureIdsBoundToSomeUnit.erase(tex->GetGLId());
}
