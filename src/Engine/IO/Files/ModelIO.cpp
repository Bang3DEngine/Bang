#include "Bang/ModelIO.h"

#include <assimp/anim.h>
#include <assimp/cexport.h>
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/matrix4x4.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/material.inl>
#include <assimp/matrix4x4.inl>
// Avoid asserts when reading meshes...
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/vector3.h>
#include <sys/types.h>
#include <array>
#include <fstream>
#include <unordered_map>
#include <utility>
#ifndef FROM_TRAVIS
#include <assimp/Exporter.hpp>
#endif

#include "Bang/Animation.h"
#include "Bang/Array.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Color.h"
#include "Bang/Debug.h"
#include "Bang/Extensions.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/List.tcc"
#include "Bang/Material.h"
#include "Bang/Mesh.h"
#include "Bang/MeshRenderer.h"
#include "Bang/Model.h"
#include "Bang/Path.h"
#include "Bang/Quaternion.h"
#include "Bang/StreamOperators.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/Tree.h"
#include "Bang/Tree.tcc"
#include "Bang/UMap.h"
#include "Bang/UMap.tcc"
#include "Bang/USet.h"
#include "Bang/USet.tcc"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

// Conversion functions =======================
String AiStringToString(const aiString &str)
{
    return String(str.C_Str());
}
Vector3 AiVec3ToVec3(const aiVector3D &v)
{
    return Vector3(v.x, v.y, v.z);
}
Quaternion AiQuatToQuat(const aiQuaternion &q)
{
    return Quaternion(q.x, q.y, q.z, q.w);
}
Color AiColor3ToColor(const aiColor3D &c)
{
    return Color(c.r, c.g, c.b, 1);
}
Color AiColor4ToColor(const aiColor4D &c)
{
    return Color(c.r, c.g, c.b, c.a);
}
aiVector3D VectorToAiVec3(const Vector3 &v)
{
    return aiVector3D(v.x, v.y, v.z);
}
aiString StringToAiString(const String &str)
{
    return aiString(str.ToCString());
}
aiColor3D ColorToAiColor3(const Color &c)
{
    return aiColor3D(c.r, c.g, c.b);
}
aiColor4D ColorToAiColor4(const Color &c)
{
    return aiColor4D(c.r, c.g, c.b, c.a);
}
aiMatrix4x4 Matrix4ToAiMatrix4(const Matrix4 &m)
{
    return aiMatrix4x4(m[0][0],
                       m[1][0],
                       m[2][0],
                       m[3][0],
                       m[0][1],
                       m[1][1],
                       m[2][1],
                       m[3][1],
                       m[0][2],
                       m[1][2],
                       m[2][2],
                       m[3][2],
                       m[0][3],
                       m[1][3],
                       m[2][3],
                       m[3][3]);
}
Matrix4 AiMatrix4ToMatrix4(const aiMatrix4x4 &m)
{
    return Matrix4(m[0][0],
                   m[0][1],
                   m[0][2],
                   m[0][3],
                   m[1][0],
                   m[1][1],
                   m[1][2],
                   m[1][3],
                   m[2][0],
                   m[2][1],
                   m[2][2],
                   m[2][3],
                   m[3][0],
                   m[3][1],
                   m[3][2],
                   m[3][3]);
}
// ==================================================

Tree<ModelIONode> *ReadModelNode(const aiScene *scene, aiNode *node)
{
    Tree<ModelIONode> *modelNodeTree = new Tree<ModelIONode>();
    ModelIONode &modelNode = modelNodeTree->GetData();

    modelNode.name = AiStringToString(node->mName);
    modelNode.localToParent = AiMatrix4ToMatrix4(node->mTransformation);

    // Set mesh indices
    for (uint i = 0; i < node->mNumMeshes; ++i)
    {
        uint meshIndex = node->mMeshes[i];
        modelNode.meshIndices.PushBack(meshIndex);
        modelNode.meshMaterialIndices.PushBack(
            scene->mMeshes[meshIndex]->mMaterialIndex);
    }

    for (uint i = 0; i < node->mNumChildren; ++i)
    {
        aiNode *child = node->mChildren[i];
        Tree<ModelIONode> *childModelTree = ReadModelNode(scene, child);
        childModelTree->SetParent(modelNodeTree);
    }

    return modelNodeTree;
}

