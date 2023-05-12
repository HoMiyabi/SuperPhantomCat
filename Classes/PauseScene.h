#pragma once

#include "cocos2d.h"

class PauseScene: public cocos2d::Scene
{
public:
	CREATE_FUNC(PauseScene);
	bool init() override;
};