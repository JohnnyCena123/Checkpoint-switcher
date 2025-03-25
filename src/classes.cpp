#include <Geode/Geode.hpp>

#include "alphalaneous.pages_api/include/PageMenu.h"

#include "classes.hpp"

using namespace geode::prelude;



#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {

    struct Fields {
        bool m_isPracticeMode;

        bool m_hasCheckpointChanged;

        CheckpointObject* m_selectedCheckpoint;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        return true;
    } 

    void setCheckpoint(CheckpointObject* checkpoint) {
        m_currentCheckpoint = checkpoint;
        m_fields->m_selectedCheckpoint = checkpoint;
        m_fields->m_hasCheckpointChanged = true;
    }

    void loadFromCheckpoint(CheckpointObject* checkpoint) {
        if (checkpoint) PlayLayer::loadFromCheckpoint(checkpoint);
    }

    void resume() {
        PlayLayer::resume();
        if (!Mod::get()->getSavedValue<bool>("is-switcher-on")) return;

        auto selectedCheckpoint = m_fields->m_selectedCheckpoint;
        if (m_fields->m_hasCheckpointChanged) {
            if (selectedCheckpoint) storeCheckpoint(selectedCheckpoint);
            resetLevel();

            m_fields->m_hasCheckpointChanged = false;
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (!Mod::get()->getSavedValue<bool>("is-switcher-on")) return;
        
        if (m_fields->m_selectedCheckpoint) loadFromCheckpoint(m_fields->m_selectedCheckpoint);
    }

    bool getIsPracticeMode() {
        return m_fields->m_isPracticeMode;
    }

    CCArray* getCheckpoints() {
        return m_checkpointArray;
    }

    void removeCheckpoint(bool p0) {
        PlayLayer::removeCheckpoint(p0);
    }

    void togglePracticeMode(bool practiceMode) {
        
        PlayLayer::togglePracticeMode(practiceMode);

        m_fields->m_isPracticeMode = practiceMode;
    }

};

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {
    void onQuit(CCObject* sender) {
        PauseLayer::onQuit(sender);
    }
};


#include <Geode/modify/CheckpointObject.hpp>
class $modify(MyCheckpointObject, CheckpointObject) {

    struct Fields {
        float m_currentPrecentage;
    };

    bool init() {
        if (!CheckpointObject::init()) return false;
    
        m_fields->m_currentPrecentage = PlayLayer::get()->getCurrentPercent();
    
        // if (Mod::get()->getSettingValue<bool>("EnablePreviews")) {
    
        // }
    
    
        return true;
    }

    void destructor() {
        auto playLayer = static_cast<MyPlayLayer*>(PlayLayer::get());
        if (this == playLayer->m_currentCheckpoint) 
            playLayer->m_currentCheckpoint = nullptr;
            playLayer->m_fields->m_selectedCheckpoint = nullptr;
        }
};


