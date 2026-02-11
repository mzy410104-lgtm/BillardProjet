#pragma once
#include "Boule.hpp"
#include <vector>

class Table{
private:
    float width;
    float height;
    //球
    std::vector<Boule> balls;
public:
    Table(float w,float h);
    
    //添加球
    void addBall(const Boule& ball);

    //获取所有球(只读不写)
    const std::vector<Boule>& getBalls() const;
    //更新
    void update(float dt);
    //判断是否回合结束
    bool allBallsStopped() const;
};