bool ModelIO::ImportModel(const Path &modelFilepath,
                          Model *model,
                          ModelIOScene *modelScene)
{
    Assimp::Importer importer;
    const aiScene *aScene = ImportScene(&importer, modelFilepath);
    if (!aScene)
    {
        return false;
    }

    // Load materials
    for (int i = 0; i < SCAST<int>(aScene->mNumMaterials); ++i)
    {
        String materialName;
        AH<Material> materialAH;
        ModelIO::ImportEmbeddedMaterial(aScene->mMaterials[i],
                                        modelFilepath.GetDirectory(),
                                        model,
                                        &materialAH,
                                        &materialName);
        modelScene->materials.PushBack(materialAH);
        modelScene->materialsNames.PushBack(materialName);
    }

    // Load meshes
    Map<String, Mesh::Bone> allBones;
    for (int i = 0; i < SCAST<int>(aScene->mNumMeshes); ++i)
    {
        AH<Mesh> meshAH;
        String meshName;
        ModelIO::ImportEmbeddedMesh(
            aScene->mMeshes[i], model, &meshAH, &meshName);
        modelScene->meshes.PushBack(meshAH);
        modelScene->meshesNames.PushBack(meshName);

        // Update global bones
        for (const auto &it : meshAH.Get()->GetBonesPool())
        {
            allBones.Add(it.first, it.second);
        }
    }

    // Load animations and store them into arrays
    for (int i = 0; i < SCAST<int>(aScene->mNumAnimations); ++i)
    {
        aiAnimation *aAnimation = aScene->mAnimations[i];
        String animationName = AiStringToString(aAnimation->mName);
        if (animationName.IsEmpty())
        {
            animationName = "Animation";
        }
        animationName.Append("." + Extensions::GetAnimationExtension());
        animationName = Path::GetDuplicateStringWithExtension(
            animationName, model->GetAnimationsNames());
        AH<Animation> animationAH =
            Assets::CreateEmbeddedAsset<Animation>(model, animationName);
        Animation *animation = animationAH.Get();
        double animDuration = aAnimation->mDuration;
        animation->SetDurationInFrames(animDuration);
        animation->SetFramesPerSecond(aAnimation->mTicksPerSecond);
        for (int j = 0; j < SCAST<int>(aAnimation->mNumChannels); ++j)
        {
            aiNodeAnim *aNodeAnim = aAnimation->mChannels[j];
            String boneName = AiStringToString(aNodeAnim->mNodeName);

            for (uint k = 0; k < aNodeAnim->mNumPositionKeys;
                 ++k)  // Position keys
            {
                Animation::KeyFrame<Vector3> keyFrame;
                keyFrame.timeInFrames = aNodeAnim->mPositionKeys[k].mTime;
                keyFrame.value =
                    AiVec3ToVec3(aNodeAnim->mPositionKeys[k].mValue);
                animation->AddPositionKeyFrame(boneName, keyFrame);
            }

            for (uint k = 0; k < aNodeAnim->mNumRotationKeys;
                 ++k)  // Rotation keys
            {
                Animation::KeyFrame<Quaternion> keyFrame;
                keyFrame.timeInFrames = aNodeAnim->mRotationKeys[k].mTime;
                keyFrame.value =
                    AiQuatToQuat(aNodeAnim->mRotationKeys[k].mValue);
                animation->AddRotationKeyFrame(boneName, keyFrame);
            }

            for (uint k = 0; k < aNodeAnim->mNumScalingKeys; ++k)  // Scale keys
            {
                Animation::KeyFrame<Vector3> keyFrame;
                keyFrame.timeInFrames = aNodeAnim->mScalingKeys[k].mTime;
                keyFrame.value =
                    AiVec3ToVec3(aNodeAnim->mScalingKeys[k].mValue);
                animation->AddScaleKeyFrame(boneName, keyFrame);
            }
        }
        modelScene->animations.PushBack(animationAH);
        modelScene->animationsNames.PushBack(animationName);
    }

    modelScene->allBones = allBones;
    modelScene->rootGameObjectName = AiStringToString(aScene->mRootNode->mName);
    modelScene->modelTree = ReadModelNode(aScene, aScene->mRootNode);

    return true;
}

