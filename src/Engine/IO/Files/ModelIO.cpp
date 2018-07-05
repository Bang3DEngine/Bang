#include "Bang/ModelIO.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <iostream>

// Avoid asserts when reading meshes...
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#ifndef FROM_TRAVIS
#include <assimp/Exporter.hpp>
#endif

#include "Bang/UMap.h"
#include "Bang/USet.h"
#include "Bang/Path.h"
#include "Bang/Mesh.h"
#include "Bang/Model.h"
#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/Debug.h"
#include "Bang/Array.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/Texture2D.h"
#include "Bang/Extensions.h"
#include "Bang/GameObject.h"
#include "Bang/MeshRenderer.h"
#include "Bang/XMLNodeReader.h"

USING_NAMESPACE_BANG

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
    return  aiMatrix4x4(m[0][0], m[1][0], m[2][0], m[3][0],
                        m[0][1], m[1][1], m[2][1], m[3][1],
                        m[0][2], m[1][2], m[2][2], m[3][2],
                        m[0][3], m[1][3], m[2][3], m[3][3]);
}
Matrix4 AiMatrix4ToMatrix4(const aiMatrix4x4 &m)
{
    return Matrix4(m[0][0], m[0][1], m[0][2], m[0][3],
                   m[1][0], m[1][1], m[1][2], m[1][3],
                   m[2][0], m[2][1], m[2][2], m[2][3],
                   m[3][0], m[3][1], m[3][2], m[3][3]);
}
// ==================================================

int ModelIO::GetModelNumTriangles(const Path &modelFilepath)
{
    Assimp::Importer importer;
    const aiScene* scene = ImportScene(&importer, modelFilepath);
    if (!scene) { return 0; }

    if (scene && scene->HasMeshes()) { return scene->mMeshes[0]->mNumFaces; }
    return 0;
}

Tree<ModelIONode>* ReadModelNode(const aiScene *scene,
                                 aiNode *node)
{
    Tree<ModelIONode>* modelNodeTree = new Tree<ModelIONode>();
    ModelIONode &modelNode = modelNodeTree->GetData();

    modelNode.name = AiStringToString(node->mName);
    modelNode.transformation = AiMatrix4ToMatrix4(node->mTransformation);

    // Set mesh indices
    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        uint meshIndex = node->mMeshes[i];
        modelNode.meshIndices.PushBack(meshIndex);
    }

    for (int i = 0; i < node->mNumChildren; ++i)
    {
        aiNode *child = node->mChildren[i];
        Tree<ModelIONode> *childModelTree = ReadModelNode(scene, child);
        childModelTree->SetParent(modelNodeTree);
    }

    return modelNodeTree;
}

