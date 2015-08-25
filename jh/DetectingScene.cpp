//
//  DetectingScene.cpp
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#include "DetectingScene.h"
#include "CMathUtil.h"

#define IS_FirstUseNormol  "IS_FirstUseNormol"
#define IS_FirstUseUpGrade "IS_FirstUseUpGrade"

#define kLockTag 1002

#define kTipFlagClose 1033
#define kTipFlagGetIt 1034

DetectingScene::DetectingScene():
_canDetecting(false)
{
    
}

DetectingScene::~DetectingScene()
{
    
}

Scene* DetectingScene::createScene()
{
    auto scene = Scene::create();
    auto layer = DetectingScene::create();
    scene->addChild(layer);
    return scene;
}

bool DetectingScene::init()
{
    if ( !GameBaseScene::init("detecting_background.jpg"))
    {
        return false;
    }
    
    //清除骨骼动画数据
    char filePath[100];
    sprintf(filePath, "BoneAnimations/%s/NewAnimation.ExportJson",GameController::getInstance()->_dragonType.c_str());
    cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo(filePath);
    
    //植物
    for (int i = 0; i != 3 ; i++)
    {
        char ImageStr[100];
        sprintf(ImageStr, "detecting_cactus%d.png",i+1);
        auto aPo = Sprite::create(ImageStr);
        STVisibleRect::setPosition(aPo, CMathUtil::randomBetween(100, 900), CMathUtil::randomBetween(100, 400));
        this->addChild(aPo);
    }
    //随机物品位置
    for (int i = 0 ; i != 3 ; i ++) {
        _posVector.push_back(Vec2(CMathUtil::randomBetween(100, 900), CMathUtil::randomBetween(200, 500)));
    }
    
    //添加可选择的机器
    auto machineNormal  = SpriteButton::create("UI_Json/studio_ui/explore/explore_icon_tools1.png", CC_CALLBACK_1(DetectingScene::machineItemTouched, this),true,false);
    auto normalBg = Sprite::create("icon_tent_bg.png");
    machineNormal->setPosition(420, 150);
    normalBg->setPosition(420, 150);
    machineNormal->setTag(MachineType::Normal);
    _banner->addChild(normalBg , kGameUIZOrder + 1);
    _banner->addChild(machineNormal , kGameUIZOrder + 1);
    
    
    auto machineUpgrade = SpriteButton::create("UI_Json/studio_ui/explore/explore_icon_tools2.png", CC_CALLBACK_1(DetectingScene::machineItemTouched, this),true,false);
    auto upgradeBg = Sprite::create("icon_tent_bg.png");
    machineUpgrade->setPosition(550, 150);
    upgradeBg->setPosition(550, 150);
    machineUpgrade->setTag(MachineType::UpGrade);
    _banner->addChild(upgradeBg , kGameUIZOrder + 1);
    _banner->addChild(machineUpgrade , kGameUIZOrder + 1);
    //不免费加锁
    if(!IAPManager::getInstance()->getItemIsBought(4))
    {
        auto lock = Sprite::create("UI_Json/studio_ui/public/pubilc_icon_lock.png");
        lock->setPosition(70, 30);
        lock->setTag(kLockTag);
        machineUpgrade->addChild(lock);
        
    }
    
    ParticleSystemQuad *par = ParticleSystemQuad::create("sandParticle.plist");
    par->setBlendAdditive(true);
    this->addChild(par);
    
    return true;
}

void DetectingScene::loadUIContent()
{
    auto main_UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/explore.json");
    this->addChild(main_UI,kGameUIZOrder);
    
    auto home = ui::Helper::seekWidgetByName(main_UI, "btn_home");
    home->addTouchEventListener(CC_CALLBACK_2(DetectingScene::homeBtnClicked, this));
    auto next = ui::Helper::seekWidgetByName(main_UI, "btn_next");
    next->addTouchEventListener(CC_CALLBACK_2(DetectingScene::nextBtnClicked, this));
    next->removeFromParent();
    
    _banner = ui::Helper::seekWidgetByName(main_UI, "exolore_abs");
    _banner->setVisible(false);
}


