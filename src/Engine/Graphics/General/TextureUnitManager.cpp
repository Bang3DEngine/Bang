#include "Bang/TextureUnitManager.h"

#include "Bang/GL.h"
#include "Bang/Set.h"
#include "Bang/Texture.h"
#include "Bang/GEngine.h"

USING_NAMESPACE_BANG

TextureUnitManager::TextureUnitManager()
{
    m_numTextureUnits = GL::GetInteger(GL::MaxTextureImageUnits);
    m_numTextureUnits -= 1; // Reserve the last one as void unit
    // m_numTextureUnits = 8; // Uncomment to stress test with min tex units

    for (int i = 0; i < m_numTextureUnits; ++i)
    {
        // All units are totally free at the beginning
        m_initialFreeUnits.push(i);
    }
}

TextureUnitManager::TexUnit
TextureUnitManager::BindTextureToUnit(Texture *texture)
{
    TextureUnitManager *tm = TextureUnitManager::GetActive();

    TexUnit texUnit;
    const GLId texId = texture->GetGLId();
    const bool texIsAlreadyInUnit = (tm->m_textureIdToBoundUnit.count(texId) == 1);
    if (texIsAlreadyInUnit)
    {
        // Texture was already bound to a unit. Return the unit it was bound to.
        TexUnit prevTexUnit = tm->m_textureIdToBoundUnit[texId];
        texUnit = prevTexUnit;
        // Debug_Log("Returning prevTexUnit " << texUnit << " for texture " << texture << "(" << texId << ")");
    }
    else
    {
        // We will have to bind the texture to some unit.
        // Debug_Log("Making room for " << texture << "...");
        const TexUnit freeUnit = tm->MakeRoomAndGetAFreeTextureUnit();

        // Bind to texture unit
        GL::ActiveTexture(GL_TEXTURE0 + freeUnit);
        GL::UnBind(GL::BindTarget::Texture1D);
        GL::UnBind(GL::BindTarget::Texture2D);
        GL::UnBind(GL::BindTarget::Texture3D);
        GL::UnBind(GL::BindTarget::TextureCubeMap);
        texture->Bind();

        // The last one is the void unit, so that subsequent binds for some other
        // reason dont change the current active texture unit.
        // In other words, without this line, the previous unit is bound, and
        // any texture bind for some other reason will overwrite the unit, and
        // we do not want it
        GL::ActiveTexture(GL_TEXTURE0 + tm->m_numTextureUnits);

        // Update number of times used
        // Debug_Log("Texture " << texture << "(" << texId << ") binding to unit " << freeUnit);
        tm->UpdateStructuresForUsedTexture(texture, freeUnit);

        texUnit = freeUnit;
    }

    tm->m_timestampTexIdUsed[texId] = tm->m_timestampCounter;
    ++tm->m_timestampCounter;

    // tm->CheckBindingsValidity();

    return texUnit;
}

TextureUnitManager::TexUnit TextureUnitManager::MakeRoomAndGetAFreeTextureUnit()
{
    TexUnit freeUnit = -1;
    uint numFreeTextures = m_initialFreeUnits.size();
    if (numFreeTextures > 0)
    {
        // If there is a directly free unit
        freeUnit = m_initialFreeUnits.top(); // Find the first free unit
        m_initialFreeUnits.pop();
        // Debug_Log("We had a direct free unit: " << freeUnit);
    }
    else
    {
        // Otherwise, we will have to make some room.
        // Unallocate one unit so that we can put the new texture there
        ASSERT(m_timestampTexIdUsed.size() >= 1);

        // Find the oldest used texture id
        GLId oldestUsedTextureId        = m_timestampTexIdUsed.begin()->first;
        uint oldestUsedTextureTimestamp = m_timestampTexIdUsed.begin()->second;
        for (const auto &pair : m_timestampTexIdUsed)
        {
            const GLId texId = pair.first;
            const uint texUseTimestamp = pair.second;
            if (texUseTimestamp < oldestUsedTextureTimestamp)
            {
                oldestUsedTextureTimestamp = texUseTimestamp;
                oldestUsedTextureId = texId;
            }
        }

        ASSERT(m_textureIdToBoundUnit.count(oldestUsedTextureId) == 1);

        // Debug_Log("Make room removing " << oldestUsedTextureId);

        // Get the unit we are going to replace
        freeUnit = m_textureIdToBoundUnit[oldestUsedTextureId];

        // UnTrack texture
        UnTrackTexture(oldestUsedTextureId);
    }
    return freeUnit;
}

void TextureUnitManager::UpdateStructuresForUsedTexture(Texture *texture,
                                                        uint usedUnit)
{
    const GLId texId = texture->GetGLId();

    m_timestampTexIdUsed[texId] = 0;
    m_textureIdToTexture[texId] = texture;
    m_textureIdToBoundUnit[texId] = usedUnit;
    texture->EventEmitter<IDestroyListener>::RegisterListener(this);
    // CheckBindingsValidity();
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

int TextureUnitManager::GetUnitTextureIsBoundTo(Texture *texture)
{
    if (!texture) { return -1; }

    TextureUnitManager *tm = TextureUnitManager::GetActive();
    GLId texId = texture->GetGLId();
    if (tm->m_textureIdToBoundUnit.count(texId) == 0) { return -1; }
    return tm->m_textureIdToBoundUnit[texId];
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

void TextureUnitManager::UnTrackTexture(GLId texId)
{
    TexUnit texUnit = m_textureIdToBoundUnit[texId];
    Texture *texture = m_textureIdToTexture[texId];
    if (m_textureIdToBoundUnit.find(texId) != m_textureIdToBoundUnit.end())
    {
        GL::ActiveTexture(GL_TEXTURE0 + texUnit);
        GL::UnBind( SCAST<GL::BindTarget>(texture->GetTextureTarget()) );
    }
    // Debug_Log("Untracking texture " << texture << "(" << texId << ") "
    //           << "which was bound to unit " << m_textureIdToBoundUnit[texId]);
    m_timestampTexIdUsed.erase(texId);
    m_textureIdToTexture.erase(texId);
    m_textureIdToBoundUnit.erase(texId);
    texture->EventEmitter<IDestroyListener>::UnRegisterListener(this);
    // CheckBindingsValidity();
}

void TextureUnitManager::CheckBindingsValidity() const
{
    Set<TexUnit> boundUnits;
    for (const auto &pair : m_textureIdToBoundUnit)
    {
        if (boundUnits.Contains(pair.second)) { ASSERT(false); }
        boundUnits.Add(pair.second);
    }
}

void TextureUnitManager::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    Texture *tex = DCAST<Texture*>(object);
    UnTrackTexture(tex->GetGLId());
}