void ModelIO::ImportMeshRaw(aiMesh *aMesh,
                            Array<Mesh::VertexId> *vertexIndices,
                            Array<Vector3> *vertexPositionsPool,
                            Array<Vector3> *vertexNormalsPool,
                            Array<Vector2> *vertexUvsPool,
                            Array<Vector3> *vertexTangentsPool,
                            Map<String, Mesh::Bone> *bones,
                            Map<String, uint> *bonesIndices)
{
    for (uint i = 0; i < aMesh->mNumFaces; ++i)
    {
        for (uint j = 0; j < aMesh->mFaces[i].mNumIndices; ++j)
        {
            Mesh::VertexId vIndex = aMesh->mFaces[i].mIndices[j];
            vertexIndices->PushBack(vIndex);
        }
    }

    // Positions
    for (uint i = 0; i < aMesh->mNumVertices; ++i)
    {
        Vector3 pos = AiVec3ToVec3(aMesh->mVertices[i]);
        vertexPositionsPool->PushBack(pos);
    }

    // Normals
    for (uint i = 0; i < aMesh->mNumVertices; ++i)
    {
        Vector3 normal = AiVec3ToVec3(aMesh->mNormals[i]);
        vertexNormalsPool->PushBack(normal);
    }

    // Uvs
    if (aMesh->GetNumUVChannels() > 0)
    {
        for (uint i = 0; i < aMesh->mNumVertices; ++i)
        {
            Vector3 uvs = AiVec3ToVec3(aMesh->mTextureCoords[0][i]);
            vertexUvsPool->PushBack(uvs.xy());
        }
    }

    // Tangents
    if (aMesh->HasTangentsAndBitangents())
    {
        for (uint i = 0; i < aMesh->mNumVertices; ++i)
        {
            Vector3 tangent = AiVec3ToVec3(aMesh->mTangents[i]);
            vertexTangentsPool->PushBack(tangent);
        }
    }

    // Bones
    if (aMesh->HasBones())
    {
        for (uint boneIdx = 0; boneIdx < aMesh->mNumBones; ++boneIdx)
        {
            Mesh::Bone bone;
            aiBone *aBone = aMesh->mBones[boneIdx];
            String boneName = AiStringToString(aBone->mName);

            // Set weights
            for (uint j = 0; j < aBone->mNumWeights; ++j)
            {
                const aiVertexWeight &aVertWeight = aBone->mWeights[j];
                bone.weights.Add(aVertWeight.mVertexId, aVertWeight.mWeight);
            }
            bone.rootSpaceToBoneBindSpaceTransformation =
                Transformation(AiMatrix4ToMatrix4(aBone->mOffsetMatrix));

            bonesIndices->Add(boneName, boneIdx);
            bones->Add(boneName, bone);
        }
    }
}

