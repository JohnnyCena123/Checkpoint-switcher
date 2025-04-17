#include <Geode/Geode.hpp>

#include "alphalaneous.pages_api/include/PageMenu.h"

#include "classes.hpp"

using namespace geode::prelude;




#include <Geode/modify/CheckpointObject.hpp>
class $modify(MyCheckpointObject, CheckpointObject) {

	struct Fields {
		float m_currentPrecentage = 0.f;
		Ref<CCTexture2D> m_screenshot;
	};

	// bool init() {
	// 	if (!CheckpointObject::init()) return false;
	
	
	// 	return true;
	// }

};

#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {

	struct Fields {
		bool m_isPracticeMode;

		bool m_hasCheckpointChanged;
		bool m_newCheckpointPlaced;

		CheckpointObject* m_selectedCheckpoint;
	};

	// bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
	//     if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

	//     return true;
	// } 

	void setCheckpoint(CheckpointObject* checkpoint) {
		m_currentCheckpoint = checkpoint;
		m_fields->m_selectedCheckpoint = checkpoint;
		m_fields->m_hasCheckpointChanged = true;
		m_fields->m_newCheckpointPlaced = false;
	}

	void loadFromCheckpoint(CheckpointObject* checkpoint) {

		auto selectedCheckpoint = m_fields->m_selectedCheckpoint;
		if (!m_fields->m_newCheckpointPlaced && selectedCheckpoint && Mod::get()->getSavedValue<bool>("is-switcher-on")) {
			PlayLayer::loadFromCheckpoint(selectedCheckpoint);
	} else {
			PlayLayer::loadFromCheckpoint(checkpoint);
	}
	}

	void resume() {
		PlayLayer::resume();
		if (!Mod::get()->getSavedValue<bool>("is-switcher-on")) return;


		auto selectedCheckpoint = m_fields->m_selectedCheckpoint;
		if (m_fields->m_hasCheckpointChanged) {
			if (selectedCheckpoint) {
				m_checkpointArray->removeObject(selectedCheckpoint);
				storeCheckpoint(selectedCheckpoint);
				m_currentCheckpoint = selectedCheckpoint;
			}
			resetLevel();

			m_fields->m_hasCheckpointChanged = false;
		}
	}

	bool getIsPracticeMode() {
		return m_fields->m_isPracticeMode;

	}

	CCArray* getCheckpoints() {
		return m_checkpointArray;
	}

	CheckpointObject* createCheckpoint() {
		auto ret = PlayLayer::createCheckpoint();
		if (ret) static_cast<MyCheckpointObject*>(ret)->m_fields->m_currentPrecentage = this->getCurrentPercent();

		m_fields->m_newCheckpointPlaced = true;
		
		if (Mod::get()->getSettingValue<bool>("enable-previews")) {
			auto winSize = CCDirector::get()->getWinSize();
			auto renderTexture = CCRenderTexture::create(winSize.width, winSize.height);
			renderTexture->begin();
			CCScene::get()->visit();
			renderTexture->end();

			static_cast<MyCheckpointObject*>(ret)->m_fields->m_screenshot = renderTexture->getSprite()->getTexture();
			auto spr = CCSprite::createWithTexture(renderTexture->getSprite()->getTexture());
			if (!spr) log::error("placed checkpoint, but failed to render screenshot.");
			spr->setScale(.5f);
			ret->addChild(spr);
		}                           

		return ret;
	}

	void removeCheckpoint(bool p0) {

		CheckpointObject* removedCheckpoint;
		if (p0) removedCheckpoint = m_currentCheckpoint;
		else removedCheckpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0));

		if (removedCheckpoint == m_currentCheckpoint) {
			m_currentCheckpoint = m_checkpointArray->count() ? 
			static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(m_checkpointArray->count() - 1)) : 
			nullptr;
			m_fields->m_selectedCheckpoint = nullptr;
		}
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


