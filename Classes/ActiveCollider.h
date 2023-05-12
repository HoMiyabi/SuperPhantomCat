#pragma once

#include "cocos2d.h"
#include <array>
#include "Collider.h"
#include "Definitions.h"

class ActiveCollider: public cocos2d::Component
{
public:
	inline static std::string name_ = "ActiveCollider";
	void setVelocity(cocos2d::Vec2 v);
	void setVelocityX(float x);
	void setVelocityY(float y);
	void addVelocity(cocos2d::Vec2 delta);
	const cocos2d::Vec2& getVelocity();
	float getVelocityX();
	float getVelocityY();

	void setGravity(float gravity);

	const auto& getComplexColliders();
	const auto& getSimpleColliders();

	void step(float dt);

	bool initWithSize(cocos2d::Size size);
	static ActiveCollider* create(cocos2d::Size size);
	cocos2d::Vec2 lastContactDownPos_ {0, 1000000};
private:
	float g_ = 0;
	std::array<class Collider*, 12> complexColliders_ {};
	std::array<Collider*, 4> simpleColliders_ {};
	enum Place { LEFT = 1, RIGHT = 1 << 1, UP = 1 << 2, DOWN = 1 << 3, CENTER_X = 1 << 4, CENTER_Y = 1 << 5 };
	struct RayCastData
	{
		Collider* collider;
		cocos2d::PhysicsRayCastInfo info;
		int count = 0;
	};
	cocos2d::Size size_;

	template<typename T>
	const T& consider(const T& first, const T& second, const T& third);

	std::map<int, cocos2d::Vec2> getTraces();
	std::map<Direction, cocos2d::Vec2> getRayPoints(enum Direction direction, const cocos2d::Vec2& center);
	cocos2d::Vec2 v_ {0, 0};
	void move(const cocos2d::Vec2& delta);
	void checkContact();
	void contactCall();
	void calculateGravity(float dt);
};