void ModelIO::ExportModel(const GameObject *rootGameObject,
                          const Path &meshExportPath)
{
#ifndef FROM_TRAVIS
    Assimp::Exporter exporter;
    aiScene scene;

    // Create scene meshes, materials and textures
    USet<Mesh *> sceneMeshes;
    USet<Material *> sceneMaterials;
    USet<Texture2D *> sceneTextures;
    UMap<Mesh *, Material *> sceneMeshToMaterial;
    UMap<MeshRenderer *, Mesh *> meshRendererToMesh;
    UMap<MeshRenderer *, Material *> meshRendererToMaterial;
    {
        Array<MeshRenderer *> rootMRs =
            rootGameObject->GetComponentsInDescendantsAndThis<MeshRenderer>();
        for (MeshRenderer *mr : rootMRs)
        {
            if (Mesh *mesh = mr->GetCurrentLODActiveMesh())
            {
                sceneMeshes.Add(mesh);
                meshRendererToMesh.Add(mr, mesh);

                Material *material = mr->GetActiveMaterial();
                if (material)
                {
                    sceneMeshToMaterial.Add(mesh, material);

                    sceneMaterials.Add(material);
                    meshRendererToMaterial.Add(mr, material);

                    if (material->GetAlbedoTexture())
                    {
                        sceneTextures.Add(material->GetAlbedoTexture());
                    }
                }
            }
        }
    }

    if (sceneMeshes.IsEmpty())
    {
        return;
    }

    Array<Mesh *> sceneMeshesArray(sceneMeshes.Begin(), sceneMeshes.End());
    Array<Texture2D *> sceneTexturesArray(sceneTextures.Begin(),
                                          sceneTextures.End());
    Array<Material *> sceneMaterialsArray;
    if (sceneMaterials.Size() > 0)
    {
        sceneMaterialsArray =
            Array<Material *>(sceneMaterials.Begin(), sceneMaterials.End());
    }

    // Create materials
    scene.mNumMaterials = sceneMaterialsArray.Size();
    if (scene.mNumMaterials > 0)
    {
        scene.mMaterials = new aiMaterial *[scene.mNumMaterials];
        for (uint i = 0; i < scene.mNumMaterials; ++i)
        {
            Material *material = sceneMaterialsArray[i];
            aiMaterial *aMaterial = MaterialToAiMaterial(material);
            scene.mMaterials[i] = aMaterial;
        }
    }

    // Create meshes
    scene.mNumMeshes = sceneMeshesArray.Size();
    if (scene.mNumMeshes > 0)
    {
        scene.mMeshes = new aiMesh *[scene.mNumMeshes];
        for (uint i = 0; i < scene.mNumMeshes; ++i)
        {
            Mesh *mesh = sceneMeshesArray[i];
            aiMesh *aMesh = MeshToAiMesh(mesh);
            if (sceneMeshToMaterial.ContainsKey(mesh))
            {
                Material *meshMaterial = sceneMeshToMaterial[mesh];
                aMesh->mMaterialIndex =
                    sceneMaterialsArray.IndexOf(meshMaterial);
            }
            scene.mMeshes[i] = aMesh;
        }
    }

    // scene.mNumTextures = sceneTexturesArray.Size();
    // scene.mTextures = new aiTexture*[scene.mNumTextures];

    scene.mRootNode = GameObjectToAiNode(rootGameObject, sceneMeshesArray);

    const String extension = meshExportPath.GetExtension();
    const String extensionId =
        GetExtensionIdFromExtension(extension).ToCString();
    const int result = exporter.Export(
        &scene, extensionId, meshExportPath.GetAbsolute().ToCString());

    if (result != AI_SUCCESS)
    {
        Debug_Error("Error exporting to " << meshExportPath << ": "
                                          << exporter.GetErrorString());
    }
#endif
}

String ModelIO::GetExtensionIdFromExtension(const String &extension)
{
#ifndef FROM_TRAVIS
    Assimp::Exporter exporter;
    for (uint i = 0; i < exporter.GetExportFormatCount(); ++i)
    {
        const aiExportFormatDesc *fmtDesc =
            exporter.GetExportFormatDescription(i);
        if (extension.EqualsNoCase(fmtDesc->fileExtension))
        {
            return String(fmtDesc->id);
        }
    }
    Debug_Error("Can't export mesh to extension '" << extension << "'");
#endif
    return 0;
}

aiNode *ModelIO::GameObjectToAiNode(const GameObject *gameObject,
                                    const Array<Mesh *> &sceneMeshes)
{
    aiNode *goNode = new aiNode();
    goNode->mName = gameObject->GetName();
    goNode->mNumChildren = gameObject->GetChildren().Size();

    // Count number of meshes
    Array<MeshRenderer *> mrs = gameObject->GetComponents<MeshRenderer>();

    // Meshes
    goNode->mNumMeshes = 0;  // Count number of meshes
    for (MeshRenderer *mr : mrs)
    {
        goNode->mNumMeshes += (mr->GetCurrentLODActiveMesh() ? 1 : 0);
    }
    if (goNode->mNumMeshes > 0)
    {
        goNode->mMeshes = new unsigned int[goNode->mNumMeshes];
    }

    int i = 0;  // Populate meshes indices
    for (MeshRenderer *mr : mrs)
    {
        if (mr->GetCurrentLODActiveMesh())
        {
            goNode->mMeshes[i] =
                sceneMeshes.IndexOf(mr->GetCurrentLODActiveMesh());
        }
    }

    // Add children if any
    if (goNode->mNumChildren > 0)
    {
        goNode->mChildren = new aiNode *[goNode->mNumChildren];
        for (uint i = 0; i < goNode->mNumChildren; ++i)
        {
            GameObject *child = gameObject->GetChild(i);
            aiNode *childNode = GameObjectToAiNode(child, sceneMeshes);
            goNode->mChildren[i] = childNode;
            childNode->mParent = goNode;
        }
    }

    if (Transform *tr = gameObject->GetTransform())
    {
        goNode->mTransformation =
            Matrix4ToAiMatrix4(tr->GetLocalToParentMatrix());
    }

    return goNode;
}

