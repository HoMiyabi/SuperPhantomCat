#pragma once

#include "cocos2d.h"

class EndScene : public cocos2d::Scene
{
public:
    static EndScene* create(int curCount)
    {
        EndScene* pRet = new(std::nothrow) EndScene(); if (pRet && pRet->init(curCount)) {
            pRet->autorelease(); return pRet;
        } else {
            delete pRet; pRet = nullptr; return nullptr;
        }
    };
    bool init(int curCount);
};