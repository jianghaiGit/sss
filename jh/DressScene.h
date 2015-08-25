//
// DressScene.h
// SHAKE4038_KidsJurassicAdventure
//
// Created by jianghai on 15/5/30.
//
//

#ifndef __SHAKE4038_KidsJurassicAdventure__DressScene__
#define __SHAKE4038_KidsJurassicAdventure__DressScene__

#include "cocos2d.h"
#include "GameBaseScene.h"
#include "DragTableView.h"
#include "EditControl.h"
USING_NS_CC;

# define SelectType int

class DressScene : public GameBaseScene, TableViewDataSource,TableViewDelegate
{
public:
    DressScene();
    ~DressScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DressScene);
private:
    void initData();
    virtual void loadUIContent();//加载cocostudio文件
    virtual void shopCloseCallback();
    virtual void UIBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void typeBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    void setTypeButtonEnable(bool enable);
    
    virtual void onEnterTransitionDidFinish();
    void dressByType(string type , int idx);
    void shareDragon();
    void pageChangeBtnClicked(cocos2d::Ref *sender, ui::Widget::TouchEventType pType);
    void dragonActionCallback(cocostudio::Armature* arm, cocostudio::MovementEventType type, const std::string name);
protected:
#pragma -TableViewDataSource-
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) {
        return cellSizeForTable(table);
    };
    virtual Size cellSizeForTable(TableView *table);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
#pragma -TableViewDelegate-
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
private:
    cocostudio::Armature* _aDragon;
    cocostudio::ArmatureAnimation *_dragonAnimations;
    ui::Widget* _tableBg;
    ui::Widget* _main_UI;
    ui::Widget* _banner;
    ui::ScrollView* _scroll;
    DragTableView *_table;
    Vector<DataModel*> _data;
    ui::Widget *_typeContainer;
    SelectType _selectType;
    vector<string> _boneNames;
    Layer *_dragDecorateLayer;
    bool _isTableMoving;
    bool _isDecorateMoving;
    ui::Widget *_next;
    ui::Widget *_reset;
};

#endif /* defined(__SHAKE4038_KidsJurassicAdventure__DressScene__) */