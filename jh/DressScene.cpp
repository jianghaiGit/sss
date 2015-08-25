//
// DressScene.cpp
// SHAKE4038_KidsJurassicAdventure
//
// Created by jianghai on 15/5/30.
//
//

#include "DressScene.h"
#include "ShareItemView.h"
#include "CMathUtil.h"
#include "DecorationReader.h"
enum ButtonType
{
    HOME = 1001,
    SHOP,
    NEXT,
    RESET
};
enum DressType
{
    CLOTH = 101,
    GLASS,
    HAT,
    PANT,
    TOY
};

DressScene::DressScene():
_selectType(0),
_dragDecorateLayer(nullptr),
_isTableMoving(false),
_isDecorateMoving(false)
{
    
}

DressScene::~DressScene()
{
    
}

Scene* DressScene::createScene()
{
    auto scene = Scene::create();
    auto layer = DressScene::create();
    scene->addChild(layer);
    return scene;
}

bool DressScene::init()
{
    if ( !GameBaseScene::init(GameController::getInstance()->_dressBackgroundImageName.c_str()))
    {
        return false;
    }
    _boneNames.push_back("clothes");
    _boneNames.push_back("glasses");
    _boneNames.push_back("headwear");
    _boneNames.push_back("pants");
    
    GameController::getInstance()->_clothIndex = -1;
    GameController::getInstance()->_hatIndex = -1;
    GameController::getInstance()->_glassIndex = -1;
    GameController::getInstance()->_pantIndex = -1;
    initData();
    return true;
}