inline void ActiveCollider::setVelocity(cocos2d::Vec2 v) { v_ = v; }
inline void ActiveCollider::setVelocityX(float x) { v_.x = x; }
inline void ActiveCollider::setVelocityY(float y) { v_.y = y; }
inline void ActiveCollider::addVelocity(cocos2d::Vec2 delta) { v_ += delta; }
inline const cocos2d::Vec2& ActiveCollider::getVelocity() { return v_; }
inline float ActiveCollider::getVelocityX() { return v_.x; }
inline float ActiveCollider::getVelocityY() { return v_.y; }
inline const auto& ActiveCollider::getComplexColliders() { return complexColliders_; }
inline const auto& ActiveCollider::getSimpleColliders() { return simpleColliders_; }
inline void ActiveCollider::setGravity(float gravity) { g_ = gravity; }
inline void ActiveCollider::step(float dt)
{
	USING_NS_CC;
	calculateGravity(dt);
	float xb = _owner->getPositionX();
	log("target y %f", _owner->getPositionY() + v_.y * dt);
	move(v_ * dt);
	log("real y %f", _owner->getPositionY());
	checkContact();
	contactCall();
	float xa = _owner->getPositionX();
	log("v %f %f x %f", v_.x, v_.y, (xa - xb) / dt);
}
inline bool ActiveCollider::initWithSize(cocos2d::Size size)
{
	setName(name_);
	size_ = size;
	return true;
}
inline ActiveCollider* ActiveCollider::create(cocos2d::Size size)
{
	ActiveCollider* pRet = new(std::nothrow) ActiveCollider(); if (pRet && pRet->initWithSize(size)) {
		pRet->autorelease(); return pRet;
	} else {
		delete pRet; pRet = nullptr; return nullptr;
	}
}
inline void ActiveCollider::move(const cocos2d::Vec2& delta)
{
	USING_NS_CC;
	auto physicsWorld = _owner->getScene()->getPhysicsWorld();
	const auto& curPos = _owner->getPosition();
	auto traces = getTraces();

	auto rayCastCallback = [](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
		auto collider = GetComponent<Collider>(info.shape->getBody()->getNode());
		if (collider && collider->isEnabled() && info.fraction != 0.0f) {
			if (static_cast<RayCastData*>(data)->count == 1) {
				return true;
			}
			static_cast<RayCastData*>(data)->collider = collider;
			static_cast<RayCastData*>(data)->info = info;
			static_cast<RayCastData*>(data)->count = 1;
		}
		return true;
	};

	std::vector<RayCastData> collisionVertexData;
	for (const auto& [place, trace] : traces) {
		RayCastData data {};
		physicsWorld->rayCast(rayCastCallback, trace, trace + delta, &data);
		auto collider = data.collider;
		if (collider) {
			if (collider->getType() == Collider::TYPE::ONE_WAY && !(delta.y <= 0 && (place & DOWN))) {
				continue;
			}
			if (collider->getType() == Collider::TYPE::TRIGGER) {
				continue;
			}
			collisionVertexData.emplace_back(data);
		}
	}
	auto it = std::min_element(collisionVertexData.begin(), collisionVertexData.end(), [](const RayCastData& data1, const RayCastData& data2) -> bool {
		return (data1.info.contact - data1.info.start).length() < (data2.info.contact - data2.info.start).length();
	});
	if (it != collisionVertexData.end()) {

		auto curDelta = it->info.contact - it->info.start;
		_owner->setPosition(curPos + curDelta + it->info.normal);
		log("correct");
		Vec2 restDelta;
		if (it->info.normal == Vec2::UNIT_X || it->info.normal == -Vec2::UNIT_X) {
			log("make v x 0");
			v_.x = 0;
			restDelta = {0, (delta - curDelta).y};
		} else if (it->info.normal == Vec2::UNIT_Y || it->info.normal == -Vec2::UNIT_Y) {
			log("make v y 0");
			v_.y = 0;
			restDelta = {(delta - curDelta).x, 0};
		}
		if (restDelta != Vec2 {0, 0}) {
			move(restDelta);
		}
	} else {
		log("no correct");
		_owner->setPosition(curPos + delta);
	}
	log("after correct %f %f", _owner->getPosition().x, _owner->getPosition().y);
}
inline void ActiveCollider::checkContact()
{
	USING_NS_CC;
	complexColliders_.fill(nullptr);
	simpleColliders_.fill(nullptr);
	auto physicsWorld = _owner->getScene()->getPhysicsWorld();
	auto rayCastCallback = [=](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
		auto collider = GetComponent<Collider>(info.shape->getBody()->getNode());
		if (collider && collider->isEnabled() && !(collider->getType() == Collider::TYPE::ONE_WAY && getVelocityY() > 0)) {
			static_cast<RayCastData*>(data)->collider = collider;
			static_cast<RayCastData*>(data)->info = info;
		}
		return true;
	};
	auto pos = _owner->getPosition() + size_ / 2;
	//Up
	auto points = getRayPoints(Direction::UP, pos);
	for (auto& [direction, point] : points) {
		RayCastData data {nullptr};
		physicsWorld->rayCast(rayCastCallback, point, point + 2 * Vec2::UNIT_Y, &data);
		if (data.collider) {
			complexColliders_[direction] = data.collider;
		}
	}
	simpleColliders_[Direction::UP] = consider(complexColliders_[Direction::UP], complexColliders_[UP_LEFT], complexColliders_[UP_RIGHT]);

	//Down
	points = getRayPoints(Direction::DOWN, pos);
	for (auto& [direction, point] : points) {
		RayCastData data {nullptr};
		physicsWorld->rayCast(rayCastCallback, point, point - 2 * Vec2::UNIT_Y, &data);
		if (data.collider) {
			complexColliders_[direction] = data.collider;
		}
	}
	simpleColliders_[Direction::DOWN] = consider(complexColliders_[Direction::DOWN], complexColliders_[DOWN_LEFT], complexColliders_[DOWN_RIGHT]);

	//Right
	points = getRayPoints(Direction::RIGHT, pos);
	for (auto& [direction, point] : points) {
		RayCastData data {nullptr};
		physicsWorld->rayCast(rayCastCallback, point, point + 2 * Vec2::UNIT_X, &data);
		if (data.collider) {
			complexColliders_[direction] = data.collider;
		}
	}
	simpleColliders_[Direction::RIGHT] = consider(complexColliders_[Direction::RIGHT], complexColliders_[RIGHT_DOWN], complexColliders_[RIGHT_UP]);

	//Left
	points = getRayPoints(Direction::LEFT, pos);
	for (auto& [direction, point] : points) {
		RayCastData data {nullptr};
		physicsWorld->rayCast(rayCastCallback, point, point - 2 * Vec2::UNIT_X, &data);
		if (data.collider) {
			complexColliders_[direction] = data.collider;
		}
	}
	simpleColliders_[Direction::LEFT] = consider(complexColliders_[Direction::LEFT], complexColliders_[LEFT_DOWN], complexColliders_[LEFT_UP]);

}
inline void ActiveCollider::contactCall()
{
	USING_NS_CC;
	//log("contactCall");
	std::map<Collider*, int> finished;
	for (int i = 0; i < 4; i++) {
		const auto direction = static_cast<Direction>(i);
		auto collider = simpleColliders_[i];
		if (collider) {
			if (finished[collider] == 0) {
				collider->contact(this, direction);
				finished[collider] = 1;
				if (direction == Direction::DOWN && collider->getType() != Collider::TYPE::TRIGGER) {
					lastContactDownPos_ = _owner->getPosition();
				}
			}
		}
	}
}
inline void ActiveCollider::calculateGravity(float dt)
{
	USING_NS_CC;
	if (!simpleColliders_.at(Direction::DOWN)) {
		v_.y += g_ * dt;
		log("gravity");
	}
	//log("gravity contactDown %d", simpleColliders_.at(Direction::DOWN));
}

