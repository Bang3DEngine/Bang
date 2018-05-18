#include "Bang/SelectionFramebuffer.h"

#include "Bang/GL.h"
#include "Bang/Paths.h"
#include "Bang/Input.h"
#include "Bang/Vector3.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

SelectionFramebuffer::SelectionFramebuffer(int width, int height) :
    Framebuffer(width, height)
{
    ShaderProgram *selectionProgram =
        ShaderProgramFactory::Get(
                ShaderProgramFactory::GetDefaultVertexShaderPath(),
                EPATH("Shaders/SelectionBuffer.frag") );

    p_selectionMaterial = Resources::Create<Material>();
    p_selectionMaterial.Get()->SetShaderProgram(selectionProgram);

    Bind();
    CreateAttachmentTex2D(AttColor, GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(GL::Attachment::DepthStencil,
                     GL::ColorFormat::Depth24_Stencil8);
    UnBind();

    p_colorTexture.Set(GetAttachmentTex2D(AttColor));
}

SelectionFramebuffer::~SelectionFramebuffer()
{
}

void SelectionFramebuffer::SetNextRenderSelectable(GameObject *go)
{
    p_nextRenderSelectable = go;
}

void SelectionFramebuffer::PrepareNewFrameForRender(const GameObject *go)
{
    IdType id = 1;
    m_gameObject_To_Id.Clear();
    m_id_To_GameObject.Clear();

    List<GameObject*> gameObjects = go->GetChildrenRecursively();
    for (GameObject *go : gameObjects)
    {
        m_gameObject_To_Id[go] = id;
        m_id_To_GameObject[id] = go;

        go->EventEmitter<IDestroyListener>::RegisterListener(this);

        ++id;
    }

    SetAllDrawBuffers();
}

void SelectionFramebuffer::RenderForSelectionBuffer(Renderer *rend)
{
    ASSERT(GL::IsBound(this));
    if (!rend->GetActiveMaterial()) { return; }

    GameObject *go = p_nextRenderSelectable ? p_nextRenderSelectable :
                                              rend->GetGameObject();

    RH<ShaderProgram> prevSP;
    prevSP.Set( rend->GetActiveMaterial()->GetShaderProgram() );

    rend->GetActiveMaterial()->EventEmitter<IMaterialChangedListener>::
                               SetEmitEvents(false);

    ShaderProgram *selSP = p_selectionMaterial.Get()->GetShaderProgram();
    rend->GetActiveMaterial()->SetShaderProgram(selSP);

    rend->Bind();
    selSP->SetColor("selectionColor", GetSelectionColor(go));
    rend->OnRender();
    rend->UnBind();

    rend->GetActiveMaterial()->SetShaderProgram(prevSP.Get());

    rend->GetActiveMaterial()->EventEmitter<IMaterialChangedListener>::
                               SetEmitEvents(true);

    p_nextRenderSelectable = nullptr;
}

GameObject *SelectionFramebuffer::
GetGameObjectInViewportPoint(const Vector2i &vpPoint)
{
    Color colorUnderMouse = ReadColor(vpPoint.x, vpPoint.y, AttColor);
    IdType id = MapColorToId(colorUnderMouse);
    if (colorUnderMouse != Color::Zero && m_id_To_GameObject.ContainsKey(id))
    {
        return m_id_To_GameObject[id];
    }
    return nullptr;
}

void SelectionFramebuffer::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    GameObject *go = DCAST<GameObject*>(object);
    if (go)
    {
        if (m_gameObject_To_Id.ContainsKey(go))
        {
            IdType id = m_gameObject_To_Id.Get(go);
            m_gameObject_To_Id.Remove(go);
            m_id_To_GameObject.Remove(id);
        }
    }
}

Color SelectionFramebuffer::GetSelectionColor(GameObject *go) const
{
    return MapIdToColor(m_gameObject_To_Id[go]);
}

Color SelectionFramebuffer::MapIdToColor(IdType id)
{
    constexpr IdType C = 256;
    Color color =
            Color(SCAST<float>(   id                % C),
                  SCAST<float>(  (id / C)           % C),
                  SCAST<float>( ((id / C) / C)      % C),
                  SCAST<float>((((id / C) / C) / C) % C)
                 );

    #ifdef DEBUG
        color.a = 256.0; // To be able to see selection framebuffer
    #endif

   return color / SCAST<float>(C);
}

typename SelectionFramebuffer::IdType
SelectionFramebuffer::MapColorToId(const Color &color)
{
    constexpr IdType C = 256;
    return   IdType(color.r * C)
           + IdType(color.g * C * C)
           + IdType(color.b * C * C * C)
           ;// + IdType(color.a * C * C * C * C);
}

RH<Texture2D> SelectionFramebuffer::GetColorTexture() const
{
    return p_colorTexture;
}
