#include "stdafx.h"
#include "AIControl.h"
#include "Player.h"
#include "Goal.h"

namespace
{
    //レイの長さ。
    const float RAY_LENGTH = 200.0f;
    //回避行動の時間。
    const float AVOID_TIME = 1.0f;
    //ゴール補正の割合。
    const float GOAL_CORRECTION_RATIO = 0.1f;
    //下方向レイの長さ。
    const float DOWN_RAY_LENGTH = 200.0f;
}

AIControl::AIControl(Player* player): m_player(player)
{

}

void AIControl::Update()
{
    if (!m_player)
    {
        return;
    }

    //ゴールを取得。
    if (!m_goal)
    {
        m_goal = FindGO<Goal>("goal");
        if (!m_goal)
        {
            return;
        }
    }

    //値をそれぞれ取得する。
    Vector3 playerPos = m_player->GetPosition();
    Vector3 goalPos = m_goal->GetPosition();

    //ゴール方向へのベクトルを求める。
    Vector3 toGoal = goalPos - playerPos;
    toGoal.y = 0.0f;
    toGoal.Normalize();

    //前方にレイを飛ばす。
    Vector3 rayStart = playerPos + Vector3(0.0f, 50.0f, 0.0f);
    Vector3 rayEnd = rayStart + toGoal * RAY_LENGTH;

    Vector3 hitPos;
    bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStart, rayEnd, hitPos);

    //何かに当たったら回避を開始。
    if (isHit && !m_isAvoiding)
    {
        m_isAvoiding = true;
        m_avoidTimer = AVOID_TIME;
        //右か左どちらかをランダムに選ぶ。
        m_avoidDir = (rand() % 2 == 0) ? 1 : -1;
    }

    //回避中の処理。
    if (m_isAvoiding)
    {
        //ゴール方向を90度回転して横方向に回避。
        Vector3 right(toGoal.z, 0.0f, -toGoal.x);
        m_moveDir = (right * (float)m_avoidDir) + (toGoal * 0.3f);
        m_moveDir.Normalize();

        //タイマーを減らす。
        m_avoidTimer -= g_gameTime->GetFrameDeltaTime();
        if (m_avoidTimer <= 0.0f)
        {
            m_isAvoiding = false;
        }
    }
    else
    {
        //通常時はゴールに向かって進む。
        m_moveDir = toGoal;
    }

    //ゴール方向への補正を少し加える。
    m_moveDir = m_moveDir * (1.0f - GOAL_CORRECTION_RATIO) + toGoal * GOAL_CORRECTION_RATIO;
    m_moveDir.Normalize();

    //少し前の方にレイを飛ばして地面を検知。
    Vector3 downRayStart = playerPos + Vector3(0.0f, 50.0f, 0.0f);
    Vector3 downRayEnd = downRayStart + Vector3(0.0f, -DOWN_RAY_LENGTH, 0.0f);

    Vector3 groundHit;
    bool isGroundAhead = PhysicsWorld::GetInstance()->RayTest(downRayStart, downRayEnd, groundHit);

    //地面がなければジャンプ。
    if (!isGroundAhead)
    {
        m_jumpRequested = true;
    }
    else
    {
        //ランダムでジャンプもする。
        m_jumpRequested = (rand() % 120 == 0);
    }
}