#pragma mark - initData
void DressScene::initData()
{
    auto dragonContainer = Layer::create();
    //添加恐龙
    char filePath[100];
    sprintf(filePath, "ACG/Dragon/%s/%s.ExportJson",GameController::getInstance()->_dragonType.c_str(),GameController::getInstance()->_dragonType.c_str());
    cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo(filePath);
    _aDragon = cocostudio::Armature::create(GameController::getInstance()->_dragonType.c_str());
    STVisibleRect::setPosition(_aDragon, 480, 250);
    dragonContainer->addChild(_aDragon);
    this->addChild(dragonContainer);
    _dragonAnimations = _aDragon->getAnimation();
    _dragonAnimations->setMovementEventCallFunc(CC_CALLBACK_3(DressScene::dragonActionCallback, this));
    _dragonAnimations->play("2", 0, 1);
    
    //装饰物容器
    _dragDecorateLayer = Layer::create();
    this->addChild(_dragDecorateLayer,5);
    
    
    //隐藏衣服等装饰
    {
        string boneName;
        for (auto str : _boneNames)
        {
            Map<string, cocostudio::Bone*> bones = _aDragon->getBoneDic();
            for (auto it = bones.begin(); it != bones.end(); it++)
            {
                string boneName = it->first;
                if (boneName.find(str) != string::npos)
                {
                    (*it).second->getDisplayRenderNode()->setVisible(false);
                }
            }
        }
    }
    DecorationReader::getInstance()->setSourcePath("DressResources.plist");
    
    _table = DragTableView::create(this, Size(80 , 430), TableView::Direction::VERTICAL);
    _table->setTableViewCellType(CellType::kCellTypeNormol);
    _table->setDragSpriteZorder(kGameUIZOrder + 10);//设置拖动物品拖动时的ZOrder
    _table->setDelegate(this);
    _table->setPosition(10, 10);
    _tableBg->addChild(_table);
}
void DressScene::loadUIContent()
{
    _main_UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("UI_Json/dress.json");
    this->addChild(_main_UI,kGameUIZOrder);
    
    auto home = ui::Helper::seekWidgetByName(_main_UI, "btn_home");
    home->setTag(HOME);
    home->addTouchEventListener(CC_CALLBACK_2(DressScene::UIBtnClicked, this));
    auto shop = ui::Helper::seekWidgetByName(_main_UI, "btn_shop");
    shop->setTag(SHOP);
    shop->addTouchEventListener(CC_CALLBACK_2(DressScene::UIBtnClicked, this));
    _next = ui::Helper::seekWidgetByName(_main_UI, "btn_next");
    _next->setTag(NEXT);
    _next->addTouchEventListener(CC_CALLBACK_2(DressScene::UIBtnClicked, this));
    _reset = ui::Helper::seekWidgetByName(_main_UI, "btn_reset");
    _reset->setTag(RESET);
    _reset->addTouchEventListener(CC_CALLBACK_2(DressScene::UIBtnClicked, this));
    
    _tableBg = ui::Helper::seekWidgetByName(_main_UI, "sub");
    _tableBg->setVisible(false);
    
    _banner = ui::Helper::seekWidgetByName(_main_UI, "table_abs");
    _banner->setVisible(false);
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    shop->removeFromParent();
    _next->setVisible(false);
    _reset->setVisible(false);
#endif
    
    _typeContainer = ui::Helper::seekWidgetByName(_main_UI, "main_1");
    
    auto cloth = ui::Helper::seekWidgetByName(_main_UI, "top");
    cloth->setTag(CLOTH);
    cloth->addTouchEventListener(CC_CALLBACK_2(DressScene::typeBtnClicked, this));
    
    auto pant = ui::Helper::seekWidgetByName(_main_UI, "bottom");
    pant->setTag(PANT);
    pant->addTouchEventListener(CC_CALLBACK_2(DressScene::typeBtnClicked, this));
    
    auto glass = ui::Helper::seekWidgetByName(_main_UI, "glasses");
    glass->setTag(GLASS);
    glass->addTouchEventListener(CC_CALLBACK_2(DressScene::typeBtnClicked, this));
    
    auto hat = ui::Helper::seekWidgetByName(_main_UI, "hat");
    hat->setTag(HAT);
    hat->addTouchEventListener(CC_CALLBACK_2(DressScene::typeBtnClicked, this));
    
    auto toy = ui::Helper::seekWidgetByName(_main_UI, "toy");
    toy->setTag(TOY);
    toy->addTouchEventListener(CC_CALLBACK_2(DressScene::typeBtnClicked, this));
    
    _scroll = dynamic_cast<ui::ScrollView*>(ui::Helper::seekWidgetByName(_main_UI, "dress_scrollview"));
    _scroll->setVisible(false);
    
    auto up = ui::Helper::seekWidgetByName(_typeContainer, "btn_prev");
    auto down = ui::Helper::seekWidgetByName(_typeContainer, "btn_next");
    up->addTouchEventListener(CC_CALLBACK_2(DressScene::pageChangeBtnClicked, this));
    down->addTouchEventListener(CC_CALLBACK_2(DressScene::pageChangeBtnClicked, this));
}
void DressScene::onEnterTransitionDidFinish()
{
    GameBaseScene::onEnterTransitionDidFinish();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    _reset->runAction(Sequence::create(MoveBy::create(0, Vec2(0, 100)),CallFunc::create([&](){
        _reset->setVisible(true);
    }), NULL));
    _next->runAction(Sequence::create(MoveBy::create(0, Vec2(-230,518)),CallFunc::create([&](){
        _next->setVisible(true);
    }), NULL));
#endif
    
    _banner->runAction(Sequence::create(MoveBy::create(0, Vec2(300, 0)),CallFunc::create([&](){
        _banner->setVisible(true);
    }), NULL));
    
    _banner->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(1, Vec2(-300, 0))),CallFunc::create([&](){
        _tableBg->setPosition(_tableBg->getPosition() + Vec2(110 , 0));//onEnter()中设置位置不成功
        _scroll->getInnerContainer()->runAction(Sequence::create(MoveBy::create(0, Vec2(0, -800)),CallFunc::create([&](){
            _scroll->setVisible(true);
        }), NULL));
        _scroll->getInnerContainer()->runAction(EaseBounceOut::create(MoveBy::create(1, Vec2(0, 800))));
    }), NULL));
}

