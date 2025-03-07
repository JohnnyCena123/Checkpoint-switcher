#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MyPlayLayer;

class CheckpointSelectorButton;

class CheckpointSwitcherLayer : public Popup<> {
    private:
        
        CCSprite* m_toggleSwitcherButtonSprite;
        CCSprite* m_toggleSwitcherButtonCheckmarkSprite;
        CCLabelBMFont* m_toggleSwitcherButtonLabel;
        CCMenuItemSpriteExtra* m_toggleSwitcherButton;

        CCMenu* m_checkpointSelectorMenu;

        MyPlayLayer* m_currentPlayLayer;
        CCArray* m_checkpoints;
        bool m_isPracticeMode;

        CheckpointObject* m_selectedCheckpoint;

        ButtonSprite* m_applyButtonEnabledSprite;
        ButtonSprite* m_applyButtonDisabledSprite;
        CCMenuItemSpriteExtra* m_applyButton;

        static CheckpointSwitcherLayer* s_currentLayer;

    protected:

        bool setup() override;

        void onToggleSwitcher(CCObject* sender);

        void onApply(CCObject* sender);

    public:

        CheckpointSelectorButton* m_selectedButton;

        CCArray* m_buttonsArray;

        void selectCheckpoint(CheckpointObject* checkpoint);
        
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

    protected:

        bool init(int buttonID, CheckpointObject* checkpoint);

        void onSelectButton(CCObject* sender);

    public:

        CheckpointObject* m_checkpoint;

        int m_buttonID;

        void setOutlineVisible(bool isVisible);

        CCSprite* getSprite();

        static CheckpointSelectorButton* create(int ID, CheckpointObject* checkpoint);

        
};