void DetectingScene::machineItemTouched(Ref *sender)
{
    auto btn = dynamic_cast<SpriteButton*>(sender);
    
    _choosedMachineType = (MachineType)btn->getTag();
    
    SimpleAudioEngine::getInstance()->playEffect("29.closed.mp3");
    if(btn->getTag() == UpGrade)
    {
        //未购买进入商店
        if(btn->getChildByTag(kLockTag) != nullptr)
        {
            showShop();
            return;
        }
    }
    
    _banner->removeFromParent();
    
    _machine = DetectingModel::create(_choosedMachineType);
    STVisibleRect::setPosition(_machine, STVisibleRect::getDesignCeneter().x, STVisibleRect::getDesignCeneter().y);
    _machine->setDragSpriteDelegate(this);
    _machine->setIdentityPos(_machine->getPosition());
    this->addChild(_machine);
    
    auto act = RepeatForever::create(Sequence::create(DelayTime::create(2),ScaleTo::create(.5f, 1.1),ScaleTo::create(.2f, 1),ScaleTo::create(.5f, 1.1),ScaleTo::create(.2f, 1), NULL));
    _machine->runAction(act);
    
    auto idx = CMathUtil::randomBetween(1, 100);
    if(idx < 5)
    {
        _canDetecting = true;
    }
    else
    {
        this->scheduleOnce(schedule_selector(DetectingScene::enableDetecting), 10);
    }
    
}
void DetectingScene::enableDetecting(float)
{
    _canDetecting = true;
}
void DetectingScene::onEnterTransitionDidFinish()
{
    GameBaseScene::onEnterTransitionDidFinish();
    _banner->runAction(Sequence::create(MoveBy::create(0, Vec2(0, -300)),CallFunc::create([&](){
        _banner->setVisible(true);
    }), NULL));
    
    _banner->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(.5f, Vec2(0, 300))),CallFunc::create([&](){
        
        _banner->setVisible(true);
        
        if(GameController::getInstance()->_shouldShowUpGradeMachine && !IAPManager::getInstance()->getItemIsBought(4))
        {
            //展示推荐机器
            SimpleAudioEngine::getInstance()->playEffect("9.upgrade.mp3");
            _tipView = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/explore_get_it.json");
            this->addChild(_tipView,  kGameUIZOrder +10);
            auto btnClose = ui::Helper::seekWidgetByName(_tipView, "btn_close");
            auto btnGet = ui::Helper::seekWidgetByName(_tipView, "btn_get_it");
            
            btnClose->setTag(kTipFlagClose);
            btnGet->setTag(kTipFlagGetIt);
            btnClose->addTouchEventListener(CC_CALLBACK_2(DetectingScene::tipViewBtnClicked, this));
            btnGet->addTouchEventListener(CC_CALLBACK_2(DetectingScene::tipViewBtnClicked, this));
            
            GameController::getInstance()->_shouldShowUpGradeMachine = false;
        }
        else
        {
            SimpleAudioEngine::getInstance()->playEffect("8.findfossil.mp3");
        }
        
        
    }), NULL));
}

void DetectingScene::tipViewBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    auto btn = dynamic_cast<ui::Widget*>(sender);
    if(btn->getTag() == kTipFlagClose)
    {
        SimpleAudioEngine::getInstance()->playEffect("29.closed.mp3");
        _tipView->removeFromParent();
    }
    else
    {
        SimpleAudioEngine::getInstance()->playEffect("28.ok.mp3");
        showShop();
    }
}

void DetectingScene::nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    GameController::getInstance()->goDigFossilScene(ChangeSceneType::REPLACE);
}

void DetectingScene::homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    GameBaseScene::homeBtnClicked(sender, type);
}

#pragma mark -DragSpriteDelegate-
void DetectingScene::DragSpriteTouchBegin(Touch *touch , DragSprite* aDragSprite)
{
}
void DetectingScene::DragSpriteTouchMove(Touch *touch , DragSprite* aDragSprite)
{
    _machine->stopAllActions();
    
    if(!_canDetecting)//不能探测
        return;
    
    Rect rect = _machine->getControlRet();
    for (auto itor = _posVector.begin(); itor != _posVector.end(); itor++) {
        if (rect.containsPoint(STVisibleRect::getRealPositionForDesignPosition((*itor)))) {
            _machine->doFindAction();
            _machine->setEnable(false);
            
            this->scheduleOnce(schedule_selector(DetectingScene::goDigScene), 2);
            
            break;
        }
    }
}
void DetectingScene::DragSpriteTouchEnd(Touch *touch , DragSprite* aDragSprite)
{
    aDragSprite->DragSpriteIdentity(true);
}
void DetectingScene::DragSpriteTouchCancel(Touch *touch , DragSprite* aDragSprite)
{
    
}

void DetectingScene::goDigScene(float)
{
    
    //低级工具成功率50% 高级90%
    auto idx = CMathUtil::randomBetween(0, 9);//[0,9]
    GameController::getInstance()->_isFindFossil = false;
    
    if(_choosedMachineType == Normal)
    {
        if(idx > 6)
            GameController::getInstance()->_isFindFossil = true;
    }
    else
    {
        if(idx > 0)
            GameController::getInstance()->_isFindFossil = true;
    }
    
    bool flagNormal = UserDefault::getInstance()->getBoolForKey(IS_FirstUseNormol, false);
    bool flagUpGrade = UserDefault::getInstance()->getBoolForKey(IS_FirstUseUpGrade, false);
    
    if(!flagNormal)
    {
        //低级工具第一次一定失败
        if(_choosedMachineType == Normal)
        {
            GameController::getInstance()->_isFindFossil = false;
            UserDefault::getInstance()->setBoolForKey(IS_FirstUseNormol, true);
            UserDefault::getInstance()->flush();
        }
    }
    if(!flagUpGrade)
    {
        //高级工具第一次一定成功
        if(_choosedMachineType == UpGrade)
        {
            GameController::getInstance()->_isFindFossil = true;
            UserDefault::getInstance()->setBoolForKey(IS_FirstUseUpGrade, true);
            UserDefault::getInstance()->flush();
        }
    }
    
    if(GameController::getInstance()->_failedCount == 2)
    {
        GameController::getInstance()->_isFindFossil = true;
    }
    GameController::getInstance()->goDigFossilScene(ChangeSceneType::REPLACE);
}
void DetectingScene::shopCloseCallback()
{
    //去锁
    if(IAPManager::getInstance()->getItemIsBought(4))
    {
        auto machine =_banner->getChildByTag(MachineType::UpGrade);
        auto lock = machine->getChildByTag(kLockTag);
        if(nullptr != lock)
            lock->removeFromParent();
    }
}