bool CheckpointSwitcherLayer::setup() {
	bool hasFailed = false;

	s_currentLayer = this;

	this->setID("CheckpointSwitcherLayer"_spr);

	this->setTitle("Choose A Checkpoint");

	m_selectedButton = nullptr;
	
	m_toggleSwitcherButtonSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
	m_toggleSwitcherButtonCheckmarkSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
	m_toggleSwitcherButtonCheckmarkSprite->setVisible(Mod::get()->getSavedValue<bool>("is-switcher-on"));
	m_toggleSwitcherButtonSprite->addChildAtPosition(m_toggleSwitcherButtonCheckmarkSprite, Anchor::Center);
	m_toggleSwitcherButton = CCMenuItemSpriteExtra::create(
		m_toggleSwitcherButtonSprite, this, menu_selector(CheckpointSwitcherLayer::onToggleSwitcher)
	);
	m_toggleSwitcherButton->ignoreAnchorPointForPosition(true);
	m_buttonMenu->addChildAtPosition(m_toggleSwitcherButton, Anchor::BottomLeft, {10.f, 10.f});
	
	m_toggleSwitcherButtonLabel = CCLabelBMFont::create("Enable the switcher!", "bigFont.fnt");
	m_toggleSwitcherButtonLabel->setScale(0.33333f);
	m_mainLayer->addChildAtPosition(m_toggleSwitcherButtonLabel, Anchor::BottomLeft, {110.f, 25.f});

	m_applyButtonEnabledSprite = ButtonSprite::create("Apply");
	m_applyButtonDisabledSprite = ButtonSprite::create("Apply");
	m_applyButtonDisabledSprite->setOpacity(155);
	m_applyButtonDisabledSprite->setColor(ccGRAY);
	m_applyButton = CCMenuItemSpriteExtra::create(
		m_applyButtonEnabledSprite, this, 
		menu_selector(CheckpointSwitcherLayer::onApply)
	);
	m_applyButton->setEnabled(false);
	m_applyButton->setOpacity(30);
	m_buttonMenu->addChildAtPosition(m_applyButton, Anchor::Bottom, {0, m_applyButton->getContentHeight() / 2.f + 10.f});

	m_checkpointSelectorMenu = CCMenu::create();
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
		auto practiceOffLabelTop = CCLabelBMFont::create("Enable practice mode", "bigFont.fnt");
		auto practiceOffLabelBottom = CCLabelBMFont::create("to use the mod!", "bigFont.fnt");

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
		m_progressBarCloneFill = CCSprite::create("sliderBar2.png");
		m_progressBarCloneFill->setColor(ccc3(125, 255, 0));
		m_progressBarCloneFill->setContentSize(progressBarFilling->getContentSize());
		m_progressBarCloneFill->setAnchorPoint({0.f, 0.f});
		m_progressBarCloneFill->setZOrder(-1);
		m_progressBarCloneFill->setTextureRect({
			0, 0,
			(progressBar->getTextureRect().getMaxX() - 5) * PlayLayer::get()->getCurrentPercent() / 100.f,
			progressBar->getTextureRect().getMaxY() / 2
		});
		ccTexParams texParams = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
		m_progressBarCloneFill->getTexture()->setTexParameters(&texParams);
		m_progressBarClone->addChildAtPosition(m_progressBarCloneFill, Anchor::BottomLeft, {2.f, 4.f});
		m_mainLayer->addChildAtPosition(m_progressBarClone, Anchor::Center, progressBar->getPosition() - CCDirector::get()->getWinSize() / 2);
	
		m_progressBarClone->setID("progress-bar-clone");
		m_progressBarCloneFill->setID("progress-bar-clone-filling");
	
		m_checkpointIndicatorsNode = CCNode::create();
		m_checkpointIndicatorsNode->setAnchorPoint({0.5f, 0.5f});
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
			checkpointIndicatorSprite->setAnchorPoint({0.5f, 0.f});
			checkpointIndicatorLine->setAnchorPoint({0.5f, 0.f});
			checkpointIndicatorSprite->setScale(0.5f);
			checkpointIndicatorSprite->addChildAtPosition(checkpointIndicatorLine, Anchor::Top, {0.f, 2.f});
			m_checkpointIndicatorsNode->addChildAtPosition(checkpointIndicatorSprite, Anchor::BottomLeft, {
				progressBar->getContentWidth() * checkpoint->m_fields->m_currentPrecentage / 100.f + 2.f,
				- checkpointIndicatorLine->getContentHeight() - 6.f 
			});

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
	log::debug("the switcher was {} and is now {}.", isSwitcherOn ? "on" : "disabled", isSwitcherOn ? "disabled" : "on");
	Mod::get()->setSavedValue("is-switcher-on", !isSwitcherOn);
	m_toggleSwitcherButtonCheckmarkSprite->setVisible(!isSwitcherOn);
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

	m_buttonID = buttonID;
	m_checkpoint = checkpoint;

	if(!m_checkpoint->m_fields->m_screenshot.data()) log::error("failed to save screenshot.");

	m_buttonSprite = Mod::get()->getSettingValue<bool>("enable-previews") ? 
	CCSprite::createWithSpriteFrameName("checkpoint_01_001.png") : 
	CCSprite::createWithTexture(m_checkpoint->m_fields->m_screenshot.data());
	if (!m_buttonSprite) {
		log::error("button sprite failed to initialize.");
		hasFailed = true;
	}


	m_buttonSprite->setScale(80 / m_buttonSprite->getContentHeight());
	m_mainNode = CCNode::create();
	m_mainNode->setContentSize(m_buttonSprite->getScaledContentSize());
	m_mainNode->addChildAtPosition(m_buttonSprite, Anchor::Center);
	
	if (!CCMenuItemSpriteExtra::init(m_mainNode, m_mainNode, this, menu_selector(CheckpointSelectorButton::onSelectButton))) {
		log::error("CCMenuItemSpriteExtra failed to initialize."); 
		hasFailed = true;
	}

	if (Mod::get()->getSettingValue<bool>("enable-previews")) {
		m_buttonOutline->setScaleX((m_buttonSprite->getContentWidth() + 5.f) / m_buttonOutline->getContentWidth());
		m_buttonOutline->setScaleY((m_buttonSprite->getContentHeight() + 5.f) / m_buttonOutline->getContentHeight());
		m_buttonOutline->setZOrder(-1);
		m_checkpointGlowOutline = CCSprite::createWithSpriteFrameName("square_01_glow_001.png");
		m_checkpointGlowOutline->setZOrder(-1);
	} else {
		m_buttonOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_color_001.png");
		m_checkpointGlowOutline = CCSprite::createWithSpriteFrameName("checkpoint_01_glow_001.png");
		m_buttonOutline = CCSprite::createWithSpriteFrameName("d_whiteBlock_01_001.png");
	}

	m_buttonOutline->setColor(ccc3(255, 243, 69));
	m_checkpointGlowOutline->setColor(ccc3(255, 253, 137));

	m_buttonSprite->addChildAtPosition(m_buttonOutline, Anchor::Center);
	m_buttonOutline->addChildAtPosition(m_checkpointGlowOutline, Anchor::Center);

	m_buttonLabel = CCLabelBMFont::create(
		m_checkpoint ? 
		fmt::format("Checkpoint at {:.4}%", m_checkpoint->m_fields->m_currentPrecentage).c_str() : 
		"The start!", "bigFont.fnt"
	);
	m_mainNode->addChildAtPosition(m_buttonLabel, Anchor::Top, {0.f, 10.f});
	m_buttonLabel->setScale(.4f);

	m_mainNode->setID("main-node");
	m_buttonSprite->setID("checkpoint-sprite");
	m_buttonLabel->setID("checkpoint-label");
	m_buttonOutline->setID("checkpoint-outline");
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
		m_mainNode->runAction(CCEaseInOut::create(CCScaleTo::create(.1f, toScaleUp ? 1.25f : 1.f), 2.f));
		m_isScaledUp = toScaleUp;
	}
}

void CheckpointSelectorButton::setOutlineVisible(bool isVisible) {
	m_buttonOutline->setVisible(isVisible);
}

CCSprite* CheckpointSelectorButton::getSprite() {
	return m_buttonSprite;
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
