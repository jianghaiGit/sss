//
//  DetectingScene.h
//  SHAKE4038_KidsJurassicAdventure
//
//  Created by jianghai on 15/5/14.
//
//

#ifndef __SHAKE4038_KidsJurassicAdventure__DetectingScene__
#define __SHAKE4038_KidsJurassicAdventure__DetectingScene__

#include <stdio.h>
#include "cocos2d.h"
#include "GameBaseScene.h"
#include "DetectingModel.h"
USING_NS_CC;

class DetectingScene : public GameBaseScene,DragSpriteDelegate
{
public:
    DetectingScene();
    ~DetectingScene();
    
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DetectingScene);
private:
    virtual void loadUIContent();//加载cocostudio文件
    virtual void shopCloseCallback();
    virtual void nextBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void homeBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    virtual void tipViewBtnClicked(Ref *sender,ui::Widget::TouchEventType type);
    
    void machineItemTouched(Ref *sender);
    void goDigScene(float);
    void enableDetecting(float);
    virtual void onEnterTransitionDidFinish();
protected:
#pragma mark -DragSpriteDelegate-
    virtual void DragSpriteTouchBegin(Touch *touch , DragSprite* aDragSprite);
    virtual void DragSpriteTouchMove(Touch *touch , DragSprite* aDragSprite);
    virtual void DragSpriteTouchEnd(Touch *touch , DragSprite* aDragSprite);
    virtual void DragSpriteTouchCancel(Touch *touch , DragSprite* aDragSprite);
private:
    DetectingModel* _machine;
    std::vector<Vec2> _posVector;
    ui::Widget* _banner;
    MachineType _choosedMachineType;
    bool _canDetecting;
    ui::Widget* _tipView;
};
#endif /* defined(__SHAKE4038_KidsJurassicAdventure__DetectingScene__) */
