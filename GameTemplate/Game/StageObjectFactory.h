#pragma once

class RotationGround;
class Hammer;
class Seesaw;
class Axe;

class StageObjectFactory : public IGameObject
{
public:
    StageObjectFactory() = default;
    ~StageObjectFactory() override;

    bool Start() override;

private:
    void CreateRotationGrounds();
    void CreateHammers();
    void CreateSeesaws();
    void CreateAxes();
private:
    std::vector<RotationGround*> m_rotationGrounds;
    std::vector<Hammer*>         m_hammers;
    std::vector<Seesaw*>         m_seesaws;
    std::vector<Axe*>            m_axes;
};