void DressScene::UIBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    
    auto btn = dynamic_cast<ui::Widget*>(sender);
    switch (btn->getTag()) {
        case HOME:
            GameBaseScene::homeBtnClicked(sender, type);
            break;
        case NEXT:
        {
            SimpleAudioEngine::getInstance()->playEffect("all_button.mp3");
            _main_UI->setVisible(false);
            _aDragon->setVisible(false);
            
            int width = Director::getInstance()->getVisibleSize().width;
            int height = Director::getInstance()->getVisibleSize().height;
            _main_UI->setVisible(false);
            
            for(auto decoItem : _dragDecorateLayer->getChildren())
            {
                if(dynamic_cast<EditControl*>(decoItem))
                    dynamic_cast<EditControl*>(decoItem)->hiddenTipsFrame();
            }
            RenderTexture *render = RenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888);
            render->begin();
            _dragDecorateLayer->visit();
            render->end();
            Director::getInstance()->getRenderer()->render();
            _main_UI->setVisible(true);
            _aDragon->setVisible(true);
            GameController::getInstance()->goShareScene(ChangeSceneType::REPLACE,render);
            break;
        }
        case SHOP:
            showShop();
            break;
        case RESET:
        {
            SimpleAudioEngine::getInstance()->playEffect("all_button.mp3");
            if(nullptr == this->getChildByTag(kAlertViewTag))
            {
                auto alertCallback = [](AlertViewLayer* alert ,kAlertViewButton type){
                    
                    if(type == kAlertViewButton::Left)
                    {
                        Director::getInstance()->replaceScene(DressScene::createScene());
                    }
                };
                AlertViewLayer *aler = AlertViewLayer::create(alertCallback,"UI_Json/studio_ui/public/public_btn_yes.png","UI_Json/studio_ui/public/public_btn_no.png","Are you sure you  want to reset?");
                this->addChild(aler,kGameUIZOrder+50);
            }
            break;
        }
        default:
            break;
    }
}
void DressScene::setTypeButtonEnable(bool enable)
{
    auto cloth = ui::Helper::seekWidgetByName(_main_UI, "top");
    auto pant = ui::Helper::seekWidgetByName(_main_UI, "bottom");
    auto glass = ui::Helper::seekWidgetByName(_main_UI, "glasses");
    auto hat = ui::Helper::seekWidgetByName(_main_UI, "hat");
    auto toy = ui::Helper::seekWidgetByName(_main_UI, "toy");
    
    cloth->setEnabled(enable);
    pant->setEnabled(enable);
    glass->setEnabled(enable);
    hat->setEnabled(enable);
    toy->setEnabled(enable);
}

