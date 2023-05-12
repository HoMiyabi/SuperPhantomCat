#pragma once

#include "cocos2d.h"

class CoinCountComponent : public cocos2d::Component
{
private:
	int count_ = 0;
	const std::string fontName_ = "/Font/Linotte-Regular.ttf";
	float fontSize_ = 48;
	cocos2d::LabelTTF* label_ = nullptr;
public:
	inline static std::string name_ = "CoinCountComponent";
	bool init() override;
	void update();
	void setCount(int count);
	int getCount();
	void addCount(int delta);
	void onAdd() override;
};

inline bool CoinCountComponent::init()
{
	setName(name_);
	return true;
}
inline void CoinCountComponent::update()
{
	USING_NS_CC;
	label_->setString(StringUtils::toString(count_));
}
inline void CoinCountComponent::setCount(int count)
{
	count_ = count;
	update();
}
inline int CoinCountComponent::getCount() { return count_; }
inline void CoinCountComponent::addCount(int delta)
{
	count_ += delta;
	update();
}
inline void CoinCountComponent::onAdd() 
{
	USING_NS_CC;
	label_ = dynamic_cast<LabelTTF*>(getOwner());
	label_->setFontName(fontName_);
	label_->setFontSize(fontSize_);
	update();
	auto listener = EventListenerCustom::create("PICK", [&](EventCustom* event) {
		addCount(1);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, getOwner());
}