bool ModelIO::ImportModel(const Path& modelFilepath,
                          Model *model,
                          ModelIOScene *modelScene)
{
    Assimp::Importer importer;
    const aiScene* aScene = ImportScene(&importer, modelFilepath);
    if (!aScene) { return false; }

    // Load materials
    Array< String > unorderedMaterialNames;
    Array< RH<Material> > unorderedMaterials;
    for (int i = 0; i < SCAST<int>(aScene->mNumMaterials); ++i)
    {
        String materialName;
        RH<Material> materialRH;
        ModelIO::ImportEmbeddedMaterial(aScene->mMaterials[i],
                                        modelFilepath.GetDirectory(),
                                        model,
                                        &materialRH,
                                        &materialName);

        unorderedMaterials.PushBack(materialRH);
        unorderedMaterialNames.PushBack(materialName);
    }

    // Load meshes and store them into arrays
    for (int i = 0; i < SCAST<int>(aScene->mNumMeshes); ++i)
    {
        RH<Mesh> meshRH;
        String meshName;
        ModelIO::ImportEmbeddedMesh(aScene->mMeshes[i], model, &meshRH, &meshName);

        int matIndex = aScene->mMeshes[i]->mMaterialIndex;
        RH<Material> mat = unorderedMaterials[matIndex];
        const String &materialName = unorderedMaterialNames[matIndex];

        modelScene->meshes.PushBack(meshRH);
        modelScene->meshesNames.PushBack(meshName);

        modelScene->materials.PushBack(mat);
        modelScene->materialsNames.PushBack(materialName);
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
        RH<Animation> animationRH = Resources::CreateEmbeddedResource<Animation>(
                                                    model,
                                                    animationName);
        Animation *animation = animationRH.Get();
        animation->SetDuration(aAnimation->mDuration);
        animation->SetFramesPerSecond(aAnimation->mTicksPerSecond);
        for (int j = 0; j < SCAST<int>(aAnimation->mNumChannels); ++j)
        {
            aiNodeAnim *aNodeAnim = aAnimation->mChannels[j];
            for (int k = 0; k < aNodeAnim->mNumPositionKeys; ++k) // Position keys
            {
                Animation::KeyFrame<Vector3> keyFrame;
                keyFrame.time = aNodeAnim->mPositionKeys[k].mTime;
                keyFrame.value = AiVec3ToVec3(aNodeAnim->mPositionKeys[k].mValue);
                animation->AddPositionKeyFrame(keyFrame);
            }

            for (int k = 0; k < aNodeAnim->mNumRotationKeys; ++k) // Rotation keys
            {
                Animation::KeyFrame<Quaternion> keyFrame;
                keyFrame.time = aNodeAnim->mRotationKeys[k].mTime;
                keyFrame.value = AiQuatToQuat(aNodeAnim->mRotationKeys[k].mValue);
                animation->AddRotationKeyFrame(keyFrame);
            }

            for (int k = 0; k < aNodeAnim->mNumScalingKeys; ++k) // Scale keys
            {
                Animation::KeyFrame<Vector3> keyFrame;
                keyFrame.time = aNodeAnim->mScalingKeys[k].mTime;
                keyFrame.value = AiVec3ToVec3(aNodeAnim->mScalingKeys[k].mValue);
                animation->AddScaleKeyFrame(keyFrame);
            }
        }
        modelScene->animations.PushBack(animationRH);
        modelScene->animationsNames.PushBack(animationName);
    }

    modelScene->modelTree = ReadModelNode(aScene, aScene->mRootNode);

    return true;
}

