//
// GameBaseScene.cpp
// SHAKE4037_BritainRoyalNewBaby
//
// Created by jianghai on 15-4-13.
//
//

#include "GameBaseScene.h"
#define kShopLayerFlag 808

GameBaseScene::GameBaseScene()
{
    
}

GameBaseScene::~GameBaseScene()
{
    //    SimpleAudioEngine::getInstance()->stopAllEffects();
}

Scene* GameBaseScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameBaseScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameBaseScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    init(nullptr);
    return true;
}
bool GameBaseScene::init(const char* ImageName)
{
    if ( !Layer::init() )
    {
        return false;
    }
    if (nullptr != ImageName) {
        _backgroundImage = Sprite::create(ImageName);
        STVisibleRect::setPosition(_backgroundImage, STVisibleRect::getDesignCeneter().x, STVisibleRect::getDesignCeneter().y);
        this->addChild(_backgroundImage);
    }
    
    
    auto listenerkeyPad = EventListenerKeyboard::create();
    listenerkeyPad->onKeyReleased = CC_CALLBACK_2(GameBaseScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkeyPad, this);
    
    loadUIContent();
    return true;
}

void GameBaseScene::loadUIContent()
{
    log("GameBaseScene loadUIContent");
}

void GameBaseScene::setBackgroundImage(const char* ImageName)
{
    _backgroundImage->setSpriteFrame(Sprite::create(ImageName)->getSpriteFrame());
}

void GameBaseScene::nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    dynamic_cast<ui::Widget*>(sender)->setEnabled(false);
    SimpleAudioEngine::getInstance()->stopAllEffects();
    SimpleAudioEngine::getInstance()->playEffect("next_page.mp3");
}
void GameBaseScene::previousBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    SimpleAudioEngine::getInstance()->playEffect("previous_page.mp3");
}
void GameBaseScene::homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    
    SimpleAudioEngine::getInstance()->playEffect("all_button.mp3");
    
    auto alertCallback = [&](AlertViewLayer* alert,kAlertViewButton button){
        if(button == Left)
        {
            //清除骨骼动画数据
            char filePath[100];
            sprintf(filePath, "BoneAnimations/%s/NewAnimation.ExportJson",GameController::getInstance()->_dragonType.c_str());
            cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo(filePath);
            GameController::getInstance()->clearData();
            
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            if(!IAPManager::getInstance()->getItemIsBought(7))
            {
                _adsManager.removeAds();
                AdLoadingLayerBase::showLoading<AdLoadingLayer>(true);
                AdLoadingLayerBase::s_currentInstance->loadingDoneCallback = [](){
                    GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
                };
            }
            else
            {
                GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
            }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            _adsManager.removeAds();
            AdLoadingLayerBase::showLoading<AdLoadingLayer>(true);
            AdLoadingLayerBase::s_currentInstance->loadingDoneCallback = [](){
                GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
            };
#endif
        }
    };
    AlertViewLayer *alert = AlertViewLayer::create(alertCallback,"UI_Json/studio_ui/public/public_btn_yes.png","UI_Json/studio_ui/public/public_btn_no.png","Are you sure you  want to start over?");
    this->addChild(alert,200);
}

void GameBaseScene::showShop()
{
    if(nullptr == this->getChildByTag(kShopLayerFlag))
    {
        ShopLayer *shop = ShopLayer::create();
        shop->ccShopCloseCallback = CC_CALLBACK_0(GameBaseScene::shopCloseCallback, this);
        this->addChild(shop,kGameUIZOrder + 500,kShopLayerFlag);
    }
}
void GameBaseScene::shopCloseCallback()
{
    //子类应重写此方法接受商店关闭的消息
}
void GameBaseScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    bool _isDialogShow;
    for (auto item : this->getChildren())
    {
        if(dynamic_cast<AlertViewLayer*>(item))
            _isDialogShow = true;
        else
            _isDialogShow = false;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_BACK)  //返回
    {
        if(!_isDialogShow)
        {
            _isDialogShow = true;
            AlertViewLayer *alert = AlertViewLayer::create(CC_CALLBACK_2(GameBaseScene::keyBoardAlertViewClickedButtonAtIndex,this),"UI_Json/studio_ui/public/public_btn_yes.png","UI_Json/studio_ui/public/public_btn_no.png","Are you sure you  want to start over?");
            this->addChild(alert,200);
        }
    }
}
void GameBaseScene::keyBoardAlertViewClickedButtonAtIndex(AlertViewLayer* alert,kAlertViewButton index)
{
    if (index == Left) {
        //清除骨骼动画数据
        char filePath[100];
        sprintf(filePath, "BoneAnimations/%s/NewAnimation.ExportJson",GameController::getInstance()->_dragonType.c_str());
        cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo(filePath);
        GameController::getInstance()->clearData();
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        if(!IAPManager::getInstance()->getItemIsBought(7))
        {
            _adsManager.removeAds();
            AdLoadingLayerBase::showLoading<AdLoadingLayer>(true);
            AdLoadingLayerBase::s_currentInstance->loadingDoneCallback = [](){
                 GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
            };
        }
        else
        {
            GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
        }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        _adsManager.removeAds();
        AdLoadingLayerBase::showLoading<AdLoadingLayer>(true);
        AdLoadingLayerBase::s_currentInstance->loadingDoneCallback = [](){
             GameController::getInstance()->goHomeScene(ChangeSceneType::REPLACE);
        };
#endif
    }
}

void GameBaseScene::onExit()
{
    SimpleAudioEngine::getInstance()->stopAllEffects();
    Layer::onExit();
}