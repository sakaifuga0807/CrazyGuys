#include "stdafx.h"
#include "StageObjectFactory.h"

#include "RotationGround.h"
#include "Hammer.h"
#include "Seesaw.h"
#include "Axe.h"
#include "JsonUtility.h"

bool StageObjectFactory::Start()
{
    CreateRotationGrounds();
    CreateHammers();
    CreateSeesaws();
    CreateAxes();
    return true;
}

StageObjectFactory::~StageObjectFactory()
{
    for (auto obj : m_rotationGrounds) DeleteGO(obj);//回転床を破棄する。
    for (auto obj : m_hammers) DeleteGO(obj);//ハンマーを破棄する。
    for (auto obj : m_seesaws) DeleteGO(obj);//シーソーを破棄する。
    for (auto obj : m_axes) DeleteGO(obj);//斧を破棄する。

    m_rotationGrounds.clear();//回転床リストをクリアする。
    m_hammers.clear();//ハンマーリストをクリアする。
    m_seesaws.clear();//シーソーリストをクリアする。
    m_axes.clear();//斧リストをクリアする。
}

//----------------回転床生成処理。----------------
void StageObjectFactory::CreateRotationGrounds()
{
    json config;
    if (!JsonUtility::LoadJson("Assets/config/RotationGroundList.json", config))
        return;//設定ファイルの読み込みに失敗した場合は終了する。

    auto array = config["RotationGrounds"];

    for (int i = 0; i < array.size(); i++)
    {
        auto data = array[i];

        Vector3 pos(
            data["Position"][0],
            data["Position"][1],
            data["Position"][2]);

        Quaternion rot(
            data["Rotation"][0],
            data["Rotation"][1],
            data["Rotation"][2],
            data["Rotation"][3]);

        Vector3 scale(
            data["Scale"][0],
            data["Scale"][1],
            data["Scale"][2]);

        float speed = data["AngleSpeed"];

        auto ground = NewGO<RotationGround>(1, ("RotationGround" + std::to_string(i)).c_str());
        ground->SetPosition(pos);
        ground->SetRotation(rot);
        ground->SetScale(scale);
        ground->SetAngleSpeed(speed);

        m_rotationGrounds.push_back(ground);//生成した回転床を管理リストに追加する。
    }
}

//----------------ハンマー生成処理。----------------
void StageObjectFactory::CreateHammers()
{
    json config;
    if (!JsonUtility::LoadJson("Assets/config/HammerList.json", config))
        return;//設定ファイルの読み込みに失敗した場合は終了する。

    auto array = config["Hammers"];

    for (int i = 0; i < array.size(); i++)
    {
        auto data = array[i];

        Vector3 pos(
            data["Position"][0],
            data["Position"][1],
            data["Position"][2]);

        Quaternion rot(
            data["Rotation"][0],
            data["Rotation"][1],
            data["Rotation"][2],
            data["Rotation"][3]);

        Vector3 scale(
            data["Scale"][0],
            data["Scale"][1],
            data["Scale"][2]);

        auto hammer = NewGO<Hammer>(1, ("Hammer" + std::to_string(i)).c_str());
        hammer->SetPosition(pos);
        hammer->SetRotation(rot);
        hammer->SetScale(scale);

        m_hammers.push_back(hammer);//生成したハンマーを管理リストに追加する。
    }
}

//----------------シーソー生成処理。----------------
void StageObjectFactory::CreateSeesaws()
{
    json config;
    if (!JsonUtility::LoadJson("Assets/config/SeesawList.json", config))
        return;//設定ファイルの読み込みに失敗した場合は終了する。

    auto array = config["Seesaws"];

    for (int i = 0; i < array.size(); i++)
    {
        auto data = array[i];

        Vector3 pos(
            data["Position"][0],
            data["Position"][1],
            data["Position"][2]);

        Quaternion rot(
            data["Rotation"][0],
            data["Rotation"][1],
            data["Rotation"][2],
            data["Rotation"][3]);

        Vector3 scale(
            data["Scale"][0],
            data["Scale"][1],
            data["Scale"][2]);

        auto seesaw = NewGO<Seesaw>(1, ("Seesaw" + std::to_string(i)).c_str());
        seesaw->SetPosition(pos);
        seesaw->SetRotation(rot);
        seesaw->SetScale(scale);

        seesaw->SetMovingState(
            (rand() % 2 == 0)
            ? Seesaw::enMovingSeesaw::enUp
            : Seesaw::enMovingSeesaw::enDown);

        m_seesaws.push_back(seesaw);//生成したシーソーを管理リストに追加する。
    }
}

//----------------斧生成処理。----------------
void StageObjectFactory::CreateAxes()
{
    json config;
    if (!JsonUtility::LoadJson("Assets/config/AxeList.json", config))
        return;//設定ファイルの読み込みに失敗した場合は終了する。

    auto array = config["Axes"];

    for (int i = 0; i < array.size(); i++)
    {
        auto data = array[i];

        Vector3 pos(
            data["Position"][0],
            data["Position"][1],
            data["Position"][2]);

        Quaternion rot(
            data["Rotation"][0],
            data["Rotation"][1],
            data["Rotation"][2],
            data["Rotation"][3]);

        Vector3 scale(
            data["Scale"][0],
            data["Scale"][1],
            data["Scale"][2]);

        float speed = data["Speed"];
        float range = data["Range"];

        auto axe = NewGO<Axe>(2, ("Axe" + std::to_string(i)).c_str());
        axe->SetPosition(pos);
        axe->SetRotation(rot);
        axe->SetScale(scale);
        axe->SetSpeed(speed);
        axe->SetRange(range);

        m_axes.push_back(axe);//生成した斧を管理リストに追加する。
    }
}