void DressScene::typeBtnClicked(Ref *sender,ui::Widget::TouchEventType type)
{
    if(type != ui::Widget::TouchEventType::ENDED)
        return;
    
     auto btn = dynamic_cast<ui::Widget*>(sender);
    if (_isDecorateMoving) {
        
        if(_selectType != btn->getTag())
        {
//            log("+++++++++++++++++++++++++++++++");
            auto item = dynamic_cast<ui::CheckBox*>(ui::Helper::seekWidgetByTag(_typeContainer, btn->getTag()));
            item->setSelectedState(!item->getSelectedState());
        }
        return;
    }
    
    bool shouldTableMoveUp = false;
    
    if(_selectType != 0)
    {
        if(_selectType == btn->getTag())
        {
            this->setTypeButtonEnable(false);
            _isTableMoving = true;
            _tableBg->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(.5f, Vec2(110 , 0))),CallFunc::create([&](){
                _isTableMoving = false;
                _tableBg->setVisible(false);
                this->setTypeButtonEnable(true);
                
            }), NULL));
            _selectType = 0;
            return;
        }
        else
        {
            auto item = dynamic_cast<ui::CheckBox*>(ui::Helper::seekWidgetByTag(_typeContainer, _selectType));
            item->setSelectedState(false);
        }
    }
    else
    {
        shouldTableMoveUp = true;
    }
    
    _selectType = btn->getTag();
    
    switch (btn->getTag()) {
        case CLOTH:
            _data = DecorationReader::getInstance()->getDressDataVector("clothes");
            break;
        case GLASS:
            _data = DecorationReader::getInstance()->getDressDataVector("glasses");
            break;
        case HAT:
            _data = DecorationReader::getInstance()->getDressDataVector("hat");
            break;
        case TOY:
            _data = DecorationReader::getInstance()->getDressDataVector("toy");
            break;
        case PANT:
            _data = DecorationReader::getInstance()->getDressDataVector("pants");
            break;
        default:
            break;
    }
    _data.reverse();
    _table->reloadData();
    _tableBg->setVisible(true);
    if(shouldTableMoveUp)
    {
        
        this->setTypeButtonEnable(false);
        _isTableMoving = true;
        _tableBg->runAction(Sequence::create(EaseBounceOut::create(MoveBy::create(.5f, Vec2(-110,0))),CallFunc::create([&](){
            _isTableMoving = false;
            this->setTypeButtonEnable(true);
        }), NULL));
    }
    
}
#pragma mark -TableViewDataSource-
Size DressScene::cellSizeForTable(TableView *table)
{
    return Size(80, 90);
}
TableViewCell* DressScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
    DataModel *aModel = _data.at(idx);
    DragTableViewCell *pCell = DragTableViewCell::create(aModel);
    
    auto item = Sprite::create(aModel->getIconName());
    item->setPosition(40 , 45);
    pCell->addChild(item);
    if(!aModel->getIsFree() && !IAPManager::getInstance()->getItemIsBought(6))
    {
        auto lock = Sprite::create("UI_Json/studio_ui/public/pubilc_icon_lock.png");
        lock->setPosition(55, 45);
        pCell->addChild(lock);
    }
    
    return pCell;
}
ssize_t DressScene::numberOfCellsInTableView(TableView *table)
{
    return _data.size();
}
#pragma mark -TableViewDelegate-
void DressScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    if(_isTableMoving)
        return;
    if(!_data.at(cell->getIdx())->getIsFree() && !IAPManager::getInstance()->getItemIsBought(6))
    {
        showShop();
        return;
    }
    if(_selectType < TOY)
    {
        //获取骨骼
        Node* theBone;
        Map<string, cocostudio::Bone*> bones = _aDragon->getBoneDic();
        for (auto it = bones.begin(); it != bones.end(); it++)
        {
            string boneName = it->first;
            if (boneName.find(_boneNames.at(_selectType - 101)) != string::npos)
            {
                theBone = (*it).second->getDisplayRenderNode();
                break;
            }
        }
        Vec2 boneCenter = theBone->getContentSize()/2;
        
        auto moveItem = Sprite::create(_data.at(cell->getIdx())->getImageName());
        moveItem->setPosition(cell->convertToWorldSpace(Vec2(40,45)));
        this->addChild(moveItem,kGameUIZOrder+5);
        
        table->setTouchEnabled(false);
//        this->setTypeButtonEnable(false);
        _isDecorateMoving = true;
        auto callback = CallFunc::create([=](){
            _isDecorateMoving = false;
            moveItem->removeFromParent();
            auto tem = CMathUtil::randomBetween(0, 1);
            if(tem == 0)
                _dragonAnimations->play("7");
            else
                _dragonAnimations->play("8");
            
            this->dressByType(_boneNames.at(_selectType - 101), (int)(_data.size() - cell->getIdx()));
            SimpleAudioEngine::getInstance()->playEffect("decoration.mp3");
            ParticleSystemQuad *particle = ParticleSystemQuad::create("dressParticle.plist");
            particle->setPosition(theBone->convertToWorldSpace(boneCenter));
            particle->setAutoRemoveOnFinish(true);
            this->addChild(particle, 20);
        });
        moveItem->runAction(Sequence::create(MoveTo::create(.5f, theBone->convertToWorldSpace(boneCenter)),callback,NULL));
    }
    else
    {
        auto moveItem = EditControl::create(_data.at(cell->getIdx())->getImageName());
        moveItem->setPosition(cell->convertToWorldSpace(cell->getContentSize()/2));
        _dragDecorateLayer->addChild(moveItem);
        
        table->setTouchEnabled(false);
        auto callback = CallFunc::create([=](){
            table->setTouchEnabled(true);
        });
        moveItem->runAction(Sequence::create(MoveTo::create(.5f, STVisibleRect::getRealPositionForDesignPosition(Vec2(380, 270))),callback,NULL));
    }
}

