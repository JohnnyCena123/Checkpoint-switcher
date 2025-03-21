#include <Geode/Geode.hpp>

#include "alphalaneous.pages_api/include/PageMenu.h"

#include "classes.hpp"

using namespace geode::prelude;




bool isSwitcherOn = true;

#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {

    struct Fields {
        bool m_isPracticeMode = false;

        CheckpointObject* m_selectedCheckpoint = nullptr;

        bool m_hasCheckpointChanged = false;
    };

    // bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    //     if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
    //     return true;
    // } 

    void setCheckpoint(CheckpointObject* checkpoint) {
        m_fields->m_selectedCheckpoint = checkpoint;
        log::debug("set checkpoint!");
    }

    void resume() {
        PlayLayer::resume();
        if (m_fields->m_selectedCheckpoint && m_fields->m_hasCheckpointChanged) { 
            loadFromCheckpoint(m_fields->m_selectedCheckpoint);
            m_currentCheckpoint = m_fields->m_selectedCheckpoint;
            m_fields->m_hasCheckpointChanged = false;
            log::debug("successfully loaded from checkpoint on resume!");
        }
        log::debug("resumed!");
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (m_fields->m_selectedCheckpoint) { 
            loadFromCheckpoint(m_fields->m_selectedCheckpoint);
            m_currentCheckpoint = m_fields->m_selectedCheckpoint;
            log::debug("successfully loaded from checkpoint on reset level!");
        }
        log::debug("reset level!");
    }

    bool getIsPracticeMode() {
        return m_fields->m_isPracticeMode;
        log::debug("got is practice mode!");
    }

    CCArray* getCheckpoints() {
        return m_checkpointArray;
        log::debug("got checkpoints!");
    }

    void togglePracticeMode(bool practiceMode) {
        
        PlayLayer::togglePracticeMode(practiceMode);

        m_fields->m_isPracticeMode = practiceMode;
        log::debug("toggled practice mode!");
    }
        
};

#include <Geode/modify/CheckpointObject.hpp>
class $modify(MyCheckpointObject, CheckpointObject) {

    struct Fields {
        
    };

    bool init() {
        if (!CheckpointObject::init()) return false;
        
        if (Mod::get()->getSettingValue<bool>("EnablePreviews")) {

        }

        log::debug("initialized checkpoint object!");
        return true;
    }

};

