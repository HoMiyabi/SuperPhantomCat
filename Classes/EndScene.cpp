#include "EndScene.h"
#include "Definitions.h"
#include "WelcomeScene.h"
#include "CoinCountNode.h"
#include "AudioEngine.h"

USING_NS_CC;

bool EndScene::init(int curCount)
{
	if (!Scene::init()) {
		return false;
	}
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

	const auto& visibleSize = Director::getInstance()->getVisibleSize();
	const auto& origin = Director::getInstance()->getVisibleOrigin();

	experimental::AudioEngine::stopAll();

	auto endImage = Sprite::create("end.png");
	endImage->setAnchorPoint({0, 0});
	endImage->setPosition(origin);
	auto& imageSize = endImage->getContentSize();
	Vec2 scale {visibleSize.width / imageSize.width, visibleSize.height / imageSize.height};
	endImage->setScale(scale.x, scale.y);
	addChild(endImage);

	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
		if (keyCode == EventKeyboard::KeyCode::KEY_J) {
			auto welcome = WelcomeScene::create();
			Director::getInstance()->replaceScene(welcome);
		} else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
			Director::getInstance()->end();
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	//File Get And Update
	int totalCount = UserDefault::getInstance()->getIntegerForKey("coin", 0);
	totalCount += curCount;
	UserDefault::getInstance()->setIntegerForKey("coin", totalCount);
	UserDefault::getInstance()->flush();

	//Cur Count
	auto curCountNode = CoinCountNode::create();
	addChild(curCountNode);
	curCountNode->setCount(curCount);
	curCountNode->setPosition(visibleSize * Vec2 {0.468, 0.375});

	//Total Count
	auto totalCountNode = CoinCountNode::create();
	addChild(totalCountNode);
	totalCountNode->setCount(totalCount);
	totalCountNode->setPosition(visibleSize * Vec2 {0.85, 0.91});
	totalCountNode->setScale(1.5);
	return true;
}