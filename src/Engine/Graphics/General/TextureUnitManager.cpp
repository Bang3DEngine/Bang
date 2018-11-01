#include "Bang/TextureUnitManager.h"

#include <GL/glew.h>
#include <ostream>
#include <utility>

#include "Bang/Assert.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/Texture.h"
#include "Bang/USet.h"
#include "Bang/USet.tcc"

using namespace Bang;

TextureUnitManager::TextureUnitManager()
{
    m_numMaxTextureUnits = GL::GetInteger(GL::MAX_TEXTURE_IMAGE_UNITS);
    m_numUsableTextureUnits = m_numMaxTextureUnits - 1;
    m_voidTexUnit = m_numMaxTextureUnits - 1;

    for (uint i = 0; i < m_numUsableTextureUnits; ++i)
    {
        // All units are totally free at the beginning
        m_freeUnits.push(i);
    }
}

void TextureUnitManager::BindTextureToUnit(Texture *texture,
                                           TextureUnitManager::TexUnit unit)
{
    TextureUnitManager *tm = TextureUnitManager::GetActive();

    // Bind to texture unit
    GL::ActiveTexture(GL_TEXTURE0 + unit);
    if (texture)
    {
        texture->Bind();
    }

    // Bind the void unit, so that we dont make unwanted changes on occupied
    // texture units
    GL::ActiveTexture(GL_TEXTURE0 + tm->m_voidTexUnit);
}

TextureUnitManager::TexUnit TextureUnitManager::BindTextureToUnit(
    Texture *texture)
{
    TextureUnitManager *tm = TextureUnitManager::GetActive();

    ASSERT(texture);

    TexUnit texUnit;
    const GLId texId = texture->GetGLId();
    const bool texIsAlreadyInUnit =
        (tm->m_textureIdToBoundUnit.count(texId) == 1);
    if (texIsAlreadyInUnit)
    {
        // Texture was already bound to a unit. Return the unit it
        // was bound to.
        TexUnit prevTexUnit = tm->m_textureIdToBoundUnit[texId];
        texUnit = prevTexUnit;
    }
    else
    {
        // We will have to bind the texture to some unit.
        const TexUnit freeUnit = tm->MakeRoomAndGetAFreeTextureUnit();

        TextureUnitManager::BindTextureToUnit(texture, freeUnit);

        // Update number of times used
        tm->UpdateStructuresForUsedTexture(texture, freeUnit);

        texUnit = freeUnit;
    }

    tm->m_timestampTexIdUsed[texId] = tm->m_timestampCounter;
    ++tm->m_timestampCounter;

    // tm->CheckBindingsValidity();

    return texUnit;
}

uint TextureUnitManager::GetMaxTextureUnits()
{
    return TextureUnitManager::GetActive()->m_numMaxTextureUnits;
}

