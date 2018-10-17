#ifndef MODELIO_H
#define MODELIO_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

FORWARD class aiMaterial;
FORWARD class aiMesh;
FORWARD class aiNode;
FORWARD class aiScene;

FORWARD namespace Assimp
{
FORWARD class Importer;
}

NAMESPACE_BANG_BEGIN

FORWARD_T class Tree;
FORWARD   class Animation;
FORWARD   class GameObject;
FORWARD   class Material;
FORWARD   class Model;
FORWARD   class Path;

struct ModelIONode
{
    String name;
    Matrix4 localToParent;

    Array<uint> meshIndices;         // Which meshes this node has
    Array<uint> meshMaterialIndices; // Which material does each mesh have
};

struct ModelIOScene
{    
    Array< RH<Mesh> > meshes;
    Array< String > meshesNames;

    Array< RH<Material> > materials;
    Array< String > materialsNames;

    Array< RH<Animation> > animations;
    Array< String > animationsNames;

    Map<String, Mesh::Bone> allBones;

    String rootGameObjectName = "";
    Tree<ModelIONode> *modelTree = nullptr;

    ~ModelIOScene();
    void Clear();
};

class ModelIO
{
public:
    static int GetModelNumTriangles(const Path& modelFilepath);

    static bool ImportModel(const Path& modelFilepath,
                            Model *model,
                            ModelIOScene *modelScene);

    static void ImportMeshRaw(
                     aiMesh *aMesh,
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
    static aiNode* GameObjectToAiNode(const GameObject *gameObject,
                                      const Array<Mesh*> &sceneMeshes);
    static aiMesh* MeshToAiMesh(const Mesh *mesh);
    static aiMaterial* MaterialToAiMaterial(const Material *material);
    static const aiScene *ImportScene(Assimp::Importer *importer,
                                      const Path& modelFilepath);

    static void ImportEmbeddedMesh(aiMesh *aMesh,
                                   Model *model,
                                   RH<Mesh> *outMesh,
                                   String *outMeshName);
    static void ImportEmbeddedMaterial(aiMaterial *aMaterial,
                                       const Path& modelDirectory,
                                       Model *model,
                                       RH<Material> *outMaterial,
                                       String *outMaterialName);
};

NAMESPACE_BANG_END

#endif // MODELIO_H
