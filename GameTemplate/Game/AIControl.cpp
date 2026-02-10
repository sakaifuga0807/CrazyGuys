#include "stdafx.h"
#include "AIControl.h"
#include "Player.h"
#include "Goal.h"

namespace
{
    //前方レイの長さ。
    const float RAY_LENGTH = 200.0f;

    //下方向レイの長さ。
    const float DOWN_RAY_LENGTH = 200.0f;

    //レイの開始位置をプレイヤー位置からどれだけ上げるか。
    const float RAY_START_HEIGHT = 50.0f;

    //回避行動を行う時間。
    const float AVOID_TIME = 1.0f;

    //回避時にゴール方向をどれくらい混ぜるか。
    const float AVOID_FORWARD_RATIO = 0.3f;

    //ゴール方向へ戻そうとする補正の割合。
    const float GOAL_CORRECTION_RATIO = 0.1f;

    //ジャンプをランダムで行う確率。
    //数値が小さいほどジャンプしやすくなる。
    const int RANDOM_JUMP_RATE = 120;

    //回避方向。
    const int AVOID_DIR_RIGHT = 1;
    const int AVOID_DIR_LEFT = -1;
}

AIControl::AIControl(Player* player)
    : m_player(player)
{
}

void AIControl::Update()
{
    //プレイヤー、ゴールが取得できていなければ処理しない。
    if (!IsValid())
    {
        return;
    }

    //ゴール方向のベクトルを更新する。
    UpdateGoalDirection();

    //前方の障害物をチェックする。
    CheckObstacle();

    //状態に応じて移動方向を決定する。
    UpdateMoveDirection();

    //ジャンプが必要かどうかを判定する。
    CheckJump();
}

bool AIControl::IsValid()
{
    //プレイヤーが存在しなければ無効。
    if (!m_player)
    {
        return false;
    }

    //ゴールが未取得であれば検索する。
    if (!m_goal)
    {
        m_goal = FindGO<Goal>("goal");
    }

    //ゴールが取得できていれば有効。
    return m_goal != nullptr;
}

void AIControl::UpdateGoalDirection()
{
    //プレイヤーとゴールの座標を取得する。
    Vector3 playerPos = m_player->GetPosition();
    Vector3 goalPos = m_goal->GetPosition();

    //ゴール方向のベクトルを計算する。
    m_toGoal = goalPos - playerPos;

    //Y成分を無視して水平移動のみにする。
    m_toGoal.y = 0.0f;

    //正規化する。
    m_toGoal.Normalize();
}

void AIControl::CheckObstacle()
{
    //レイの開始位置をプレイヤーの少し上に設定する。
    Vector3 rayStart = m_player->GetPosition()
        + Vector3(0.0f, RAY_START_HEIGHT, 0.0f);

    //ゴール方向に向かってレイを飛ばす。
    Vector3 rayEnd = rayStart + m_toGoal * RAY_LENGTH;

    Vector3 hitPos;
    bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStart, rayEnd, hitPos);

    //何かに当たっていて、まだ回避中でなければ回避を開始する。
    if (isHit && !m_isAvoiding)
    {
        StartAvoid();
    }
}

void AIControl::StartAvoid()
{
    //回避状態にする。
    m_isAvoiding = true;

    //回避タイマーを初期化する。
    m_avoidTimer = AVOID_TIME;

    //左右どちらに回避するかをランダムで決定する。
    m_avoidDir = (rand() % 2 == 0) ? AVOID_DIR_RIGHT : AVOID_DIR_LEFT;
}

void AIControl::UpdateMoveDirection()
{
    //回避中かどうかで移動処理を分ける。
    if (m_isAvoiding)
    {
        UpdateAvoidMove();
    }
    else
    {
        //通常時はゴール方向に進む。
        m_moveDir = m_toGoal;
    }

    //ゴール方向への補正を適用する。
    ApplyGoalCorrection();
}

void AIControl::UpdateAvoidMove()
{
    //ゴール方向ベクトルを90度回転させた横方向ベクトルを作る。
    Vector3 right(m_toGoal.z, 0.0f, -m_toGoal.x);

    //横移動に少しゴール方向を混ぜて移動方向を決定する。
    m_moveDir =
        (right * static_cast<float>(m_avoidDir)) +
        (m_toGoal * AVOID_FORWARD_RATIO);

    m_moveDir.Normalize();

    //回避タイマーを減らす。
    m_avoidTimer -= g_gameTime->GetFrameDeltaTime();

    //タイマーが切れたら回避を終了する。
    if (m_avoidTimer <= 0.0f)
    {
        m_isAvoiding = false;
    }
}

void AIControl::ApplyGoalCorrection()
{
    //移動方向にゴール方向への補正を少し加える。
    m_moveDir =
        m_moveDir * (1.0f - GOAL_CORRECTION_RATIO) +
        m_toGoal * GOAL_CORRECTION_RATIO;

    //正規化して方向ベクトルにする。
    m_moveDir.Normalize();
}

void AIControl::CheckJump()
{
    //プレイヤー位置を取得する。
    Vector3 playerPos = m_player->GetPosition();

    //足元から下方向にレイを飛ばす。
    Vector3 rayStart = playerPos
        + Vector3(0.0f, RAY_START_HEIGHT, 0.0f);

    Vector3 rayEnd = rayStart
        + Vector3(0.0f, -DOWN_RAY_LENGTH, 0.0f);

    Vector3 hitPos;
    bool isGround = PhysicsWorld::GetInstance()->RayTest(rayStart, rayEnd, hitPos);

    //地面がなければジャンプを要求する。
    if (!isGround)
    {
        m_jumpRequested = true;
    }
    else
    {
        //地面があっても、一定確率でジャンプする。
        m_jumpRequested = (rand() % RANDOM_JUMP_RATE == 0);
    }
}
