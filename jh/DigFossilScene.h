//
//  DigFossilScene.h
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#ifndef __SHAKE4038_KidsJurassicAdventure__DigFossilScene__
#define __SHAKE4038_KidsJurassicAdventure__DigFossilScene__

#include <stdio.h>
#include "cocos2d.h"
#include "GameBaseScene.h"
#include "RoleModel.h"
#include "TouchSprite.h"
#include "CrackSoilModel.h"
#include "FossilModel.h"
#include "DataModel.h"
#include "DragTableView.h"
#include "PlistReader.h"
USING_NS_CC;

class DigFossilScene : public GameBaseScene , public TableViewDelegate,public TableViewDataSource
{
public:
    DigFossilScene();
    ~DigFossilScene();
    
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DigFossilScene);
private:
    void crackEndCallFunc();
    virtual void loadUIContent();//加载cocostudio文件
    virtual void nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void uiBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    void resultBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    void loadTools();//加载工具
    
    void toolTouched(Ref *sender);
    void aToolUseEnd();
    void toolsTips(float);
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
    FossilModel *_findItemFossil;
    Sprite      *_findItemRubbish;
    CrackSoilModel *_soil;
    ValueVector     _toolsData;
    vector<string>     _pensData;
    Vector<SpriteButton*> _allToolBtn;
    DragTableView   *_table;
    Sprite *_tableBg;
    bool _isSelectLime;
    int _currentToolIndex;
    //UI
    ui::Widget *_banner;
    ui::Widget *_ensure;
    ui::Widget *_reset;
    ui::Widget *_leftStep;
    ui::Widget *_rightStep;
    ui::Widget *_resultTip;
    ui::Widget *_next;
    bool _isResultEnable;
};

#endif /* defined(__SHAKE4038_KidsJurassicAdventure__DigFossilScene__) */