void ModelIO::ImportMeshRaw(
                  aiMesh *aMesh,
                  Array<Mesh::VertexId> *vertexIndices,
                  Array<Vector3> *vertexPositionsPool,
                  Array<Vector3> *vertexNormalsPool,
                  Array<Vector2> *vertexUvsPool,
                  Array<Vector3> *vertexTangentsPool,
                  Map<String, Mesh::Bone> *bones)
{
    for (int i = 0; i < SCAST<int>(aMesh->mNumFaces); ++i)
    {
        for (int j = 0; j < aMesh->mFaces[i].mNumIndices; ++j)
        {
            Mesh::VertexId vIndex = aMesh->mFaces[i].mIndices[j];
            vertexIndices->PushBack(vIndex);
        }
    }

    // Positions
    for (int i = 0; i < SCAST<int>(aMesh->mNumVertices); ++i)
    {
        Vector3 pos = AiVec3ToVec3(aMesh->mVertices[i]);
        vertexPositionsPool->PushBack(pos);
    }

    // Normals
    for (int i = 0; i < SCAST<int>(aMesh->mNumVertices); ++i)
    {
        Vector3 normal = AiVec3ToVec3(aMesh->mNormals[i]);
        vertexNormalsPool->PushBack(normal);
    }

    // Uvs
    if (aMesh->GetNumUVChannels() > 0)
    {
        for (int i = 0; i < SCAST<int>(aMesh->mNumVertices); ++i)
        {
            Vector3 uvs = AiVec3ToVec3(aMesh->mTextureCoords[0][i]);
            vertexUvsPool->PushBack( uvs.xy() );
        }
    }

    // Tangents
    if (aMesh->HasTangentsAndBitangents())
    {
        for (int i = 0; i < SCAST<int>(aMesh->mNumVertices); ++i)
        {
            Vector3 tangent = AiVec3ToVec3(aMesh->mTangents[i]);
            vertexTangentsPool->PushBack( tangent );
        }
    }

    // Bones
    // Debug_Peek(aMesh->mNumAnimMeshes);
    // Debug_Peek(aMesh->mNumBones);
    if (aMesh->HasBones())
    {
        for (int i = 0; i < SCAST<int>(aMesh->mNumBones); ++i)
        {
            Mesh::Bone bone;
            aiBone *aBone = aMesh->mBones[i];
            String boneName = AiStringToString(aBone->mName);
            {
                for (int j = 0; j < aBone->mNumWeights; ++j) // Set weights
                {
                    const aiVertexWeight &aVertWeight = aBone->mWeights[j];
                    bone.weights.Add(aVertWeight.mVertexId, aVertWeight.mWeight);
                }
                bone.transform = AiMatrix4ToMatrix4(aBone->mOffsetMatrix);
            }
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
    USet<Mesh*> sceneMeshes;
    USet<Material*> sceneMaterials;
    USet<Texture2D*> sceneTextures;
    UMap<Mesh*, Material*> sceneMeshToMaterial;
    UMap<MeshRenderer*, Mesh*> meshRendererToMesh;
    UMap<MeshRenderer*, Material*> meshRendererToMaterial;
    {
        List<MeshRenderer*> rootMRs =
                   rootGameObject->GetComponentsInChildren<MeshRenderer>(true);
        for (MeshRenderer *mr : rootMRs)
        {
            Mesh *mesh = mr->GetCurrentLODActiveMesh();
            if (mesh)
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

    if (sceneMeshes.IsEmpty()) { return; }

    Array<Mesh*> sceneMeshesArray(sceneMeshes.Begin(), sceneMeshes.End());
    Array<Texture2D*> sceneTexturesArray(sceneTextures.Begin(), sceneTextures.End());
    Array<Material*> sceneMaterialsArray;
    if (sceneMaterials.Size() > 0)
    {
        sceneMaterialsArray = Array<Material*>(sceneMaterials.Begin(),
                                               sceneMaterials.End());
    }

    // Create materials
    scene.mNumMaterials = sceneMaterialsArray.Size();
    if (scene.mNumMaterials > 0)
    {
        scene.mMaterials = new aiMaterial*[scene.mNumMaterials];
        for (int i = 0; i < scene.mNumMaterials; ++i)
        {
            Material *material = sceneMaterialsArray[i];
            aiMaterial *aMaterial = MaterialToAiMaterial(material);;
            scene.mMaterials[i] = aMaterial;
        }
    }

    // Create meshes
    scene.mNumMeshes = sceneMeshesArray.Size();
    if (scene.mNumMeshes > 0)
    {
        scene.mMeshes = new aiMesh*[scene.mNumMeshes];
        for (int i = 0; i < scene.mNumMeshes; ++i)
        {
            Mesh *mesh = sceneMeshesArray[i];
            aiMesh *aMesh = MeshToAiMesh(mesh);
            if (sceneMeshToMaterial.ContainsKey(mesh))
            {
                Material *meshMaterial = sceneMeshToMaterial[mesh];
                aMesh->mMaterialIndex = sceneMaterialsArray.IndexOf(meshMaterial);
            }
            scene.mMeshes[i] = aMesh;
        }
    }

    // scene.mNumTextures = sceneTexturesArray.Size();
    // scene.mTextures = new aiTexture*[scene.mNumTextures];

    scene.mRootNode = GameObjectToAiNode(rootGameObject, sceneMeshesArray);

    const String extension = meshExportPath.GetExtension();
    const String extensionId = GetExtensionIdFromExtension(extension).ToCString();
    const int result =
        exporter.Export(&scene,
                        extensionId,
                        meshExportPath.GetAbsolute().ToCString());

    if (result != AI_SUCCESS)
    {
        Debug_Error("Error exporting to " << meshExportPath << ": " <<
                    exporter.GetErrorString());
    }
#endif
}

String ModelIO::GetExtensionIdFromExtension(const String &extension)
{
#ifndef FROM_TRAVIS
    Assimp::Exporter exporter;
    for (int i = 0; i < exporter.GetExportFormatCount(); ++i)
    {
        const aiExportFormatDesc *fmtDesc = exporter.GetExportFormatDescription(i);
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
                                    const Array<Mesh*> &sceneMeshes)
{
    aiNode *goNode = new aiNode();
    goNode->mName = gameObject->GetName();
    goNode->mNumChildren = gameObject->GetChildren().Size();

    // Count number of meshes
    const List<MeshRenderer*> &mrs = gameObject->GetComponents<MeshRenderer>();

    // Meshes
    goNode->mNumMeshes = 0; // Count number of meshes
    for (MeshRenderer *mr : mrs)
    {
        goNode->mNumMeshes += (mr->GetCurrentLODActiveMesh() ? 1 : 0);
    }
    if (goNode->mNumMeshes > 0)
    {
        goNode->mMeshes = new unsigned int[goNode->mNumMeshes];
    }

    int i = 0; // Populate meshes indices
    for (MeshRenderer *mr : mrs)
    {
        if (mr->GetCurrentLODActiveMesh())
        {
            goNode->mMeshes[i] = sceneMeshes.IndexOf(mr->GetCurrentLODActiveMesh());
        }
    }

    // Add children if any
    if (goNode->mNumChildren > 0)
    {
        goNode->mChildren = new aiNode*[goNode->mNumChildren];
        for (int i = 0; i < goNode->mNumChildren; ++i)
        {
            GameObject *child = gameObject->GetChild(i);
            aiNode *childNode = GameObjectToAiNode(child, sceneMeshes);
            goNode->mChildren[i] = childNode;
            childNode->mParent = goNode;
        }
    }

    if (Transform *tr = gameObject->GetTransform())
    {
        goNode->mTransformation = Matrix4ToAiMatrix4(
                                        tr->GetLocalToParentMatrix()
                                  );
    }

    return goNode;
}

aiMesh *ModelIO::MeshToAiMesh(const Mesh *mesh)
{
    aiMesh *aMesh = new aiMesh();
    aMesh->mNumVertices = mesh->GetPositionsPool().Size();
    if (aMesh->mNumVertices > 0)
    {
        aMesh->mPrimitiveTypes     = 1;
        aMesh->mNumUVComponents[0] = 1;
        aMesh->mVertices         = new aiVector3D[aMesh->mNumVertices];
        aMesh->mNormals          = new aiVector3D[aMesh->mNumVertices];
        aMesh->mTextureCoords[0] = new aiVector3D[aMesh->mNumVertices];

         // Populate vertices
        for (int i = 0; i < aMesh->mNumVertices; ++i)
        {
            Vector3 posi = (mesh->GetPositionsPool().Size() > i ?
                                                mesh->GetPositionsPool()[i] :
                                                Vector3::Zero);
            aMesh->mVertices[i] = VectorToAiVec3(posi);

            Vector3 normali = (mesh->GetNormalsPool().Size() > i ?
                                                mesh->GetNormalsPool()[i] :
                                                Vector3::Zero);
            aMesh->mNormals[i]  = VectorToAiVec3(normali);

            Vector3 texCoordi = (mesh->GetUvsPool().Size() > i ?
                                        Vector3(mesh->GetUvsPool()[i], 0) :
                                        Vector3::Zero);
            aMesh->mTextureCoords[0][i] = VectorToAiVec3(texCoordi);
        }

        aMesh->mNumFaces = mesh->GetNumTriangles();
        aMesh->mFaces = new aiFace[aMesh->mNumFaces];
        for(int tri = 0; tri < aMesh->mNumFaces; ++tri)
        {
            aiFace *aFace = &(aMesh->mFaces[tri]);
            aFace->mNumIndices = 3;
            aFace->mIndices = new unsigned int[aFace->mNumIndices];
            const auto triIndices = mesh->GetTriangleVertexIndices(tri);
            for (int i = 0; i < 3; ++i) { aFace->mIndices[i] = triIndices[i]; }
        }
    }
    return aMesh;
}

aiMaterial *ModelIO::MaterialToAiMaterial(const Material *material)
{
#ifndef FROM_TRAVIS
    aiMaterial *aMaterial = new aiMaterial();
    // aMaterial->mNumProperties = 1;
    // aMaterial->mProperties = new aiMaterialProperty*[aMaterial->mNumProperties];

    aiColor3D diffColor = ColorToAiColor3(Color::Red);
    aMaterial->AddProperty(&diffColor, 1, AI_MATKEY_COLOR_DIFFUSE);/*
    aMaterial->mProperties[0] = new aiMaterialProperty();
    aMaterial->mProperties[0]->mKey = AI_MATKEY_COLOR_DIFFUSE;
    aMaterial->mProperties[0]->*/
    return aMaterial;
#else
    return nullptr;
#endif
}

void ModelIO::ImportEmbeddedMaterial(aiMaterial *aMaterial,
                                     const Path& modelDirectory,
                                     Model *model,
                                     RH<Material> *outMaterial,
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

    *outMaterialName = materialName;
    *outMaterial =  Resources::CreateEmbeddedResource<Material>(model,
                                                                materialName);

    float aRoughness = 0.0f;
    aiColor3D aAmbientColor = aiColor3D(0.0f, 0.0f, 0.0f);
    aiColor3D aAlbedoColor  = aiColor3D(1.0f, 1.0f, 1.0f);
    aMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aAmbientColor);
    aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aAlbedoColor);
    aMaterial->Get(AI_MATKEY_REFLECTIVITY, aRoughness);
    aRoughness = Math::Clamp(aRoughness, 0.0f, 1.0f);

    Color albedoColor = AiColor3ToColor(aAlbedoColor);

    aiString aAlbedoTexturePath;
    aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aAlbedoTexturePath);
    Path albedoTexturePath ( String(aAlbedoTexturePath.C_Str()) );
    albedoTexturePath = modelDirectory.Append(albedoTexturePath);
    RH<Texture2D> matAlbedoTexture;
    if (albedoTexturePath.IsFile())
    {
        matAlbedoTexture = Resources::Load<Texture2D>(albedoTexturePath);
    }

    aiString aNormalsTexturePath;
    aMaterial->GetTexture(aiTextureType_NORMALS, 0, &aNormalsTexturePath);
    Path normalsTexturePath ( String(aNormalsTexturePath.C_Str()) );
    normalsTexturePath = modelDirectory.Append(normalsTexturePath);
    RH<Texture2D> matNormalTexture;
    if (normalsTexturePath.IsFile())
    {
        matNormalTexture = Resources::Load<Texture2D>(normalsTexturePath);
    }
    outMaterial->Get()->SetRoughness( aRoughness );
    outMaterial->Get()->SetAlbedoTexture( matAlbedoTexture.Get() );
    outMaterial->Get()->SetNormalMapTexture( matNormalTexture.Get() );
    outMaterial->Get()->SetAlbedoColor( albedoColor );
}

void ModelIO::ImportEmbeddedMesh(aiMesh *aMesh,
                                 Model *model,
                                 RH<Mesh> *outMeshRH,
                                 String *outMeshName)
{
    aiString aMeshName = aMesh->mName;
    String meshName = AiStringToString(aMeshName);
    if (meshName.IsEmpty())
    {
        meshName = "Mesh";
    }
    meshName.Append("." + Extensions::GetMeshExtension());

    *outMeshRH =  Resources::CreateEmbeddedResource<Mesh>(model, meshName);
    *outMeshName = meshName;

    Array<Mesh::VertexId> vertexIndices;
    Array<Vector3> vertexPositionsPool;
    Array<Vector3> vertexNormalsPool;
    Array<Vector2> vertexUvsPool;
    Array<Vector3> vertexTangentsPool;
    Map<String, Mesh::Bone> bonesPool;

    ModelIO::ImportMeshRaw(aMesh,
                           &vertexIndices,
                           &vertexPositionsPool,
                           &vertexNormalsPool,
                           &vertexUvsPool,
                           &vertexTangentsPool,
                           &bonesPool);

    Mesh *outMesh = outMeshRH->Get();
    outMesh->SetPositionsPool(vertexPositionsPool);
    outMesh->SetNormalsPool(vertexNormalsPool);
    outMesh->SetUvsPool(vertexUvsPool);
    outMesh->SetTangentsPool(vertexTangentsPool);
    outMesh->SetVertexIndices(vertexIndices);
    outMesh->SetBonesPool(bonesPool);
    outMesh->UpdateVAOs();
}

const aiScene *ModelIO::ImportScene(Assimp::Importer *importer,
                                    const Path &modelFilepath)
{
    const aiScene* scene =
      importer->ReadFile(modelFilepath.GetAbsolute().ToCString(),
                         aiProcess_Triangulate            |
                         aiProcess_JoinIdenticalVertices  |
                         aiProcess_GenSmoothNormals       |
                         aiProcess_CalcTangentSpace);

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
