#include "Bang/Selection.h"

#include "Bang/Scene.h"
#include "Bang/Input.h"
#include "Bang/Camera.h"
#include "Bang/GEngine.h"
#include "Bang/SceneManager.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GameObject *Selection::GetOveredGameObject()
{
    if (!Input::IsMouseInsideWindow()) { return nullptr; }
    return GetOveredGameObject(  SceneManager::GetActiveScene() );
}

GameObject *Selection::GetOveredGameObject(Scene *scene)
{
    if (!scene) { return nullptr; }
    return Selection::GetOveredGameObject( scene->GetCamera() );
}

GameObject *Selection::GetOveredGameObject(Camera *cam)
{
    if (!cam) { return nullptr; }

    Vector2i mouseViewportPoint = Input::GetMousePositionWindow();
    mouseViewportPoint = cam->FromWindowPointToViewportPoint(mouseViewportPoint);
    return Selection::GetOveredGameObject(cam, mouseViewportPoint);
}

GameObject *Selection::GetOveredGameObject(Camera *cam,
                                           const Vector2i &viewportPoint)
{
    SelectionFramebuffer *sfb = cam->GetSelectionFramebuffer();
    if (sfb)
    {
        GameObject *selGo = sfb->GetGameObjectInViewportPoint(viewportPoint);
        if (selGo) { return selGo; }
    }
    return nullptr;
}
