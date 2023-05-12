#include "Hero.h"
#include "Ticker.h"
#include "AudioEngine.h"

USING_NS_CC;

Hero* Hero::create(const std::string& filename)
{
	Hero* sprite = new (std::nothrow) Hero();
	if (sprite && sprite->initWithFile(filename)) {
		sprite->autorelease();
		sprite->initHero();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Hero::update(float dt, std::map<cocos2d::EventKeyboard::KeyCode, bool>& keyMap)
{
	input(keyMap);
	updateState();
	calculateWalk();
	activeCollider_->step(dt);
}

void Hero::input(std::map<cocos2d::EventKeyboard::KeyCode, bool>& keyMap)
{
	lastPressed_ = curPressed_;
	curPressed_[LEFT] = controlAble_[LEFT] && keyMap[EventKeyboard::KeyCode::KEY_A];
	curPressed_[RIGHT] = controlAble_[RIGHT] && keyMap[EventKeyboard::KeyCode::KEY_D];
	curPressed_[UP] = controlAble_[UP] && keyMap[EventKeyboard::KeyCode::KEY_J];
}

void Hero::updateState()
{
	auto& simpleColliders = activeCollider_->getSimpleColliders();
	auto& complexColliders = activeCollider_->getComplexColliders();

	//FSM
	switch (state_) {
		case State::FEAR_FRONT:
		case State::FEAR_BACK:
		case State::IDLE: {
			if (complexColliders[DOWN_LEFT] && !complexColliders[DOWN] && !complexColliders[DOWN]) {
				if (face_ == Face::RIGHT) {
					setState(State::FEAR_FRONT);
				} else {
					setState(State::FEAR_BACK);
				}
			} else if (!complexColliders[DOWN_LEFT] && !complexColliders[DOWN] && complexColliders[DOWN_RIGHT]) {
				if (face_ == Face::LEFT) {
					setState(State::FEAR_FRONT);
				} else {
					setState(State::FEAR_BACK);
				}
			}
			if (curPressed_[LEFT] && !simpleColliders.at(Direction::LEFT)) {
				setState(State::RUN);
			} else if (curPressed_[RIGHT] && !simpleColliders.at(Direction::RIGHT)) {
				setState(State::RUN);
			} else if (curPressed_[UP] && simpleColliders.at(Direction::DOWN)) {
				setState(State::JUMP);
			}
			break;
		}
		case State::RUN: {
			if (!curPressed_[LEFT] || (curPressed_[LEFT] && simpleColliders.at(Direction::LEFT) && simpleColliders.at(Direction::LEFT)->getType() == Collider::TYPE::NORMAL)) {
				if (!curPressed_[RIGHT] || (curPressed_[RIGHT] && simpleColliders.at(Direction::RIGHT) && simpleColliders.at(Direction::RIGHT)->getType() == Collider::TYPE::NORMAL)) {
					setState(State::IDLE);
				}
			}
			if (curPressed_[UP] && simpleColliders.at(Direction::DOWN)) {
				setState(State::JUMP);
			}
			break;
		}
		case State::JUMP_HIGH:
		case State::JUMP: {
			if (simpleColliders.at(Direction::DOWN) && activeCollider_->getVelocityY() <= 0) {
				setState(State::IDLE);
			}
			break;
		}
		case State::HURT: {
			if (simpleColliders.at(Direction::DOWN) && activeCollider_->getVelocityY() <= 0) {
				setState(State::IDLE);
				log("recover");
				controlAble_[LEFT] = true;
				controlAble_[RIGHT] = true;
				controlAble_[UP] = true;
			}
			break;
		}
	}

	//Face Direction
	if (controlAble_[LEFT] && controlAble_[RIGHT]) {
		if (curPressed_[LEFT] && curPressed_[RIGHT]) {
			if (lastPressed_[LEFT] && !lastPressed_[RIGHT]) {
				moveDirection_.x = 1;
				setFace(Face::RIGHT);
				Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CAMERA_TO_RIGHT");
			} else if (!lastPressed_[LEFT] && lastPressed_[RIGHT]) {
				moveDirection_.x = -1;
				setFace(Face::LEFT);
				Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CAMERA_TO_LEFT");
			} else if (!lastPressed_[LEFT] && !lastPressed_[RIGHT]){
				moveDirection_.x = 1;
				setFace(Face::RIGHT);
				Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CAMERA_TO_RIGHT");
			}
		} else if (curPressed_[LEFT]) {
			moveDirection_.x = -1;
			setFace(Face::LEFT);
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CAMERA_TO_LEFT");
		} else if (curPressed_[RIGHT]) {
			moveDirection_.x = 1;
			setFace(Face::RIGHT);
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CAMERA_TO_RIGHT");
		} else {
			moveDirection_.x = 0;
		}
	}
}

void Hero::initHero()
{
	setAnchorPoint({0, 0});
	auto activeCollider = ActiveCollider::create({56, 60});
	addComponent(activeCollider);
	activeCollider->setGravity(GRAVITY_DEFAULT_);
	activeCollider_ = activeCollider;
}

void Hero::setControlAble(Direction direction, bool state)
{
	controlAble_[direction] = state;
}

void Hero::setJumpEffect(const char* jumpEffect)
{
	jumpEffect_ = jumpEffect;
}
void Hero::setJumpSpeed(const float jumpSpeed)
{
	jumpSpeed_ = jumpSpeed;
}

void Hero::addPosition(const cocos2d::Vec2& delta)
{
	setPosition(getPosition() + delta);
}

void Hero::setAnimation(State state)
{
	switch (state) {
		case State::IDLE: {
			stopAllActions();
			Animation* animation = Animation::create();
			for (int i = 0; i <= 60; i++) {
				auto name = std::format("/nono/idle/img_hero_nono_idle_0_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.05f);
			Animate* animate = Animate::create(animation);
			runAction(RepeatForever::create(animate));
			break;
		}
		case State::RUN: {
			stopAllActions();
			Animation* animation = Animation::create();
			for (int i = 0; i <= 16; i++) {
				auto name = std::format("/nono/run/img_hero_nono_run_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.03f);
			Animate* animate = Animate::create(animation);
			runAction(RepeatForever::create(animate));
			break;
		}
		case State::JUMP: {
			stopAllActions();
			Animation* animation = Animation::create();
			for (int i = 0; i <= 6; i++) {
				auto name = std::format("/nono/jump/img_hero_nono_jump_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.05f);
			Animate* animate = Animate::create(animation);
			runAction(animate);
			break;
		}
		case State::JUMP_HIGH: {
			stopAllActions();
			Animation* animation = Animation::create();
			for (int i = 4; i <= 9; i++) {
				auto name = std::format("/nono/jump/img_hero_nono_jump_high_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.05f);
			Animate* animate = Animate::create(animation);
			runAction(animate);
			break;
		}
		case State::HURT: {
			stopAllActions();
			Animation* animation = Animation::create();
			for (int i = 0; i <= 2; i++) {
				auto name = std::format("/nono/hurt/img_hero_nono_hurt_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.1f);
			Animate* animate = Animate::create(animation);
			runAction(animate);
			break;
		}
		case State::FEAR_FRONT: {
			stopAllActions();
			Animation* animation = Animation::create();
			int count = 36;
			for (int i = 0; i < count; i++) {
				auto name = std::format("/nono/fear/img_hero_nono_fear_1_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.033f);
			Animate* animate = Animate::create(animation);
			runAction(RepeatForever::create(animate));
			break;
		}
		case State::FEAR_BACK: {
			stopAllActions();
			Animation* animation = Animation::create();
			int count = 36;
			for (int i = 0; i < count; i++) {
				auto name = std::format("/nono/fear/img_hero_nono_fear_2_{}.png", i);
				animation->addSpriteFrameWithFile(name);
			}
			animation->setDelayPerUnit(0.033f);
			Animate* animate = Animate::create(animation);
			runAction(RepeatForever::create(animate));
			break;
		}
	}
}

void Hero::calculateWalk()
{
	auto& simpleColliders = activeCollider_->getSimpleColliders();
	if (moveDirection_.x == 1 && controlAble_[LEFT]) {
		activeCollider_->setVelocityX(moveDirection_.x * hSpeed_);
	} else if (moveDirection_.x == -1 && controlAble_[RIGHT]) {
		activeCollider_->setVelocityX(moveDirection_.x * hSpeed_);
	} else if (controlAble_[LEFT] && controlAble_[RIGHT]) {
		activeCollider_->setVelocityX(0);
	}
}

void Hero::setState(State state)
{
	if (state_ != state) {
		log("rightNow %d setState run", state_, state == State::RUN);
		state_ = state;
		setAnimation(state);
		if (state_ == State::JUMP) {
			log("jump");
			jump();
		}
	}

}

void Hero::setFace(Face face)
{
	if (face_ != face) {
		face_ = face;
		if (face == Face::RIGHT) {
			setFlippedX(false);
		} else {
			setFlippedX(true);
		}
	}
}

void Hero::jump()
{
	experimental::AudioEngine::play2d(jumpEffect_);

	activeCollider_->setVelocityY(jumpSpeed_);
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("JUMP", this);
	activeCollider_->setGravity(0.7 * GRAVITY_DEFAULT_);

	scheduleOnce([this](float d) {
		if (curPressed_[UP]) {
			setState(State::JUMP_HIGH);
			activeCollider_->setGravity(0.4 * GRAVITY_DEFAULT_);
			scheduleOnce([this](float d) {
				activeCollider_->setGravity(GRAVITY_DEFAULT_);
			}, 0.15, "HIGH_JUMP_WAIT");
		} else {
			activeCollider_->setGravity(GRAVITY_DEFAULT_);
		}
	}, 0.15, "CHECK_EARLY_RELEASE");
}