#ifndef MODELIO_H
#define MODELIO_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/String.h"

struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;

namespace Assimp
{
class Importer;
}

namespace Bang
{
template <class>
class Tree;
class Animation;
class GameObject;
class Material;
class Model;
class Path;

struct ModelIONode
{
    String name;
    Matrix4 localToParent;

    Array<uint> meshIndices;          // Which meshes this node has
    Array<uint> meshMaterialIndices;  // Which material does each mesh have
};

struct ModelIOScene
{
    Array<AH<Mesh>> meshes;
    Array<String> meshesNames;

    Array<AH<Material>> materials;
    Array<String> materialsNames;

    Array<AH<Animation>> animations;
    Array<String> animationsNames;

    Map<String, Mesh::Bone> allBones;

    String rootGameObjectName = "";
    Tree<ModelIONode> *modelTree = nullptr;

    ~ModelIOScene();
    void Clear();
};

class ModelIO
{
public:
    static bool ImportModel(const Path &modelFilepath,
                            Model *model,
                            ModelIOScene *modelScene);

    static void ImportMeshRaw(aiMesh *aMesh,
                              Array<Mesh::VertexId> *vertexIndices,
                              Array<Vector3> *vertexPositionsPool,
                              Array<Vector3> *vertexNormalsPool,
                              Array<Vector2> *vertexUvsPool,
                              Array<Vector3> *vertexTangentsPool,
                              Map<String, Mesh::Bone> *bones,
                              Map<String, uint> *bonesIndices);

    static void ExportModel(const GameObject *gameObject,
                            const Path &meshExportPath);

    ModelIO() = delete;

private:
    static String GetExtensionIdFromExtension(const String &extension);
    static aiNode *GameObjectToAiNode(const GameObject *gameObject,
                                      const Array<Mesh *> &sceneMeshes);
    static aiMesh *MeshToAiMesh(const Mesh *mesh);
    static aiMaterial *MaterialToAiMaterial(const Material *material);
    static const aiScene *ImportScene(Assimp::Importer *importer,
                                      const Path &modelFilepath);

    static void ImportEmbeddedMesh(aiMesh *aMesh,
                                   Model *model,
                                   AH<Mesh> *outMesh,
                                   String *outMeshName);
    static void ImportEmbeddedMaterial(aiMaterial *aMaterial,
                                       const Path &modelDirectory,
                                       Model *model,
                                       AH<Material> *outMaterial,
                                       String *outMaterialName);
};
}  // namespace Bang

#endif  // MODELIO_H
