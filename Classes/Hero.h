#pragma once

#include "cocos2d.h"
#include <array>
#include "Definitions.h"
#include "ActiveCollider.h"

class Hero: public cocos2d::Sprite
{
public:
	enum class State {IDLE, RUN, JUMP, JUMP_HIGH, HURT, FEAR_FRONT, FEAR_BACK};
	enum class Face {RIGHT, LEFT};
	const char* const JUMP_EFFECT_DEFAULT_ = "/audio/inGame_action_jump.mp3";
	const float JUMP_SPEED_DEFAULT_ = 850;
	const float GRAVITY_DEFAULT_ = -2600;
	const float H_SPEED_DEFAULT = 430;
private:
	const char* jumpEffect_ = JUMP_EFFECT_DEFAULT_;
	float jumpSpeed_ = JUMP_SPEED_DEFAULT_;
	float hSpeed_ = H_SPEED_DEFAULT;

	class ActiveCollider* activeCollider_;
	std::array<bool, 4> controlAble_ {true, true, true, true};
	std::array<bool, 4> lastPressed_ {false, false, false, false};
	std::array<bool, 4> curPressed_ {false, false, false, false};

	State state_ = State::IDLE;
	Face face_ = Face::RIGHT;

	struct MoveDirection
	{
		int x;
		int y;
	};

	MoveDirection moveDirection_ { 0, 0 };
	void updateState();
	void jump();
public:
	void initHero();
	void setControlAble(Direction direction, bool state);
	void setJumpEffect(const char* jumpEffect);
	void setJumpSpeed(const float jumpSpeed);
	static Hero *create(const std::string& filename);
	void input(std::map<cocos2d::EventKeyboard::KeyCode, bool> &keyMap);
	void update(float dt, std::map<cocos2d::EventKeyboard::KeyCode, bool>& keyMap);
	void calculateWalk();
	void setAnimation(State state);
	void addPosition(const cocos2d::Vec2& delta);
	void setState(State state);
	void setFace(Face face);
};