template<typename T>
inline const T& ActiveCollider::consider(const T& first, const T& second, const T& third)
{
	if (first) {
		return first;
	}
	if (second) {
		return second;
	}
	return third;
}

inline std::map<int, cocos2d::Vec2> ActiveCollider::getTraces()
{
	USING_NS_CC;
	Vec2 base = _owner->getPosition();
	Vec2 right {size_.width, 0};
	Vec2 up {0, size_.height};
	Vec2 centerX {size_.width / 2.0f, 0};
	Vec2 centerY {0, size_.height / 2.0f};
	std::map<int, cocos2d::Vec2> traces {
		{LEFT | DOWN, base},
		{LEFT | CENTER_Y, base + centerY},
		{LEFT | UP, base + up},
		{RIGHT | DOWN, base + right},
		{RIGHT | CENTER_Y, base + right + centerY},
		{RIGHT | UP, base + right + up},
		{UP | CENTER_X, base + up + centerX},
		{DOWN | CENTER_X, base + centerX}
	};
	return traces;
}
inline std::map<Direction, cocos2d::Vec2> ActiveCollider::getRayPoints(enum Direction direction, const cocos2d::Vec2& center)
{
	USING_NS_CC;
	constexpr float buffer = 0.12f;

	Vec2 rCenter;
	Vec2 step;

	switch (direction) {
		case Direction::RIGHT: {
			rCenter = center + Vec2 {size_.width / 2.0f, 0};
			step = {0.0f, size_.height * (0.5f - buffer)};
			return {{Direction::RIGHT_DOWN, rCenter - step}, {Direction::RIGHT, rCenter}, {Direction::RIGHT_UP, rCenter + step}};
		}
		case Direction::LEFT: {
			rCenter = center - Vec2 {size_.width / 2.0f, 0};
			step = {0.0f, size_.height * (0.5f - buffer)};
			return {{Direction::LEFT_DOWN, rCenter - step}, {Direction::LEFT, rCenter}, {Direction::LEFT_UP, rCenter + step}};
		}
		case Direction::UP: {
			rCenter = center + Vec2 {0, size_.height / 2.0f};
			step = {size_.width * (0.5f - buffer), 0.0f};
			return {{Direction::UP_LEFT, rCenter - step}, {Direction::UP, rCenter}, {Direction::UP_RIGHT, rCenter + step}};
		}
		case Direction::DOWN: {
			rCenter = center - Vec2 {0, size_.height / 2.0f};
			step = {size_.width * (0.5f - buffer), 0.0f};
			return {{Direction::DOWN_LEFT, rCenter - step}, {Direction::DOWN, rCenter}, {Direction::DOWN_RIGHT, rCenter + step}};
		}
	}
}
