//
//  FillToolScene.cpp
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#include "FillToolScene.h"
#include "PlistReader.h"
FillToolScene::FillToolScene():
_fillCount(0),
_tips(nullptr),
_isDraged(false)
{
    
}

FillToolScene::~FillToolScene()
{
    if(this->isScheduled(schedule_selector(FillToolScene::autoDisMissTipLayer)))
        this->unschedule(schedule_selector(FillToolScene::autoDisMissTipLayer));
}

Scene* FillToolScene::createScene()
{
    auto scene = Scene::create();
    auto layer = FillToolScene::create();
    scene->addChild(layer);
    return scene;
}

bool FillToolScene::init()
{
    if ( !GameBaseScene::init("tent_puzzle.jpg"))
    {
        return false;
    }
    //初始化工具位置数组
    _toolPosVector.push_back(Vec2(345 , 325));
    _toolPosVector.push_back(Vec2(612 , 360));
    _toolPosVector.push_back(Vec2(465 , 365));
    _toolPosVector.push_back(Vec2(210 , 423));
    _toolPosVector.push_back(Vec2(140 , 327));
    _toolPosVector.push_back(Vec2(568 , 419));
    
    loadResources();
    
    return true;
}

void FillToolScene::loadResources()
{
    //加主角
    _role = RoleModel::create(GameController::getInstance()->_RoleName.c_str());
    STVisibleRect::setPosition(_role, 821, 297);
    this->addChild(_role);
//    _role->beginAnimation();
    
    _data = PlisteReader::shareReader()->getImageDataName("ResourcesList.plist", "tools");
    _table = DragTableView::create(this, Size(670, 108), TableView::Direction::HORIZONTAL);
    _table->setTableViewCellType(CellType::kCellTypeCanDrag);
    _table->setDragSpriteZorder(kGameUIZOrder + 10);//设置拖动物品拖动时的ZOrder
    _table->setTableCellDragEndDelegate(this);
    _table->setVisible(false);
    STVisibleRect::setPosition(_table, 155, 100);
    this->addChild(_table , kGameUIZOrder +1);
}

void FillToolScene::loadUIContent()
{
    _main_UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/dig.json");
    this->addChild(_main_UI,kGameUIZOrder);
    
    auto home = ui::Helper::seekWidgetByName(_main_UI, "btn_home");
    home->addTouchEventListener(CC_CALLBACK_2(FillToolScene::homeBtnClicked, this));
    _next = ui::Helper::seekWidgetByName(_main_UI, "btn_next");
    _next->addTouchEventListener(CC_CALLBACK_2(FillToolScene::nextBtnClicked, this));
    _banner = ui::Helper::seekWidgetByName(_main_UI, "dig_abs");
    _banner->setVisible(false);
    _next->setVisible(false);
}

void FillToolScene::onEnterTransitionDidFinish()
{
    GameBaseScene::onEnterTransitionDidFinish();
    SimpleAudioEngine::getInstance()->playEffect("7.tap.mp3");
    _banner->runAction(Sequence::create(MoveBy::create(0, Vec2(0, -300)),CallFunc::create([&](){
        _banner->setVisible(true);
    }), NULL));
    
    _table->getContainer()->setPosition(_table->getContainer()->getPosition() + Vec2(800, 0));
    _banner->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(1, Vec2(0, 300))),CallFunc::create([&](){
        _table->setVisible(true);
        _table->getContainer()->runAction(Sequence::create(MoveBy::create(1, Vec2(-800, 0)),CallFunc::create([&](){
            
            _tips = TipsView::create(TipType::DRAG, 3);
            _tips->setDragTipPath(_table->getPosition(), _table->getPosition() + Vec2(150 , 150));
            this->addChild(_tips,kGameUIZOrder+10);
            _tips->beginTips();
            
        }), NULL));
        
    }), NULL));
}

void FillToolScene::nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    SimpleAudioEngine::getInstance()->playEffect("all_button.mp3");
    GameController::getInstance()->goDetectingScene(ChangeSceneType::REPLACE);
}

void FillToolScene::homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    GameBaseScene::homeBtnClicked(sender, type);
}

void FillToolScene::showTipsWithIndex(int index)
{
    _maskLayer = SwallowTouchLayer::create();
    _maskLayer->_ccLayerTouchCallback = CC_CALLBACK_0(FillToolScene::disMissTipLayer, this);
    this->addChild(_maskLayer , kGameUIZOrder + 50);
    
    char ImageName[50];
    sprintf(ImageName, "tent_tool_%d.png",index);
    auto item = Sprite::create(ImageName);
    STVisibleRect::setPosition(item, 480, 320);
    _maskLayer->addChild(item);
    
    item->setScale(0);
    item->runAction(ScaleTo::create(.3f, 1));
    
    this->scheduleOnce(schedule_selector(FillToolScene::autoDisMissTipLayer), 6);
}

