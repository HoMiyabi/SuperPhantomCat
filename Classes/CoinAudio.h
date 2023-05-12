#pragma once

#include "cocos2d.h"

class CoinAudio: public cocos2d::Node
{
private:
	int combo_ = 1;
	int min_ = 1;
	int max_ = 8;
public:
	bool init() override;
};

inline bool CoinAudio::init()
{
	USING_NS_CC;
	Node::init();
	for (int i = min_; i <= max_; i++) {
		auto name = std::format("/audio/inGame_pick_coin_{}.mp3", i);
		experimental::AudioEngine::preload(name);
	}
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("PICK", [&](EventCustom* event) {
		auto name = std::format("/audio/inGame_pick_coin_{}.mp3", combo_);
		if (combo_ < max_) {
			combo_++;
		}
		experimental::AudioEngine::play2d(name);
		unscheduleAllCallbacks();
		scheduleOnce([&](float a) {
			combo_ = 1;
		}, 0.5f, "stopCombo");
	});
	return true;
}