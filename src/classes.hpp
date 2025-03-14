<<<<<<< HEAD
#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MyPlayLayer;
class MyCheckpointObject;

class CheckpointSelectorButton;

class CheckpointSwitcherLayer : public Popup<> {
private:
        
    CCSprite* m_toggleSwitcherButtonSprite;
    CCSprite* m_toggleSwitcherButtonCheckmarkSprite;
    CCNode* m_toggleSwitcherButtonLabel;
    CCMenuItemSpriteExtra* m_toggleSwitcherButton;

    CCMenu* m_checkpointSelectorMenu;

    MyPlayLayer* m_currentPlayLayer;
    CCArray* m_checkpoints;
    bool m_isPracticeMode;

    MyCheckpointObject* m_selectedCheckpoint;

    ButtonSprite* m_applyButtonEnabledSprite;
    ButtonSprite* m_applyButtonDisabledSprite;
    CCMenuItemSpriteExtra* m_applyButton;


    static CheckpointSwitcherLayer* s_currentLayer;

protected:

    bool setup() override;

    void onToggleSwitcher(CCObject* sender);

    void onApply(CCObject* sender);

public:

    CCSprite* m_progressBarClone;
    CCSprite* m_progressBarCloneFilling;
    CCNode* m_checkpointIndicatorsNode;

    CheckpointSelectorButton* m_selectedButton;

    CCArray* m_buttonsArray;

    void selectCheckpoint(MyCheckpointObject* checkpoint);
        
    void enableApplyButton();


    static CheckpointSwitcherLayer* create();
    static CheckpointSwitcherLayer* get();


    ~CheckpointSwitcherLayer();

};

class CheckpointSelectorButton : public CCMenuItemSpriteExtra {
private:

    CCNode* m_mainNode;

    CCSprite* m_checkpointSprite;
    CCLabelBMFont* m_buttonLabel;

    CCSprite* m_checkpointOutline;
    CCSprite* m_checkpointGlowOutline;

    bool m_isScaledUp;

protected:

    bool init(int buttonID, MyCheckpointObject* checkpoint);

    void onSelectButton(CCObject* sender);

public:

    CCMenuItem* m_containerMenuItem;
    CCMenu* m_containerMenu;

    MyCheckpointObject* m_checkpoint;
    int m_buttonID;

    void changeScale(bool toScaleUp);

    void setOutlineVisible(bool isVisible);

    CCSprite* getSprite();

    static CheckpointSelectorButton* create(int ID, MyCheckpointObject* checkpoint);

        
};
=======
#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MyPlayLayer;
class MyCheckpointObject;

class CheckpointSelectorButton;

class CheckpointSwitcherLayer : public Popup<> {
private:
        
    CCSprite* m_toggleSwitcherButtonSprite;
    CCSprite* m_toggleSwitcherButtonCheckmarkSprite;
    CCNode* m_toggleSwitcherButtonLabel;
    CCMenuItemSpriteExtra* m_toggleSwitcherButton;

    CCMenu* m_checkpointSelectorMenu;

    MyPlayLayer* m_currentPlayLayer;
    CCArray* m_checkpoints;
    bool m_isPracticeMode;

    MyCheckpointObject* m_selectedCheckpoint;

    ButtonSprite* m_applyButtonEnabledSprite;
    ButtonSprite* m_applyButtonDisabledSprite;
    CCMenuItemSpriteExtra* m_applyButton;


    static CheckpointSwitcherLayer* s_currentLayer;

protected:

    bool setup() override;

    void onToggleSwitcher(CCObject* sender);

    void onApply(CCObject* sender);

public:

    CCSprite* m_progressBarClone;
    CCSprite* m_progressBarCloneFilling;
    CCNode* m_checkpointIndicatorsNode;

    CheckpointSelectorButton* m_selectedButton;

    CCArray* m_buttonsArray;

    void selectCheckpoint(MyCheckpointObject* checkpoint);
        
    void enableApplyButton();


    static CheckpointSwitcherLayer* create();
    static CheckpointSwitcherLayer* get();


    ~CheckpointSwitcherLayer();

};

class CheckpointSelectorButton : public CCMenuItemSpriteExtra {
private:

    CCNode* m_mainNode;

    CCSprite* m_checkpointSprite;
    CCLabelBMFont* m_buttonLabel;

    CCSprite* m_checkpointOutline;
    CCSprite* m_checkpointGlowOutline;

    bool m_isScaledUp;

protected:

    bool init(int buttonID, MyCheckpointObject* checkpoint);

    void onSelectButton(CCObject* sender);

public:

    CCMenuItem* m_containerMenuItem;
    CCMenu* m_containerMenu;

    MyCheckpointObject* m_checkpoint;
    int m_buttonID;

    void changeScale(bool toScaleUp);

    void setOutlineVisible(bool isVisible);

    CCSprite* getSprite();

    static CheckpointSelectorButton* create(int ID, MyCheckpointObject* checkpoint);

        
};
>>>>>>> 61e551d4825c78ffcb23a002d1b5dab2f0ccf7ef