void FillToolScene::autoDisMissTipLayer(float)
{
    if(!_isDraged)
    {
        TipsView *tips = TipsView::create(TipType::DRAG, 3);
        tips->setDragTipPath(_table->getPosition() + Vec2(100 , 30), _table->getPosition() + Vec2(250 , 180));
        this->addChild(tips,kGameUIZOrder+10);
        tips->beginTips();
    }
    _maskLayer->removeFromParent();
    _maskLayer = nullptr;
}
void FillToolScene::disMissTipLayer()
{
    if(this->isScheduled(schedule_selector(FillToolScene::autoDisMissTipLayer)))
        this->unschedule(schedule_selector(FillToolScene::autoDisMissTipLayer));
    _maskLayer->removeFromParent();
    _maskLayer = nullptr;
}

#pragma mark -TableViewDataSource-
Size FillToolScene::cellSizeForTable(TableView *table)
{
    return Size(110, 108);
}
TableViewCell* FillToolScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
    DataModel *aModel = _data.at(idx);
    DragTableViewCell *pCell = DragTableViewCell::create(aModel);
    
    auto itemBg = Sprite::create("icon_tent_bg.png");
    itemBg->setPosition(49 , 54);
    pCell->addChild(itemBg);
    
    auto item = Sprite::create(aModel->getIconName());
    item->setPosition(49 , 54);
    pCell->addChild(item);
    
    return pCell;
}
ssize_t FillToolScene::numberOfCellsInTableView(TableView *table)
{
    return _data.size();
}
//#pragma -TableViewDelegate-
//void FillToolScene::tableCellTouched(TableView* table, TableViewCell* cell)
//{
//    SimpleAudioEngine::getInstance()->playEffect("28.ok.mp3");
//    auto dragCell = dynamic_cast<DragTableViewCell*>(cell);
//    int idx = dragCell->getDataModel()->getID();
//    showTipsWithIndex(idx + 1);
//}
#pragma -TableCellDragDelegate-
void FillToolScene::TableCellDragEndCallback(Sprite* dragSprite,Point point , DataModel *data)
{
    if(nullptr == dragSprite)
        return;
    
    _isDraged = true;
    
    int idx = data->getID();
    auto aPoint = STVisibleRect::getRealPositionForDesignPosition(_toolPosVector.at(idx));
    if (dragSprite->getBoundingBox().containsPoint(aPoint) && !aPoint.isZero())
    {
        SimpleAudioEngine::getInstance()->playEffect("46.right.mp3");
        
        auto callback = [&](Ref* sender){
            auto btn = dynamic_cast<SpriteButton*>(sender);
            SimpleAudioEngine::getInstance()->playEffect("28.ok.mp3");
            this->showTipsWithIndex(btn->getTag() + 1);
            for (Node* nod : btn->getChildren()) {
                if(dynamic_cast<ParticleSystemQuad*>(nod))
                {
                    nod->removeFromParent();
                }
            }
        };
        auto item = SpriteButton::create(data->getImageName(),callback,true,false);
        item->setPosition(aPoint.x, aPoint.y);
        item->setTag(data->getID());
        ParticleSystemQuad *toolParticle = ParticleSystemQuad::create("toymanurls.plist");
        toolParticle->setPosition(item->getContentSize()/2);
        toolParticle->setAutoRemoveOnFinish(true);
        item->addChild(toolParticle);
        this->addChild(item,kGameUIZOrder+20);
        
        _toolPosVector.at(idx) = Vec2::ZERO;
        _fillCount ++;
        
        if(_fillCount == 6)
        {
            _next->setVisible(true);
            _banner->runAction(Sequence::create(MoveBy::create(1, Vec2(-800, 0)),CallFunc::create([&](){
                _banner->removeFromParent();
            }), NULL));
            _table->getContainer()->runAction(Sequence::create(MoveBy::create(1, Vec2(-800, 0)),CallFunc::create([&](){
                _table->removeFromParent();
            }), NULL));
        }
        
        ParticleSystemQuad *par = ParticleSystemQuad::create("fillTools.plist");
        par->setPosition(aPoint);
        par->setAutoRemoveOnFinish(true);
        this->addChild(par);
        
    }
    else
    {
        SimpleAudioEngine::getInstance()->playEffect("47.wrong.mp3");
    }
}
void FillToolScene::TableCellTouchMoveCallback(Sprite* dragSprite,Point point , DataModel *data)
{
    if(nullptr != _tips && _tips->getType() == TipType::DRAG)
    {
        _tips->removeFromParent();
        _tips = nullptr;
    }
}