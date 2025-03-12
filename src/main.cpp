#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "classes.hpp"

$execute {
    if (!Mod::get()->setSavedValue("has-loaded-mod-once", true)) {
        log::info("Loading the mod for the first time!");
        Mod::get()->setSavedValue("is-switcher-on", true);
    }

}

#include <Geode/modify/PauseLayer.hpp>
class $modify(MyPauseLayer, PauseLayer) {

    void customSetup() {
        
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) {
            log::warn("Failed to get right-button-menu.");
            return;
        }

        auto baseButtonSprite = CCSprite::create("checkpoint-switcher-button-base.png"_spr);
        auto buttonSprite = CircleButtonSprite::create(baseButtonSprite);
        buttonSprite->setScale(0.5f);
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MyPauseLayer::onCheckpointSwitcher));    
        menu->addChild(button);
        menu->updateLayout();

        button->setID("checkpoint-switcher-button"_spr);
    }

    void onCheckpointSwitcher(CCObject* sender) {
        CheckpointSwitcherLayer::create()->show();
    }
        
};