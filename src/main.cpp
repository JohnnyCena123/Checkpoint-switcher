#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "classes.hpp"

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {

    void customSetup() {
        
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) {
            log::warn("Failed to get right-button-menu.");
            return;
        }
        auto buttonSprite = CCSprite::create("checkpoint-switcher-button.png"_spr);
        buttonSprite->setScale(0.5f);
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onCheckpointSwitcher));    
        menu->addChild(button);
        menu->updateLayout();

        buttonSprite->setID("checkpoint-switcher-button"_spr);
    }

    void onCheckpointSwitcher(CCObject* sender) {
        CheckpointSwitcherLayer::create()->show();
    }
        
};