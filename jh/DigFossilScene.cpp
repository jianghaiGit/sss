//
//  DigFossilScene.cpp
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#include "DigFossilScene.h"
#include "CMathUtil.h"

enum ButtonType
{
    SAVE = 1001,
    RESET,
    LEFT_STEP,
    RIGHT_STEP,
    RESULT_SUCCEED,
    RESULT_FAILED,
};

DigFossilScene::DigFossilScene():
_isSelectLime(false),
_currentToolIndex(0),
_isResultEnable(true)
{
    
}

DigFossilScene::~DigFossilScene()
{
    if(this->isScheduled(schedule_selector(DigFossilScene::toolsTips)))
        this->unschedule(schedule_selector(DigFossilScene::toolsTips));
}

Scene* DigFossilScene::createScene()
{
    auto scene = Scene::create();
    auto layer = DigFossilScene::create();
    scene->addChild(layer);
    return scene;
}

bool DigFossilScene::init()
{
    if ( !GameBaseScene::init("dig_background.jpg"))
    {
        return false;
    }
    
    std::vector<std::string> dragonVector;
    dragonVector.push_back("apatosaurus");
    dragonVector.push_back("pteranodon");
    dragonVector.push_back("stegosaurus");
    dragonVector.push_back("triceratops");
    dragonVector.push_back("tyrannosaurus");
//    GameController::getInstance()->_isFindFossil = 1;
    if(GameController::getInstance()->_isFindFossil)
    {
        GameController::getInstance()->_failedCount = 0;
        //未购买只能产生两只恐龙 
        //找到的是化石
        if(IAPManager::getInstance()->getItemIsBought(2))
            GameController::getInstance()->_dragonType = dragonVector.at(CMathUtil::randomBetween(0, 4));
        else
            GameController::getInstance()->_dragonType = dragonVector.at(CMathUtil::randomBetween(0, 1));
        _findItemFossil = FossilModel::create(GameController::getInstance()->_dragonType.c_str());
        _findItemFossil->ccToolUseEndCallback = CC_CALLBACK_0(DigFossilScene::aToolUseEnd, this);
        STVisibleRect::setPosition(_findItemFossil, 492, 415);
        this->addChild(_findItemFossil,kGameUIZOrder-1);
    }
    else
    {
        GameController::getInstance()->_failedCount++;
        //非化石
        char ImageName[50];
        sprintf(ImageName,"detecting_%d.png",CMathUtil::randomBetween(1, 6));
        _findItemRubbish = Sprite::create(ImageName);
        STVisibleRect::setPosition(_findItemRubbish, 492, 415);
        this->addChild(_findItemRubbish,kGameUIZOrder+1);
    }
    
    _soil = CrackSoilModel::create();
    _soil->soilCrackCallback = std::bind(&DigFossilScene::crackEndCallFunc,this);
    this->addChild(_soil,kGameUIZOrder+2);
    
    return true;
}
void DigFossilScene::loadUIContent()
{
    auto main_UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/draw.json");
    this->addChild(main_UI,kGameUIZOrder);
    
    auto home = ui::Helper::seekWidgetByName(main_UI, "btn_home");
    home->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::homeBtnClicked, this));
    _next = ui::Helper::seekWidgetByName(main_UI, "btn_next");
    _next->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::nextBtnClicked, this));
    _next->setVisible(false);
    
    _banner    = ui::Helper::seekWidgetByName(main_UI, "draw_abs");
    _ensure    = ui::Helper::seekWidgetByName(main_UI, "btn_save");
    _reset     = ui::Helper::seekWidgetByName(main_UI, "btn_reset");
    _leftStep  = ui::Helper::seekWidgetByName(_banner, "btn_prev");
    _rightStep = ui::Helper::seekWidgetByName(_banner, "btn_next");
    
    _ensure->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::uiBtnClicked, this));
    _reset->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::uiBtnClicked, this));
    _leftStep->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::uiBtnClicked, this));
    _rightStep->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::uiBtnClicked, this));
    
    _ensure->setTag(SAVE);
    _reset->setTag(RESET);
    _leftStep->setTag(LEFT_STEP);
    _rightStep->setTag(RIGHT_STEP);
    
    _banner->setVisible(false);
    _ensure->setVisible(false);
    _reset->setVisible(false);
    _leftStep->setVisible(false);
    _rightStep->setVisible(false);
}
void DigFossilScene::nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    SimpleAudioEngine::getInstance()->playEffect("all_button.mp3");
    
    int width = _findItemFossil->getContentSize().width;
    int height = _findItemFossil->getContentSize().height;
    
    _findItemFossil->setScale(1);
    _findItemFossil->setPosition(676/2, 331/2);
    _findItemFossil->setToolVisible(false);
    RenderTexture *render = RenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888);
    render->begin();
    _findItemFossil->visit();
    render->end();
    Director::getInstance()->getRenderer()->render();
    render->saveToFile("findedFossilImage.png", Image::Format::PNG);

    STVisibleRect::setPosition(_findItemFossil, 492, 415);
    
    GameController::getInstance()->goTransportationScene(ChangeSceneType::REPLACE,render);
}