bool CheckpointSwitcherLayer::setup() {
    bool hasFailed = false;

    s_currentLayer = this;

    this->setID("CheckpointSwitcherLayer"_spr);

    this->setTitle("Choose checkpoint");

    m_selectedButton = nullptr;
    
    m_toggleSwitcherButtonSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"); if (!m_toggleSwitcherButtonSprite) {log::error("toggle switcher button sprite failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonCheckmarkSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"); if (!m_toggleSwitcherButtonCheckmarkSprite) {log::error("toggle switcher button checkmark sprite failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonCheckmarkSprite->setVisible(isSwitcherOn);
    m_toggleSwitcherButtonSprite->addChildAtPosition(m_toggleSwitcherButtonCheckmarkSprite, Anchor::Center);
    m_toggleSwitcherButton = CCMenuItemSpriteExtra::create(m_toggleSwitcherButtonSprite, this, menu_selector(CheckpointSwitcherLayer::onToggleSwitcher)); if (!m_toggleSwitcherButton) {log::error("toggle switcher button failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButton->ignoreAnchorPointForPosition(true);
    m_buttonMenu->addChildAtPosition(m_toggleSwitcherButton, Anchor::BottomLeft, ccp(10.f, 10.f));
    
    m_toggleSwitcherButtonLabel = CCLabelBMFont::create("Enable the switcher!", "bigFont.fnt"); if (!m_toggleSwitcherButtonLabel) {log::error("toggle switcher button label failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonLabel->setScale(0.33333f);
    m_mainLayer->addChildAtPosition(m_toggleSwitcherButtonLabel, Anchor::BottomLeft, ccp(110, 25));

    m_applyButtonEnabledSprite = ButtonSprite::create("Apply"); if (!m_applyButtonEnabledSprite) {log::error("apply button enabled sprite failed to initialize."); hasFailed = true;}
    m_applyButtonDisabledSprite = ButtonSprite::create("Apply"); if (!m_applyButtonDisabledSprite) {log::error("apply button disabled sprite failed to initialize."); hasFailed = true;}
    m_applyButtonDisabledSprite->setOpacity(155);
    m_applyButtonDisabledSprite->setColor(ccGRAY);
    m_applyButton = CCMenuItemSpriteExtra::create(m_applyButtonEnabledSprite, m_applyButtonDisabledSprite, this, menu_selector(CheckpointSwitcherLayer::onApply)); if (!m_applyButton) {log::error("apply button failed to initialize."); hasFailed = true;}
    m_applyButton->setEnabled(false);
    m_applyButton->setOpacity(30);
    m_buttonMenu->addChildAtPosition(m_applyButton, Anchor::Bottom, ccp(0, m_applyButton->getContentHeight() / 2.f + 10.f));

    m_checkpointSelectorMenu = CCMenu::create(); if (!m_checkpointSelectorMenu) {log::error("checkpoint selector menu failed to initialize."); hasFailed = true;}
    m_checkpointSelectorMenu->setLayout(
        RowLayout::create()
            ->setGap(30.f)
            ->setGrowCrossAxis(false)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisOverflow(false)
    );
    m_mainLayer->addChildAtPosition(m_checkpointSelectorMenu, Anchor::Center);
    m_checkpointSelectorMenu->setContentWidth(m_mainLayer->getContentWidth());

    auto pagedCheckpointSelectorMenu = static_cast<PageMenu*>(m_checkpointSelectorMenu);
    pagedCheckpointSelectorMenu->setPaged(3, HORIZONTAL, 600.f, 4);

    m_currentPlayLayer = static_cast<MyPlayLayer*>(CCScene::get()->getChildByID("PlayLayer"));
    m_checkpoints = m_currentPlayLayer->getCheckpoints();
    m_isPracticeMode = m_currentPlayLayer->getIsPracticeMode();

    
    m_buttonsArray = CCArray::create(); if (!m_buttonsArray) {log::error("buttons array failed to initialize."); hasFailed = true;}
    

    if (!m_isPracticeMode) {
        auto practiceOffLabel = CCNode::create(); if (!practiceOffLabel) {log::error("practice off label (node) failed to initialize."); hasFailed = true;}
        auto practiceOffLabelTop = CCLabelBMFont::create("Enable practice mode", "bigFont.fnt"); if (!practiceOffLabelTop) {log::error("practice off label top failed to initialize."); hasFailed = true;}
        auto practiceOffLabelBottom = CCLabelBMFont::create("to use the mod!", "bigFont.fnt"); if (!practiceOffLabelBottom) {log::error("practice off label bottom failed to initialize."); hasFailed = true;}

        practiceOffLabelTop->setPositionY(20);
        practiceOffLabelBottom->setPositionY(-20);

        practiceOffLabel->setPosition(m_mainLayer->getContentWidth() / 2.f, m_mainLayer->getContentHeight() / 2.f);

        m_mainLayer->addChild(practiceOffLabel);
        practiceOffLabel->addChild(practiceOffLabelTop);
        practiceOffLabel->addChild(practiceOffLabelBottom);

        practiceOffLabel->setID("label-practice-off");


        m_applyButton->removeFromParent();

    } else {
        
        for (int i = 0; i < m_checkpoints->count(); i++) {

            auto checkpoint = static_cast<CheckpointObject*>(m_checkpoints->objectAtIndex(i));

            auto checkpointButton = CheckpointSelectorButton::create(i + 1, checkpoint); if (!checkpointButton) {log::error("checkpoint button no. {} failed to initialize.", i + 1); hasFailed = true;}
            m_checkpointSelectorMenu->addChild(checkpointButton);
            m_buttonsArray->addObject(checkpointButton);

        }

        m_checkpointSelectorMenu->updateLayout();

    }


    
    m_toggleSwitcherButtonSprite->setID("toggle-switcher-checkbox");
    m_toggleSwitcherButtonCheckmarkSprite->setID("toggle-switcher-checkbox-checkmark");
    m_toggleSwitcherButtonLabel->setID("toggle-switcher-label");
    m_toggleSwitcherButton->setID("toggle-checkpoint-switcher-button");
    m_applyButtonEnabledSprite->setID("apply-button-enabled-sprite");
    m_applyButtonDisabledSprite->setID("apply-button-disabled-sprite");
    m_applyButton->setID("apply-button");
    m_checkpointSelectorMenu->setID("checkpoint-selector-menu");

    m_buttonMenu->setID("button-menu");
    m_mainLayer->setID("main-layer");
    m_title->setID("title");
    m_closeBtn->setID("close-button");
    m_bgSprite->setID("background-sprite");

    if (hasFailed) {return false; log::error("failed to initialize checkpoint switcher layer.");}
    log::debug("initialized checkpoint switcher layer!");
    return true;
}

void CheckpointSwitcherLayer::onToggleSwitcher(CCObject* sender) {
    isSwitcherOn = !isSwitcherOn;
    m_toggleSwitcherButtonCheckmarkSprite->setVisible(isSwitcherOn);
    log::debug("toggled switcher!");
}

void CheckpointSwitcherLayer::onApply(CCObject* sender) {
    m_currentPlayLayer->setCheckpoint(m_selectedCheckpoint);
    m_currentPlayLayer->m_fields->m_hasCheckpointChanged = true;
    m_applyButton->setEnabled(false);
    log::debug("applied!");
}

void CheckpointSwitcherLayer::selectCheckpoint(CheckpointObject* checkpoint) {
    m_selectedCheckpoint = checkpoint;
    log::debug("selected checkpoint!");
}

void CheckpointSwitcherLayer::enableApplyButton() {
    m_applyButton->setEnabled(true);
    m_applyButton->setOpacity(255);
    m_applyButton->setColor(ccWHITE);
    log::debug("enabled apply button!");
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::create() {
    auto ret = new CheckpointSwitcherLayer();
    if (ret->initAnchored(480.f, 240.f)) {
        ret->autorelease();
        log::debug("created checkpoint switcher layer!");
        return ret;
    }
    log::error("failed to create checkpoint switcher layer.");

    delete ret;
    return nullptr;
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::get() { 
    log::debug("got checkpoint switcher layer!");
    return s_currentLayer;
}

CheckpointSwitcherLayer::~CheckpointSwitcherLayer() {
    s_currentLayer = nullptr;
    log::debug("destructed checkpoint switcher layer!");
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::s_currentLayer = nullptr;

    
    
bool CheckpointSelectorButton::init(int buttonID, CheckpointObject* checkpoint) {
    bool hasFailed = false;

    m_mainNode = CCNode::create();
    m_checkpointSprite = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png"); if (!m_checkpointSprite) {log::error("checkpoint sprite failed to initialize."); hasFailed = true;} 
    m_checkpointSprite->setScale(80 / m_checkpointSprite->getContentHeight());
    m_mainNode->setContentSize(m_checkpointSprite->getScaledContentSize());
    m_mainNode->addChildAtPosition(m_checkpointSprite, Anchor::Center);
    
    if (!CCMenuItemSpriteExtra::init(m_mainNode, m_mainNode, this, menu_selector(CheckpointSelectorButton::onSelectButton))) return false;

    m_checkpoint = checkpoint;

    m_buttonID = buttonID;

    m_isScaledUp = false;

    m_checkpointOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_color_001.png"); if (!m_checkpointOutline) {log::error("checkpoint outline failed to initialize."); hasFailed = true;}
    m_checkpointOutline->setVisible(false);
    m_checkpointOutline->setColor(ccc3(255, 243, 69));
    m_checkpointSprite->addChildAtPosition(m_checkpointOutline, Anchor::Center);

    m_checkpointGlowOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_glow_001.png"); if (!m_checkpointGlowOutline) {log::error("checkpoint glow outline failed to initialize."); hasFailed = true;}
    m_checkpointGlowOutline->setColor(ccc3(255, 253, 137));
    m_checkpointOutline->addChildAtPosition(m_checkpointGlowOutline, Anchor::Center);

    m_buttonLabel = CCLabelBMFont::create(fmt::format("Checkpoint #{}", buttonID).c_str(), "bigFont.fnt"); if (!m_buttonLabel) {log::error("button label failed to initialize."); hasFailed = true;}
    m_checkpointSprite->addChildAtPosition(m_buttonLabel, Anchor::Top, ccp(0.f, 5.f));
    m_buttonLabel->setScale(0.2);

    m_mainNode->setID("main-node");
    m_checkpointSprite->setID("checkpoint-sprite");
    m_buttonLabel->setID("checkpoint-label");
    m_checkpointOutline->setID("checkpoint-outline");
    m_checkpointGlowOutline->setID("checkpoint-glow-outline");
    this->setID(fmt::format("checkpoint-button-no-{}", buttonID + 1).c_str());

    if (hasFailed) {return false; log::error("failed to initialize checkpoint selector button no. {}.", m_buttonID);}
    log::debug("initialized checkpoint selector button no. {}!", m_buttonID);
    return true;
    
}

void CheckpointSelectorButton::onSelectButton(CCObject* sender) {

    auto currentPlayLayer = static_cast<MyPlayLayer*>(CCScene::get()->getChildByID("PlayLayer"));

    auto lastSelectedButton = CheckpointSwitcherLayer::get()->m_selectedButton;

    CheckpointSwitcherLayer::get()->selectCheckpoint(m_checkpoint);
    CheckpointSwitcherLayer::get()->enableApplyButton();

    if (lastSelectedButton != nullptr) lastSelectedButton->setOutlineVisible(false);
    this->setOutlineVisible(true);

    if (lastSelectedButton != nullptr) lastSelectedButton->changeScale(false);
    this->changeScale(true);

    CheckpointSwitcherLayer::get()->m_selectedButton = this;
    log::debug("selected checkpoint selector button no. {}!", m_buttonID);
} 

void CheckpointSelectorButton::changeScale(bool toScaleUp) {
    if (toScaleUp != m_isScaledUp) {
        log::debug("toScaleUp: {}, m_isScaledUp: {}", toScaleUp, m_isScaledUp);
        m_mainNode->runAction(CCEaseInOut::create(CCScaleTo::create(0.1f, toScaleUp ? 1.25f : 1.f), 2.f));
        m_isScaledUp = toScaleUp;
        log::debug("changed scale of checkpoint selector button no. {}!", m_buttonID);
        return;
    }
    log::debug("did not change scale of checkpoint selector button no. {}.", m_buttonID);
}

void CheckpointSelectorButton::setOutlineVisible(bool isVisible) {
    m_checkpointOutline->setVisible(isVisible);
    log::debug("set the outline of checkpoint selector button no. {} to {} be visible!", m_buttonID, isVisible);
}

CCSprite* CheckpointSelectorButton::getSprite() {
    return m_checkpointSprite;
    log::debug("got the sprite of checkpoint selector button no. {}!", m_buttonID);
}

CheckpointSelectorButton* CheckpointSelectorButton::create(int buttonID, CheckpointObject* checkpoint) {
    auto ret = new CheckpointSelectorButton();
    if (ret->init(buttonID, checkpoint)) {
        ret->autorelease();
        log::debug("created checkpoint selector button no. {}!", buttonID);
        return ret;
    }
    log::debug("failed to create checkpoint selector button no. {}.", buttonID);

    delete ret;
    return nullptr;
}

