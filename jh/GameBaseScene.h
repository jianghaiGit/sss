//
// GameBaseScene.h
// SHAKE4037_BritainRoyalNewBaby
//
// Created by jianghai on 15-4-13.
//
//

#ifndef __SHAKE4037_BritainRoyalNewBaby__GameBaseScene__
#define __SHAKE4037_BritainRoyalNewBaby__GameBaseScene__

#include "cocos2d.h"
#include "STVisibleRect.h"
#include <cocostudio/CocoStudio.h>
#include <ui/CocosGUI.h>
#include "CFSystemFunction.h"
#include "GameController.h"
#include "AlertViewLayer.h"
#include "SimpleAudioEngine.h"
#include "STAds.h"
#include "ShopLayer.h"
#include "TipsView.h"
#include "IAPManager.h"
#include "AdLoadingLayer.h"

#define LOCAL_SOUND_STATUS "soundstatus"
#define IS_MUSIC_ON UserDefault::getInstance()->getBoolForKey(LOCAL_SOUND_STATUS, true)

USING_NS_CC;
USING_NS_ST;
using namespace CocosDenshion;
using namespace std;

#define kGameUIZOrder 100

class GameBaseScene : public Layer
{
public:
    GameBaseScene();
    ~GameBaseScene();
    static Scene* createScene();
    virtual bool init();
    virtual bool init(const char* ImageName);
    CREATE_FUNC(GameBaseScene);
    
    virtual void loadUIContent();//加载cocostudio文件
    
    virtual void nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    
    virtual void previousBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    
    virtual void homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    
    virtual void shopCloseCallback();
    
    void showShop();
    
    virtual void onExit();
    
    void setBackgroundImage(const char* ImageName);
    
private:
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
    virtual void keyBoardAlertViewClickedButtonAtIndex(AlertViewLayer* alert,kAlertViewButton index);
    
protected:
    Sprite* _backgroundImage;
    ui::Widget* _alert;
public:
    CFSystemFunction _manager;
    STAds _adsManager;
};

#endif /* defined(__SHAKE4037_BritainRoyalNewBaby__GameBaseScene__) */