void DigFossilScene::homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    GameBaseScene::homeBtnClicked(sender, type);
}

void DigFossilScene::crackEndCallFunc()
{
    if(GameController::getInstance()->_isFindFossil)
    {
        SimpleAudioEngine::getInstance()->playEffect("33.sucess.mp3");
        _resultTip = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/explore_success.json");
        this->addChild(_resultTip,  kGameUIZOrder +10);
        auto btn = ui::Helper::seekWidgetByName(_resultTip, "btn_next");
        btn->setTag(RESULT_SUCCEED);
        btn->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::resultBtnClicked, this));
    }
    else
    {
        SimpleAudioEngine::getInstance()->playEffect("34.failed.mp3");
        _resultTip = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/explore_failed.json");
        this->addChild(_resultTip,  kGameUIZOrder +10);
        auto btn = ui::Helper::seekWidgetByName(_resultTip, "btn_reset");
        btn->setTag(RESULT_FAILED);
        btn->addTouchEventListener(CC_CALLBACK_2(DigFossilScene::resultBtnClicked, this));
    }
}

void DigFossilScene::loadTools()
{
    _banner->runAction(Sequence::create(MoveBy::create(0, Vec2(0, -300)),CallFunc::create([&](){
        _banner->setVisible(true);
    }), NULL));
    
    _banner->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(.5f, Vec2(0, 300))),CallFunc::create([&](){
        
        _toolsData = FileUtils::getInstance()->getValueVectorFromFile("DigToolList.plist");
        
        auto toolsBg = ui::Helper::seekWidgetByName(_banner, "main_1");
        
        for (int i = 0; i != _toolsData.size(); i++) {
            string iconName = _toolsData.at(i).asString();
            
            auto itemBg = Sprite::create("icon_tent_bg.png");
            
            itemBg->setPosition(95 + 110*i , 80);
            toolsBg->addChild(itemBg);
            
            auto item = SpriteButton::create(iconName.c_str(),CC_CALLBACK_1(DigFossilScene::toolTouched, this),true,false);
            item->setTag(300 + i);
            item->setPosition(49 , 54);
            itemBg->addChild(item);
            _allToolBtn.pushBack(item);
            
            item->setOpacity(0);
            itemBg->setOpacity(0);
            itemBg->runAction(FadeIn::create(1.5));
            item->runAction(FadeIn::create(1.5));
        }
        
        this->schedule(schedule_selector(DigFossilScene::toolsTips), 2);
        
    }), NULL));
    
}

void DigFossilScene::toolTouched(Ref *sender)
{
    auto tool = dynamic_cast<SpriteButton*>(sender);
    
    if(this->isScheduled(schedule_selector(DigFossilScene::toolsTips)))
    {
        tool->stopAllActions();
        this->unschedule(schedule_selector(DigFossilScene::toolsTips));
    }
    SimpleAudioEngine::getInstance()->playEffect("24.choose.mp3");
    switch (tool->getTag())
    {
        case 300:
            _findItemFossil->showToolWithType(ToolType::HAMMER);
            tool->setEnable(false);
            break;
        case 301:
            if(_currentToolIndex == 1)
            {
                _findItemFossil->showToolWithType(ToolType::CHISEL);
                tool->setEnable(false);
            }
            else
                toolsTips(0);
            break;
        case 302:
            if(_currentToolIndex == 2)
            {     _findItemFossil->showToolWithType(ToolType::BRUSH);
                tool->setEnable(false);
            }
            else
                toolsTips(0);
            break;
        case 303:
            if(_currentToolIndex == 3)
            {   _findItemFossil->showToolWithType(ToolType::PAPER);
                tool->setEnable(false);
            }
            else
                toolsTips(0);
            break;
        case 304:
            if(_currentToolIndex == 4)
            {
                tool->setSpriteFrame(Sprite::create("icon_unselect_bucket.png")->getSpriteFrame());
                tool->setEnable(false);
                _isSelectLime = !_isSelectLime;
                _findItemFossil->showToolWithType(ToolType::Lime);
            }
            else
                toolsTips(0);
            break;
        case 305:
            if(_currentToolIndex == 5)
            {
                _leftStep->setVisible(true);
                _rightStep->setVisible(true);
                auto toolsBg = ui::Helper::seekWidgetByName(_banner, "main_1");
                toolsBg->removeAllChildren();//移除工具
                
                char pFile[50];
                for (int i = 1; i != 21; i++) {
                    sprintf(pFile, "pen_%d.png",i);
                    _pensData.push_back(pFile);
                }
                _table = DragTableView::create(this, Size(670, 108), TableView::Direction::HORIZONTAL);
                _table->setTableViewCellType(CellType::kCellTypeNormol);
                _table->setDelegate(this);
                _table->setPosition(40, 25);
                toolsBg->addChild(_table);
                tool->setEnable(false);
            }
            else
                toolsTips(0);
            break;
        default:
            break;
    }
}