void DressScene::dressByType(string type , int idx)
{
    if (nullptr == _aDragon)
    {
        log("ModelRole does not created by studio file!");
        return;
    }
    char sName[50] = {0};
    sprintf(sName, "%s1",type.c_str());
    auto pBone = _aDragon->getBone(sName);
    if(pBone)
    {
        auto pSkin = dynamic_cast<cocostudio::Skin*>(pBone->getDisplayRenderNode());
        sprintf(sName, "DressUpResource/%s/%s/%d.png",GameController::getInstance()->_dragonType.c_str(),type.c_str(),idx);
        pSkin->setVisible(true);
        pSkin->setSpriteFrame(Sprite::create(sName)->displayFrame());
        if(0 == _boneNames.at(0).compare(type))
            GameController::getInstance()->_clothIndex = idx;
        if(0 == _boneNames.at(1).compare(type))
            GameController::getInstance()->_glassIndex = idx;
        if(0 == _boneNames.at(2).compare(type))
            GameController::getInstance()->_hatIndex = idx;
        if(0 == _boneNames.at(3).compare(type))
            GameController::getInstance()->_pantIndex = idx;
        return;
    }
    int i = 0;
    int flag = 1;
    do
    {
        sprintf(sName, "%s1_%d",type.c_str(),i+1);
        pBone = _aDragon->getBone(sName);
        if (pBone)
        {
            ++flag;
            auto pSkin = dynamic_cast<cocostudio::Skin*>(pBone->getDisplayRenderNode());
            sprintf(sName, "DressUpResource/%s/%s/%d_%d.png",GameController::getInstance()->_dragonType.c_str(),type.c_str(),idx,i+1);
            pSkin->setVisible(true);
            pSkin->setSpriteFrame(Sprite::create(sName)->displayFrame());
            if(0 == _boneNames.at(0).compare(type))
                GameController::getInstance()->_clothIndex = idx;
            if(0 == _boneNames.at(1).compare(type))
                GameController::getInstance()->_glassIndex = idx;
            if(0 == _boneNames.at(2).compare(type))
                GameController::getInstance()->_hatIndex = idx;
            if(0 == _boneNames.at(3).compare(type))
                GameController::getInstance()->_pantIndex = idx;
        }
        ++i;
    }while (i<flag);
}

void DressScene::shareDragon()
{
    int width = Director::getInstance()->getVisibleSize().width;
    int height = Director::getInstance()->getVisibleSize().height;
    _main_UI->setVisible(false);
    RenderTexture *render = RenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888);
    render->begin();
    this->visit();
    render->end();
    Director::getInstance()->getRenderer()->render();
    _main_UI->setVisible(true);
    
    ShareItemView *view = ShareItemView::create(render->newImage());
    this->getParent()->addChild(view , kGameUIZOrder +10);
}
void DressScene::pageChangeBtnClicked(cocos2d::Ref *sender, ui::Widget::TouchEventType pType)
{
    if(pType != ui::Widget::TouchEventType::ENDED)
        return;
    auto widget = dynamic_cast<ui::Widget*>(sender);
    if(widget->getName().compare("btn_prev") == 0){
        _scroll->scrollToBottom(.2f, true);
    } else if(widget->getName().compare("btn_next")==0){
        _scroll->scrollToTop(.2f, true);
    }
}
#pragma mark -dragonActionCallback-
void DressScene::dragonActionCallback(cocostudio::Armature* arm, cocostudio::MovementEventType type, const std::string name)
{
    auto condition1 = (type == cocostudio::MovementEventType::LOOP_COMPLETE);
    auto condition2 = (0 == name.compare("7") || 0 == name.compare("8"));
    if(condition1 && condition2)
    {
        _dragonAnimations->play("2");
        this->setTypeButtonEnable(true);
        _table->setTouchEnabled(true);
    }
}
void DressScene::shopCloseCallback()
{
    _table->reloadData();
}