TextureUnitManager::TexUnit TextureUnitManager::MakeRoomAndGetAFreeTextureUnit()
{
    TexUnit freeUnit = -1;
    uint numFreeTextures = SCAST<uint>(m_freeUnits.size());
    if (numFreeTextures > 0)
    {
        // If there is a directly free unit
        freeUnit = m_freeUnits.front();  // Find the first free unit
        m_freeUnits.pop();
    }
    else
    {
        // Otherwise, we will have to make some room.
        // Unallocate one unit so that we can put the new texture there
        ASSERT(m_timestampTexIdUsed.size() >= 1);

        // Find the oldest used texture id
        GLId oldestUsedTextureId = m_timestampTexIdUsed.begin()->first;
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
    texture->EventEmitter<IEventsDestroy>::RegisterListener(this);

    // CheckBindingsValidity();
}

uint TextureUnitManager::GetNumUsableTextureUnits()
{
    TextureUnitManager *tm = TextureUnitManager::GetActive();
    return (tm ? tm->m_numUsableTextureUnits : 99);
}

void TextureUnitManager::UnBindAllTexturesFromAllUnits()
{
    const uint MaxTexUnits = TextureUnitManager::GetNumUsableTextureUnits();
    for (int unit = 0; unit < MaxTexUnits; ++unit)
    {
        GL::ActiveTexture(GL_TEXTURE0 + unit);
        GL::UnBind(GL::BindTarget::TEXTURE_1D);
        GL::UnBind(GL::BindTarget::TEXTURE_2D);
        GL::UnBind(GL::BindTarget::TEXTURE_3D);
        GL::UnBind(GL::BindTarget::TEXTURE_CUBE_MAP);
    }
}

uint TextureUnitManager::GetUnitTextureIsBoundTo(Texture *texture)
{
    if (!texture)
    {
        return -1;
    }

    TextureUnitManager *tm = TextureUnitManager::GetActive();
    GLId texId = texture->GetGLId();
    if (tm->m_textureIdToBoundUnit.count(texId) == 0)
    {
        return -1;
    }
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
    const int NumTextureUnits = GL::GetInteger(GL::MAX_TEXTURE_IMAGE_UNITS);
    for (int i = 0; i < NumTextureUnits; ++i)
    {
        Debug_Log("Texture unit " << i << " ---:");
        GL::ActiveTexture(GL_TEXTURE0 + i);
        GLId tex1DId = GL::GetInteger(GL::Enum::TEXTURE_BINDING_1D);
        GLId tex2DId = GL::GetInteger(GL::Enum::TEXTURE_BINDING_2D);
        GLId tex3DId = GL::GetInteger(GL::Enum::TEXTURE_BINDING_3D);
        GLId texCMId = GL::GetInteger(GL::Enum::TEXTURE_BINDING_CUBE_MAP);
        Debug_Log("  Texture_1D:      " << tex1DId);
        Debug_Log("  Texture_2D:      " << tex2DId);
        Debug_Log("  Texture_3D:      " << tex3DId);
        Debug_Log("  Texture_CubeMap: " << texCMId);

        uint boundToThisUnit = 0;
        if (tex1DId > 0)
        {
            ++boundToThisUnit;
        }
        if (tex2DId > 0)
        {
            ++boundToThisUnit;
        }
        if (tex3DId > 0)
        {
            ++boundToThisUnit;
        }
        if (texCMId > 0)
        {
            ++boundToThisUnit;
        }
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
    GEngine *ge = GEngine::GetInstance();
    return (ge ? ge->GetTextureUnitManager() : nullptr);
}

void TextureUnitManager::UnTrackTexture(GLId texId)
{
    TexUnit texUnit = m_textureIdToBoundUnit[texId];
    Texture *texture = m_textureIdToTexture[texId];
    if (m_textureIdToBoundUnit.find(texId) != m_textureIdToBoundUnit.end())
    {
        GL::ActiveTexture(GL_TEXTURE0 + texUnit);
        if (texture)
        {
            GL::UnBind(SCAST<GL::BindTarget>(texture->GetTextureTarget()));
        }
        GL::ActiveTexture(GL_TEXTURE0 + m_voidTexUnit);
    }

    m_timestampTexIdUsed.erase(texId);
    m_textureIdToTexture.erase(texId);
    m_textureIdToBoundUnit.erase(texId);
    if (texture)
    {
        texture->EventEmitter<IEventsDestroy>::UnRegisterListener(this);
    }

    // CheckBindingsValidity();
}

void TextureUnitManager::CheckBindingsValidity() const
{
    USet<TexUnit> boundUnits;
    for (const auto &pair : m_textureIdToBoundUnit)
    {
        if (boundUnits.Contains(pair.second))
        {
            ASSERT(false);
        }
        boundUnits.Add(pair.second);
    }
}

void TextureUnitManager::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    Texture *tex = DCAST<Texture *>(object);

    int texUnitTexWasBoundTo = -1;
    if (m_textureIdToBoundUnit.count(tex->GetGLId()) == 1)
    {
        texUnitTexWasBoundTo = m_textureIdToBoundUnit[tex->GetGLId()];
    }

    UnTrackTexture(tex->GetGLId());

    if (texUnitTexWasBoundTo >= 0)
    {
        m_freeUnits.push(texUnitTexWasBoundTo);
    }
}
