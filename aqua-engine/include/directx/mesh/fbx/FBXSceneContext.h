#ifndef FBXSCENECONTEXT_H
#define FBXSCENECONTEXT_H
#include <fbxsdk.h>
#include <windows.h>


namespace AquaEngine
{
    class FBXSceneContext {
    public:
        enum class Status
        {
            UNLOADED,
            MUST_BE_LOADED,
            MUST_BE_REFRESHED,
            REFRESHED
        };

        FBXSceneContext(const char* filename, int windowWidth, int windowHeight, bool supportVBO);
        ~FBXSceneContext();

        HRESULT LoadFile();

        HRESULT OnRender();
        void OnReshape(int width, int height);
        void OnTimerClick();

        [[nodiscard]] FbxScene* GetScene() const
        {
            return m_scene;
        }

        [[nodiscard]] FbxTime GetFrameTime() const
        {
            return m_frameTime;
        }

        [[nodiscard]] Status GetStatus() const
        {
            return m_status;
        }

        [[nodiscard]] FbxArray<FbxString *> anim_stack_name_array() const
        {
            return m_animStackNameArray;
        }

        [[nodiscard]] FbxArray<FbxPose *> pose_array() const
        {
            return m_poseArray;
        }

        [[nodiscard]] FbxArray<FbxNode *> camera_array() const
        {
            return m_cameraArray;
        }

        [[nodiscard]] bool pause() const
        {
            return m_pause;
        }

        HRESULT SetCurrentAnimStack(int index);
        HRESULT SetCurrentCamera(const char* cameraName);
        HRESULT SetCurrentPoseIndex(int poseIndex);
        void SetSelectedNode(FbxNode* node);
        void SetPause(bool pause);

    private:
        void DisplayWindowMessage();
        void DisplayGrid(const FbxAMatrix& transform);

        const char* m_fileName;
        mutable Status m_status;
        mutable FbxString m_statusMessage;

        FbxScene* m_scene;
        FbxAnimLayer* m_currentAnimLayer;
        FbxNode* m_selectedNode;

        int m_poseIndex;
        FbxArray<FbxString*> m_animStackNameArray;
        FbxArray<FbxPose*> m_poseArray;
        FbxArray<FbxNode*> m_cameraArray;

        FbxTime m_frameTime, m_startTime, m_stopTime, m_currentTime;
        FbxTime m_cacheStart, m_cacheStop;

        int m_lastX, m_lastY;
        FbxVector4 m_cameraPosition, m_cameraCenter;
        double m_roll;

        bool m_pause;
        bool m_supportVBO;

        int m_windowWidth;
        int m_windowHeight;
    };

}


#endif //FBXSCENECONTEXT_H
