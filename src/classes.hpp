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

        void selectCheckpoint(CheckpointObject* checkpoint);
        
        void enableApplyButton();

        CCArray* m_buttonsArray;

        static CheckpointSwitcherLayer* create();
        static CheckpointSwitcherLayer* get();

        ~CheckpointSwitcherLayer();

};

class CheckpointSelectorButton : public CCMenuItemSpriteExtra {
    private:

        CCSprite* m_checkpointSprite;
        CCLabelBMFont* m_buttonLabel;

        CCSprite* m_checkpointOutline;
        CCSprite* m_checkpointGlowOutline;

    protected:
        bool init(int buttonID, CheckpointObject* checkpoint);

        void onSelectButton(CCObject* sender);

    public:

        CheckpointObject* m_checkpoint;

        void setOutlineVisible(bool isVisible);

        int m_buttonID;

        static CheckpointSelectorButton* create(int ID, CheckpointObject* checkpoint);

        
};
