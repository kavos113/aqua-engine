#ifndef FBXMANAGER_H
#define FBXMANAGER_H

#include <print>
#include <fbxsdk.h>

namespace AquaEngine {

    class FBXManager {
    public:
        static void Init()
        {
            CreateManager();
        }

        static FbxManager* Get()
        {
            return lSdkManager;
        }

        static void Shutdown()
        {
            lSdkManager->Destroy();
        }

        static int ReadFile(const char* filename, FbxScene** scene)
        {
            if (!FbxFileUtils::Exist(filename))
            {
                std::println("FBX file not found: {}", filename);
                return -1;
            }

            FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
            lSdkManager->SetIOSettings(ios);

            FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

            if (!lImporter->Initialize(filename, -1, lSdkManager->GetIOSettings()))
            {
                std::println("Failed to initialize FBX importer: {}", filename);
                std::println("Error: {}", lImporter->GetStatus().GetErrorString());
                return -1;
            }

            *scene = FbxScene::Create(lSdkManager, filename);
            lImporter->Import(*scene);
            lImporter->Destroy();

            FbxGeometryConverter lGeomConverter(lSdkManager);
            lGeomConverter.Triangulate(*scene, true);

            FbxAxisSystem sceneAxisSystem = (*scene)->GetGlobalSettings().GetAxisSystem();
            FbxAxisSystem directXAxisSystem(FbxAxisSystem::eDirectX);
            if (sceneAxisSystem != directXAxisSystem)
            {
                directXAxisSystem.ConvertScene(*scene);
            }

            return 0;
        }

    private:
        static FbxManager* lSdkManager;

        static void CreateManager() {
            lSdkManager = FbxManager::Create();
            if (!lSdkManager) {
                std::println("Failed to create FBX manager.");
            }
        }
    };

} // AquaEngine

#endif //FBXMANAGER_H
