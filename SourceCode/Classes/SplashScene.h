#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "cocos2d.h"

class SplashScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene * createScene();
	virtual bool init();
	void menuCallBack(cocos2d::Ref* obj);
	CREATE_FUNC(SplashScene);
};

#endif