aiMesh *ModelIO::MeshToAiMesh(const Mesh *mesh)
{
    aiMesh *aMesh = new aiMesh();
    aMesh->mNumVertices = mesh->GetPositionsPool().Size();
    if (aMesh->mNumVertices > 0)
    {
        aMesh->mPrimitiveTypes = 1;
        aMesh->mNumUVComponents[0] = 1;
        aMesh->mVertices = new aiVector3D[aMesh->mNumVertices];
        aMesh->mNormals = new aiVector3D[aMesh->mNumVertices];
        aMesh->mTextureCoords[0] = new aiVector3D[aMesh->mNumVertices];

        // Populate vertices
        for (uint i = 0; i < aMesh->mNumVertices; ++i)
        {
            Vector3 posi = (mesh->GetPositionsPool().Size() > i
                                ? mesh->GetPositionsPool()[i]
                                : Vector3::Zero());
            aMesh->mVertices[i] = VectorToAiVec3(posi);

            Vector3 normali =
                (mesh->GetNormalsPool().Size() > i ? mesh->GetNormalsPool()[i]
                                                   : Vector3::Zero());
            aMesh->mNormals[i] = VectorToAiVec3(normali);

            Vector3 texCoordi = (mesh->GetUvsPool().Size() > i
                                     ? Vector3(mesh->GetUvsPool()[i], 0)
                                     : Vector3::Zero());
            aMesh->mTextureCoords[0][i] = VectorToAiVec3(texCoordi);
        }

        aMesh->mNumFaces = mesh->GetNumTriangles();
        aMesh->mFaces = new aiFace[aMesh->mNumFaces];
        for (uint tri = 0; tri < aMesh->mNumFaces; ++tri)
        {
            aiFace *aFace = &(aMesh->mFaces[tri]);
            aFace->mNumIndices = 3;
            aFace->mIndices = new unsigned int[aFace->mNumIndices];
            const auto triIndices = mesh->GetVertexIdsFromTriangle(tri);
            for (int i = 0; i < 3; ++i)
            {
                aFace->mIndices[i] = triIndices[i];
            }
        }
    }
    return aMesh;
}

aiMaterial *ModelIO::MaterialToAiMaterial(const Material *material)
{
#ifndef FROM_TRAVIS
    aiMaterial *aMaterial = new aiMaterial();
    // aMaterial->mNumProperties = 1;
    // aMaterial->mProperties = new
    // aiMaterialProperty*[aMaterial->mNumProperties];

    aiColor3D diffColor = ColorToAiColor3(Color::Red());
    aMaterial->AddProperty(&diffColor, 1, AI_MATKEY_COLOR_DIFFUSE); /*
     aMaterial->mProperties[0] = new aiMaterialProperty();
     aMaterial->mProperties[0]->mKey = AI_MATKEY_COLOR_DIFFUSE;
     aMaterial->mProperties[0]->*/
    return aMaterial;
#else
    return nullptr;
#endif
}