void DigFossilScene::aToolUseEnd()
{
    vector<string> soundVector;
    soundVector.push_back("goodjob.mp3");
    soundVector.push_back("great.mp3");
    soundVector.push_back("fantastic.mp3");
    soundVector.push_back("wonderful.mp3");
    soundVector.push_back("gogrious.mp3");
    SimpleAudioEngine::getInstance()->playEffect(soundVector.at(CMathUtil::randomBetween(0, 4)).c_str());
    
    ParticleSystemQuad *particle = ParticleSystemQuad::create("ToolUsedEnd.plist");
    particle->setPosition(_findItemFossil->getPosition());
    particle->setAutoRemoveOnFinish(true);
    this->addChild(particle, kGameUIZOrder+1);
    
    _currentToolIndex ++;
    this->schedule(schedule_selector(DigFossilScene::toolsTips), 2);
}

void DigFossilScene::toolsTips(float)
{
    auto rotate1 = EaseOut::create(RotateTo::create(.3f, -15), 2);
    auto rotate2 = EaseIn::create(RotateTo::create(.3f, 0), 2);
    auto rotate3 = EaseOut::create(RotateTo::create(.3f, 15), 2);
    auto rotate4 = EaseIn::create(RotateTo::create(.3f, 0), 2);
    
    auto act = Sequence::create(rotate1,rotate2,rotate3,rotate4,DelayTime::create(2), NULL);
    _allToolBtn.at(_currentToolIndex)->runAction(act);
}

void DigFossilScene::resultBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(!_isResultEnable)
        return;
        
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    auto btn = dynamic_cast<ui::Widget*>(sender);
    btn->setEnabled(false);
    _isResultEnable = false;
    if(btn->getTag() == RESULT_SUCCEED)
    {
        _resultTip->removeFromParent();
        SimpleAudioEngine::getInstance()->playEffect("28.ok.mp3");
        SimpleAudioEngine::getInstance()->playEffect("10.dig.mp3");
        //加载工具栏
        loadTools();
    }
    else
    {
        SimpleAudioEngine::getInstance()->playEffect("23.start.mp3");
        GameController::getInstance()->_shouldShowUpGradeMachine = true;
        GameController::getInstance()->goDetectingScene(ChangeSceneType::REPLACE);
    }
}

void DigFossilScene::uiBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    auto btn = dynamic_cast<ui::Widget*>(sender);
    switch (btn->getTag()) {
        case SAVE:
            _banner->setVisible(true);
            _findItemFossil->showToolWithType(ToolType::NONE);
            _findItemFossil->runAction(Sequence::create(ScaleTo::create(.5f, 1),CallFunc::create([&](){
                _next->setVisible(true);
                _ensure->setVisible(false);
                _reset->setVisible(false);
                _table->setEnable(true);
                
            }), NULL));
            break;
        case RESET:
            _findItemFossil->cleanCanvas();
            break;
        case LEFT_STEP:
            if(_table->getContentOffset().x + 110 > _table->maxContainerOffset().x)
                _table->setContentOffsetInDuration(_table->maxContainerOffset(), .2f);
            else
                _table->setContentOffsetInDuration(_table->getContentOffset() + Vec2(110, 0), .2f);
            break;
        case RIGHT_STEP:
            if(_table->getContentOffset().x - 110 < _table->minContainerOffset().x)
                _table->setContentOffsetInDuration(_table->minContainerOffset(), .2f);
            else
                _table->setContentOffsetInDuration(_table->getContentOffset() + Vec2(-110, 0), .2f);
            break;
        default:
            break;
    }
}

#pragma -TableViewDataSource-
Size DigFossilScene::cellSizeForTable(TableView *table)
{
    return Size(110, 108);
}
TableViewCell* DigFossilScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell *pCell = TableViewCell::create();
    
    auto itemBg = Sprite::create("icon_tent_bg.png");
    itemBg->setPosition(55 , 54);
    pCell->addChild(itemBg);
    
    auto item = Sprite::create(_pensData.at(idx).c_str());
    item->setPosition(55 , 54);
    pCell->addChild(item);
    
    return pCell;
}
ssize_t DigFossilScene::numberOfCellsInTableView(TableView *table)
{
    return _pensData.size();
}
#pragma -TableViewDelegate-
void DigFossilScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    SimpleAudioEngine::getInstance()->playEffect("24.choose.mp3");
    GameController::getInstance()->_selectedPenIndex = (int)cell->getIdx() + 1;
    _banner->setVisible(false);
    _next->setVisible(false);
    _findItemFossil->showToolWithType(ToolType::PEN);
    _findItemFossil->runAction(Sequence::create(ScaleTo::create(.5f, 1.3),CallFunc::create([&](){
        _ensure->setVisible(true);
        _reset->setVisible(true);
        _table->setEnable(false);
        
    }), NULL));
}