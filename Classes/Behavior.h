#pragma once

#include "cocos2d.h"

class Behavior : public cocos2d::Component
{
public:
	inline static std::string name_ = "Behavior";
	bool init() override;
};

inline bool Behavior::init()
{
	setName(name_);
	return true;
}