bool CheckpointSwitcherLayer::setup() {
    bool hasFailed = false;

    s_currentLayer = this;

    this->setID("CheckpointSwitcherLayer"_spr);

    this->setTitle("Choose checkpoint");

    m_selectedButton = nullptr;
    
    m_toggleSwitcherButtonSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"); /* nullptr check */ if (!m_toggleSwitcherButtonSprite) {log::error("toggle switcher button sprite failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonCheckmarkSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"); /* nullptr check */ if (!m_toggleSwitcherButtonCheckmarkSprite) {log::error("toggle switcher button checkmark sprite failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonCheckmarkSprite->setVisible(Mod::get()->getSavedValue<bool>("is-switcher-on"));
    m_toggleSwitcherButtonSprite->addChildAtPosition(m_toggleSwitcherButtonCheckmarkSprite, Anchor::Center);
    m_toggleSwitcherButton = CCMenuItemSpriteExtra::create(m_toggleSwitcherButtonSprite, this, menu_selector(CheckpointSwitcherLayer::onToggleSwitcher)); /* nullptr check */ if (!m_toggleSwitcherButton) {log::error("toggle switcher button failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButton->ignoreAnchorPointForPosition(true);
    m_buttonMenu->addChildAtPosition(m_toggleSwitcherButton, Anchor::BottomLeft, ccp(10.f, 10.f));
    
    m_toggleSwitcherButtonLabel = CCLabelBMFont::create("Enable the switcher!", "bigFont.fnt"); /* nullptr check */ if (!m_toggleSwitcherButtonLabel) {log::error("toggle switcher button label failed to initialize."); hasFailed = true;}
    m_toggleSwitcherButtonLabel->setScale(0.33333f);
    m_mainLayer->addChildAtPosition(m_toggleSwitcherButtonLabel, Anchor::BottomLeft, ccp(110.f, 25.f));

    m_applyButtonEnabledSprite = ButtonSprite::create("Apply"); /* nullptr check */ if (!m_applyButtonEnabledSprite) {log::error("apply button enabled sprite failed to initialize."); hasFailed = true;}
    m_applyButtonDisabledSprite = ButtonSprite::create("Apply"); /* nullptr check */ if (!m_applyButtonDisabledSprite) {log::error("apply button disabled sprite failed to initialize."); hasFailed = true;}
    m_applyButtonDisabledSprite->setOpacity(155);
    m_applyButtonDisabledSprite->setColor(ccGRAY);
    m_applyButton = CCMenuItemSpriteExtra::create(m_applyButtonEnabledSprite, m_applyButtonDisabledSprite, this, menu_selector(CheckpointSwitcherLayer::onApply)); if (!m_applyButton) {log::error("apply button failed to initialize."); hasFailed = true;}
    m_applyButton->setEnabled(false);
    m_applyButton->setOpacity(30);
    m_buttonMenu->addChildAtPosition(m_applyButton, Anchor::Bottom, ccp(0, m_applyButton->getContentHeight() / 2.f + 10.f));

    m_checkpointSelectorMenu = CCMenu::create(); /* nullptr check */ if (!m_checkpointSelectorMenu) {log::error("checkpoint selector menu failed to initialize."); hasFailed = true;}
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

    m_currentPlayLayer = static_cast<MyPlayLayer*>(PlayLayer::get());
    m_checkpoints = m_currentPlayLayer->getCheckpoints();
    m_isPracticeMode = m_currentPlayLayer->getIsPracticeMode();

    m_buttonsArray = CCArray::create(); 

    m_checkpointIndicatorsNode = CCNode::create();
    
    if (!m_isPracticeMode) {
        auto practiceOffLabel = CCNode::create(); 
        auto practiceOffLabelTop = CCLabelBMFont::create("Enable practice mode", "bigFont.fnt"); /* nullptr check */ if (!practiceOffLabelTop) {log::error("practice off label top failed to initialize."); hasFailed = true;}
        auto practiceOffLabelBottom = CCLabelBMFont::create("to use the mod!", "bigFont.fnt"); /* nullptr check */ if (!practiceOffLabelBottom) {log::error("practice off label bottom failed to initialize."); hasFailed = true;}

        practiceOffLabelTop->setPositionY(20);
        practiceOffLabelBottom->setPositionY(-20);

        practiceOffLabel->setPosition(m_mainLayer->getContentWidth() / 2.f, m_mainLayer->getContentHeight() / 2.f);

        m_mainLayer->addChild(practiceOffLabel);
        practiceOffLabel->addChild(practiceOffLabelTop);
        practiceOffLabel->addChild(practiceOffLabelBottom);

        practiceOffLabel->setID("practice-off-label");


        m_applyButton->removeFromParent();


        m_mainLayer->addChild(m_checkpointIndicatorsNode);


    } else {

        auto progressBar = PlayLayer::get()->m_progressBar;
        auto progressBarFilling = PlayLayer::get()->m_progressFill;
    
        m_progressBarClone = CCSprite::create("slidergroove2.png");
        m_progressBarCloneFilling = CCSprite::create("sliderBar2.png");
        m_progressBarCloneFilling->setColor(ccc3(125, 255, 0));
        m_progressBarCloneFilling->setContentSize(progressBarFilling->getContentSize());
        m_progressBarCloneFilling->setAnchorPoint(ccp(0.f, 0.f));
        m_progressBarCloneFilling->setZOrder(-1);
        m_progressBarCloneFilling->setTextureRect({
            0, 0,
            (progressBar->getTextureRect().getMaxX() - 5) * PlayLayer::get()->getCurrentPercent() / 100.f,
            progressBar->getTextureRect().getMaxY() / 2
        });
        ccTexParams texParams = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        m_progressBarCloneFilling->getTexture()->setTexParameters(&texParams);
        m_progressBarClone->addChildAtPosition(m_progressBarCloneFilling, Anchor::BottomLeft, ccp(2.f, 4.f));
        m_mainLayer->addChildAtPosition(m_progressBarClone, Anchor::Center, progressBar->getPosition() - CCDirector::get()->getWinSize() / 2);
    
        m_progressBarClone->setID("progress-bar-clone");
        m_progressBarCloneFilling->setID("progress-bar-clone-filling");
    
        m_checkpointIndicatorsNode = CCNode::create();
        m_checkpointIndicatorsNode->setAnchorPoint(ccp(0.5f, 0.5f));
        m_checkpointIndicatorsNode->setContentSize({progressBar->getContentWidth(), 8.f});
        m_progressBarClone->addChildAtPosition(m_checkpointIndicatorsNode, Anchor::Center);
    
        m_checkpointIndicatorsNode->setID("checkpoint-indicators-node");


        // todo: make this work
        
        // m_firstCheckpointButton = CheckpointSelectorButton::create(0, nullptr); /* nullptr check */ if (!m_firstCheckpointButton) {log::error("first checkpoint button failed to initialize."); hasFailed = true;}
        // m_checkpointSelectorMenu->addChild(m_firstCheckpointButton);

        for (int i = 0; i < m_checkpoints->count(); i++) {

            auto checkpoint = static_cast<MyCheckpointObject*>(m_checkpoints->objectAtIndex(i));
            
            auto checkpointIndicatorSprite = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
            auto checkpointIndicatorLine = CCLabelBMFont::create("|", "chatFont.fnt");
            checkpointIndicatorSprite->setAnchorPoint(ccp(0.5f, 0.f));
            checkpointIndicatorLine->setAnchorPoint(ccp(0.5f, 0.f));
            checkpointIndicatorSprite->setScale(0.5f);
            checkpointIndicatorSprite->addChildAtPosition(checkpointIndicatorLine, Anchor::Top, ccp(0.f, 2.f));
            m_checkpointIndicatorsNode->addChildAtPosition(checkpointIndicatorSprite, Anchor::BottomLeft, ccp(
                progressBar->getContentWidth() * checkpoint->m_fields->m_currentPrecentage / 100.f + 2.f,
                - checkpointIndicatorLine->getContentHeight() - 6.f 
            ));

            checkpointIndicatorLine->setID(fmt::format("checkpoint-indicator-line-no-{}", i + 1).c_str());
            checkpointIndicatorSprite->setID(fmt::format("checkpoint-indicator-no-{}", i + 1).c_str());


            auto checkpointButton = CheckpointSelectorButton::create(i + 1, checkpoint); /* nullptr check */ if (!checkpointButton) {log::error("checkpoint button no. {} failed to initialize.", i + 1); hasFailed = true;}
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

    if (hasFailed) return false;
    return true;
}

void CheckpointSwitcherLayer::onToggleSwitcher(CCObject* sender) {
    bool isSwitcherOn = Mod::get()->getSavedValue<bool>("is-switcher-on");
    log::debug("the switcher is now {}.", isSwitcherOn ? "on" : "disabled");
    Mod::get()->setSavedValue("is-switcher-on", isSwitcherOn ? false : true);
    m_toggleSwitcherButtonCheckmarkSprite->setVisible(isSwitcherOn);
}
 
void CheckpointSwitcherLayer::onApply(CCObject* sender) {
    m_currentPlayLayer->setCheckpoint(m_selectedCheckpoint);
    m_applyButton->setEnabled(false);
}

void CheckpointSwitcherLayer::selectCheckpoint(MyCheckpointObject* checkpoint) {
    m_selectedCheckpoint = static_cast<CheckpointObject*>(checkpoint);
}

void CheckpointSwitcherLayer::enableApplyButton() {
    m_applyButton->setEnabled(true);
    m_applyButton->setOpacity(255);
    m_applyButton->setColor(ccWHITE);
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::create() {
    auto ret = new CheckpointSwitcherLayer();
    if (ret->initAnchored(480.f, 240.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::get() { 
    return s_currentLayer;
}

CheckpointSwitcherLayer::~CheckpointSwitcherLayer() {
    s_currentLayer = nullptr;
}

CheckpointSwitcherLayer* CheckpointSwitcherLayer::s_currentLayer = nullptr;




    
bool CheckpointSelectorButton::init(int buttonID, MyCheckpointObject* checkpoint) {
    bool hasFailed = false;

    m_checkpointSprite = CCSprite::createWithSpriteFrameName("checkpoint_01_001.png"); /* nullptr check */ if (!m_checkpointSprite) {log::error("checkpoint sprite failed to initialize."); hasFailed = true;} 
    m_checkpointSprite->setScale(80 / m_checkpointSprite->getContentHeight());
    m_mainNode = CCNode::create();
    m_mainNode->setContentSize(m_checkpointSprite->getScaledContentSize());
    m_mainNode->addChildAtPosition(m_checkpointSprite, Anchor::Center);
    
    if (!CCMenuItemSpriteExtra::init(m_mainNode, m_mainNode, this, menu_selector(CheckpointSelectorButton::onSelectButton))) {
        log::error("CCMenuItemSpriteExtra failed to initialize."); 
        hasFailed = true;
    }

    m_buttonID = buttonID;
    m_checkpoint = checkpoint;

    m_checkpointOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_color_001.png"); /* nullptr check */ if (!m_checkpointOutline) {log::error("checkpoint outline failed to initialize."); hasFailed = true;}
    m_checkpointOutline->setVisible(false);
    m_checkpointOutline->setColor(ccc3(255, 243, 69));
    m_checkpointSprite->addChildAtPosition(m_checkpointOutline, Anchor::Center);

    m_checkpointGlowOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_glow_001.png"); /* nullptr check */ if (!m_checkpointGlowOutline) {log::error("checkpoint glow outline failed to initialize."); hasFailed = true;}
    m_checkpointGlowOutline->setColor(ccc3(255, 253, 137));
    m_checkpointOutline->addChildAtPosition(m_checkpointGlowOutline, Anchor::Center);

    m_buttonLabel = CCLabelBMFont::create(m_checkpoint ? fmt::format("Checkpoint at {}%", (int)m_checkpoint->m_fields->m_currentPrecentage).c_str() : "The start!", "bigFont.fnt"); /* nullptr check */ if (!m_buttonLabel) {log::error("button label failed to initialize."); hasFailed = true;}
    m_mainNode->addChildAtPosition(m_buttonLabel, Anchor::Top, ccp(0.f, 10.f));
    m_buttonLabel->setScale(0.4);

    m_mainNode->setID("main-node");
    m_checkpointSprite->setID("checkpoint-sprite");
    m_buttonLabel->setID("checkpoint-label");
    m_checkpointOutline->setID("checkpoint-outline");
    m_checkpointGlowOutline->setID("checkpoint-glow-outline");
    this->setID(fmt::format("checkpoint-button-no-{}", m_buttonID).c_str());

    if (hasFailed) return false;
    return true;
    
}

void CheckpointSelectorButton::onSelectButton(CCObject* sender) {

    auto lastSelectedButton = CheckpointSwitcherLayer::get()->m_selectedButton;

    // todo: make it work with the first button
    CheckpointSwitcherLayer::get()->selectCheckpoint(m_checkpoint);
    CheckpointSwitcherLayer::get()->enableApplyButton();

    if (lastSelectedButton != nullptr) lastSelectedButton->setOutlineVisible(false);
    this->setOutlineVisible(true);

    if (lastSelectedButton != nullptr) lastSelectedButton->changeScale(false);
    this->changeScale(true);

    CheckpointSwitcherLayer::get()->m_selectedButton = this;
} 

void CheckpointSelectorButton::changeScale(bool toScaleUp) {
    if (toScaleUp != m_isScaledUp) {
        m_mainNode->runAction(CCEaseInOut::create(CCScaleTo::create(0.1f, toScaleUp ? 1.25f : 1.f), 2.f));
        m_isScaledUp = toScaleUp;
    }
}

void CheckpointSelectorButton::setOutlineVisible(bool isVisible) {
    m_checkpointOutline->setVisible(isVisible);
}

CCSprite* CheckpointSelectorButton::getSprite() {
    return m_checkpointSprite;
}

CheckpointSelectorButton* CheckpointSelectorButton::create(int buttonID, MyCheckpointObject* checkpoint) {
    auto ret = new CheckpointSelectorButton();
    if (ret->init(buttonID, checkpoint)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
