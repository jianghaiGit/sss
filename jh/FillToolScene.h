//
//  FillToolScene.h
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#ifndef __SHAKE4038_KidsJurassicAdventure__FillToolScene__
#define __SHAKE4038_KidsJurassicAdventure__FillToolScene__

#include <stdio.h>
#include "cocos2d.h"
#include "GameBaseScene.h"
#include "RoleModel.h"
#include "TouchSprite.h"
#include "RoleModel.h"
#include "DragTableView.h"
#include "SwallowTouchLayer.h"
USING_NS_CC;

class FillToolScene : public GameBaseScene , TableViewDataSource ,TableCellDragDelegate
{
public:
    FillToolScene();
    ~FillToolScene();
    
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(FillToolScene);
private:
    virtual void loadUIContent();//加载cocostudio文件
    void loadResources();
    void showTipsWithIndex(int index);
    void autoDisMissTipLayer(float);
    void disMissTipLayer();
    
    virtual void onEnterTransitionDidFinish();
    
    virtual void nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
protected:
#pragma -TableViewDataSource-
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) {
        return cellSizeForTable(table);
    };
    virtual Size cellSizeForTable(TableView *table);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
#pragma -TableCellDragDelegate-
    virtual void TableCellDragEndCallback(Sprite* dragSprite,Point point , DataModel *data);
    virtual void TableCellTouchMoveCallback(Sprite* dragSprite,Point point , DataModel *data);
private:
    DragTableView* _table;
    Vector<DataModel*> _data;
    vector<Vec2> _toolPosVector;
    RoleModel *_role;
    SwallowTouchLayer* _maskLayer;
    int _fillCount;
    ui::Widget *_main_UI;
    ui::Widget *_banner;
    ui::Widget *_next;
    TipsView *_tips;
    bool _isDraged;
};
#endif /* defined(__SHAKE4038_KidsJurassicAdventure__FillToolScene__) */
