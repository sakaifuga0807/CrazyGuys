#include "stdafx.h"
#include "MultiViewRender.h"
#include "Player.h"
#include "GameCamera.h"


bool MultiViewRender::Start()
{
    m_playerCount = static_cast<int>(m_players.size());

    // ============================
    // step-1 オフスクリーンRT初期化
    // ============================
    float clearColor[4] = { 0,0,0,0 };

    for (int i = 0; i < m_playerCount; i++)
    {
        m_renderTargets[i].Create(
            512, 512,
            1, 1,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_D32_FLOAT,
            clearColor
        );
    }

    // ============================
    // step-3 描画用カメラ
    // ============================
    m_drawHumanModelCamera.SetUpdateProjMatrixFunc(
        Camera::enUpdateProjMatrixFunc_Ortho);
    m_drawHumanModelCamera.SetWidth(200.0f);
    m_drawHumanModelCamera.SetHeight(200.0f);
    m_drawHumanModelCamera.SetNear(1.0f);
    m_drawHumanModelCamera.SetFar(1000.0f);
    m_drawHumanModelCamera.SetPosition(0, 100, 200);
    m_drawHumanModelCamera.SetTarget(0, 100, 0);
    m_drawHumanModelCamera.SetUp(0, 1, 0);
    m_drawHumanModelCamera.Update();

    // ============================
    // step-4 板ポリ初期化
    // ============================
    m_worldMatrixSB.Init(sizeof(Matrix), 1, nullptr);

    for (int i = 0; i < m_playerCount; i++)
    {
        ModelInitData init;
        init.m_tkmFilePath = "Assets/modelData/plane.tkm";
        init.m_fxFilePath = "Assets/shader/preset/sample3D.fx";
        init.m_expandShaderResoruceView[0] = &m_worldMatrixSB;

        m_planeModels[i].Init(init);
        m_planeModels[i].ChangeAlbedoMap(
            "",
            m_renderTargets[i].GetRenderTargetTexture()
        );

        Vector3 pos;
        pos.x = (i % 2 == 0) ? -480.0f : 480.0f;
        pos.y = (i < 2) ? 270.0f : -270.0f;
        pos.z = 0.0f;

        m_planeModels[i].UpdateWorldMatrix(
            pos, g_quatIdentity, Vector3(480, 270, 1));
    }

    return true;
}

void MultiViewRender::Render(RenderContext& rc)
{
    for (int i = 0; i < m_playerCount; i++)
    {
        // ============================
        // step-5 PlayerをRTに描画
        // ============================
        rc.WaitUntilToPossibleSetRenderTarget(m_renderTargets[i]);
        rc.SetRenderTargetAndViewport(m_renderTargets[i]);
        rc.ClearRenderTargetView(m_renderTargets[i]);

        // ★ここが今まで無かった致命点
        //m_players[i]->GetModelPath().Draw(rc, m_drawHumanModelCamera);

        rc.WaitUntilFinishDrawingToRenderTarget(m_renderTargets[i]);
    }

    // ============================
    // step-6 フレームバッファに戻す
    // ============================
    rc.SetRenderTarget(
        g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
        g_graphicsEngine->GetCurrentFrameBuffuerDSV()
    );

    // ============================
    // step-7 plane描画
    // ============================
    for (int i = 0; i < m_playerCount; i++)
    {
        m_planeModels[i].Draw(rc);
    }
}