void ModelIO::ImportEmbeddedMaterial(aiMaterial *aMaterial,
                                     const Path &modelDirectory,
                                     Model *model,
                                     AH<Material> *outMaterial,
                                     String *outMaterialName)
{
    aiString aMatName;
    aiGetMaterialString(aMaterial, AI_MATKEY_NAME, &aMatName);
    String materialName = AiStringToString(aMatName);
    if (materialName.IsEmpty())
    {
        materialName = "Material";
    }
    materialName.Append("." + Extensions::GetMaterialExtension());
    materialName = Path::GetDuplicateStringWithExtension(
        materialName, model->GetMaterialsNames());

    *outMaterialName = materialName;
    *outMaterial = Assets::CreateEmbeddedAsset<Material>(model, materialName);

    aiColor3D aAmbientColor = aiColor3D(0.0f, 0.0f, 0.0f);
    aiColor3D aDiffuseColor = aiColor3D(1.0f, 1.0f, 1.0f);
    aMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aAmbientColor);
    aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aDiffuseColor);

    Color albedoColor = AiColor3ToColor(aDiffuseColor);

    aiString aAlbedoTexturePath;
    aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aAlbedoTexturePath);
    Path albedoTexturePath(String(aAlbedoTexturePath.C_Str()));
    albedoTexturePath = modelDirectory.Append(albedoTexturePath);
    AH<Texture2D> matAlbedoTexture;
    if (albedoTexturePath.IsFile())
    {
        matAlbedoTexture = Assets::Load<Texture2D>(albedoTexturePath);
    }
    if (albedoTexturePath.HasExtension("dds"))
    {
        outMaterial->Get()->SetAlbedoUvMultiply(Vector2(1, -1));
    }

    aiString aNormalsTexturePath;
    aMaterial->GetTexture(aiTextureType_NORMALS, 0, &aNormalsTexturePath);
    Path normalsTexturePath(String(aNormalsTexturePath.C_Str()));
    normalsTexturePath = modelDirectory.Append(normalsTexturePath);
    AH<Texture2D> matNormalTexture;
    if (normalsTexturePath.IsFile())
    {
        matNormalTexture = Assets::Load<Texture2D>(normalsTexturePath);
    }
    if (normalsTexturePath.HasExtension("dds"))
    {
        outMaterial->Get()->SetNormalMapUvMultiply(Vector2(1, -1));
    }

    outMaterial->Get()->SetRoughness(0.5f);
    outMaterial->Get()->SetMetalness(0.1f);
    outMaterial->Get()->SetAlbedoTexture(matAlbedoTexture.Get());
    outMaterial->Get()->SetNormalMapTexture(matNormalTexture.Get());
    outMaterial->Get()->SetAlbedoColor(matAlbedoTexture ? Color::White()
                                                        : albedoColor);
}

void ModelIO::ImportEmbeddedMesh(aiMesh *aMesh,
                                 Model *model,
                                 AH<Mesh> *outMeshAH,
                                 String *outMeshName)
{
    aiString aMeshName = aMesh->mName;
    String meshName = AiStringToString(aMeshName);
    if (meshName.IsEmpty())
    {
        meshName = "Mesh";
    }
    meshName.Append("." + Extensions::GetMeshExtension());
    meshName = Path::GetDuplicateStringWithExtension(meshName,
                                                     model->GetMeshesNames());

    *outMeshAH = Assets::CreateEmbeddedAsset<Mesh>(model, meshName);
    *outMeshName = meshName;

    Array<Mesh::VertexId> vertexIndices;
    Array<Vector3> vertexPositionsPool;
    Array<Vector3> vertexNormalsPool;
    Array<Vector2> vertexUvsPool;
    Array<Vector3> vertexTangentsPool;
    Map<String, Mesh::Bone> bonesPool;
    Map<String, uint> bonesIndices;

    ModelIO::ImportMeshRaw(aMesh,
                           &vertexIndices,
                           &vertexPositionsPool,
                           &vertexNormalsPool,
                           &vertexUvsPool,
                           &vertexTangentsPool,
                           &bonesPool,
                           &bonesIndices);

    Mesh *outMesh = outMeshAH->Get();
    outMesh->SetPositionsPool(vertexPositionsPool);
    outMesh->SetNormalsPool(vertexNormalsPool);
    outMesh->SetUvsPool(vertexUvsPool);
    outMesh->SetTangentsPool(vertexTangentsPool);
    outMesh->SetTrianglesVertexIds(vertexIndices);
    outMesh->SetBonesPool(bonesPool);
    outMesh->SetBonesIds(bonesIndices);
    outMesh->UpdateVAOs();
}

const aiScene *ModelIO::ImportScene(Assimp::Importer *importer,
                                    const Path &modelFilepath)
{
    if (!modelFilepath.IsFile())
    {
        return nullptr;
    }

    const aiScene *scene = importer->ReadFile(
        modelFilepath.GetAbsolute().ToCString(),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

    String errorStr = importer->GetErrorString();
    if (!errorStr.IsEmpty())
    {
        Debug_Error(errorStr);
    }

    return scene;
}

ModelIOScene::~ModelIOScene()
{
    Clear();
}

void ModelIOScene::Clear()
{
    meshes.Clear();
    meshesNames.Clear();
    materials.Clear();
    materialsNames.Clear();
    animations.Clear();
    animationsNames.Clear();

    if (modelTree)
    {
        delete modelTree;
        modelTree = nullptr;
    }
}
