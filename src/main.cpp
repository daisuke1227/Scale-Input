#include <Geode/Geode.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/binding/EditorUI.hpp>

using namespace geode::prelude;

// so im using something thats "out of line" on windows (valueFromScale), and idk if that means itll work or not, but if someone complains of an error or something because of it ill just add something to check for windows and disable it ig 
class $modify (ScaleControl, GJScaleControl) {

    struct Fields {
        CCLabelBMFont* customScaleDefaultLabel;
        CCLabelBMFont* customScaleXLabel;
        CCLabelBMFont* customScaleYLabel;
        TextInput* scaleDefaultInput;
        TextInput* scaleXInput;
        TextInput* scaleYInput;
        CCMenu* defaultShortcutsMenu;
        CCMenu* xShortcutsMenu;
        CCMenu* yShortcutsMenu;
        float shortcutAlignment;
        CCNode* clipboardX;
        CCNode* clipboardY;
        CCMenu* siderbarMenu;
    };

    bool init() {
        if (!GJScaleControl::init()) return false;

        if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
            // YOU CAN USE CCP FOR CCPOINT?????
            auto scaleDefaultInput = TextInput::create(50, "1.00");
            scaleDefaultInput->setPosition(ccp(35, 28.5));
            scaleDefaultInput->setScale(0.55);
            scaleDefaultInput->setFilter("1234567890.-");
            scaleDefaultInput->setMaxCharCount(Mod::get()->getSettingValue<int64_t>("scale-max-characters"));
            scaleDefaultInput->setID("scale-default-input");
            scaleDefaultInput->setCallback([this](const std::string& textInput) {
                if (textInput.find_first_of("0123456789") != std::string::npos) {
                    if (auto editorUI = EditorUI::get()) {
                        
                        float input = std::stof(textInput);
                        float scale = std::max(m_valueX, m_valueY);
                        if (input == 0) input = 1;
                        if (scale == 0) scale = 1;

                        editorUI->scaleXYChanged(input * (m_valueX / scale), input * (m_valueY / scale), m_scaleLocked);

                        this->updateLabelXY(input);
                        m_sliderXY->setValue(valueFromScale(input));
                        m_valueX = input * (m_valueX / scale);
                        m_valueY = input * (m_valueY / scale);
                    }
                }
            });

            auto scaleXInput = TextInput::create(50, "1.00");
            scaleXInput->setPosition(ccp(42, 28.5));
            scaleXInput->setScale(0.55);
            scaleXInput->setFilter("1234567890.-");
            scaleXInput->setMaxCharCount(Mod::get()->getSettingValue<int64_t>("scale-max-characters"));
            scaleXInput->setID("scale-default-input");
            scaleXInput->setCallback([this](const std::string& input) {
                if (input.find_first_of("0123456789") != std::string::npos) {
                    if (auto editorUI = EditorUI::get()) {
                        editorUI->scaleXChanged(std::stof(input), m_scaleLocked);

                        this->updateLabelX(std::stof(input));
                        m_sliderX->setValue(valueFromScale(std::stof(input)));
                        m_valueX = std::stof(input);
                    }
                }
            });

            auto scaleYInput = TextInput::create(50, "1.00");
            scaleYInput->setPosition(ccp(42, 88.5));
            scaleYInput->setScale(0.55);
            scaleYInput->setFilter("1234567890.-");
            scaleYInput->setMaxCharCount(Mod::get()->getSettingValue<int64_t>("scale-max-characters"));
            scaleYInput->setID("scale-default-input");
            scaleYInput->setCallback([this](const std::string& input) {
                if (input.find_first_of("0123456789") != std::string::npos) {
                    if (auto editorUI = EditorUI::get()) {
                        editorUI->scaleYChanged(std::stof(input), m_scaleLocked);

                        this->updateLabelY(std::stof(input));
                        m_sliderY->setValue(valueFromScale(std::stof(input)));
                        m_valueY = std::stof(input);
                    }
                }
            });

            // remove old beta labels (not like fully removing them cuz i have a feeling thatll crash the game)
            m_scaleLabel->setOpacity(0);
            m_scaleXLabel->setOpacity(0);
            m_scaleYLabel->setOpacity(0);

            // add new sigma labels
            auto scaleDefaultLabel = CCLabelBMFont::create("Scale: ", "bigFont.fnt");
            scaleDefaultLabel->setScale(0.6);
            scaleDefaultLabel->setAnchorPoint(ccp(0, 0.5));
            scaleDefaultLabel->setPosition(ccp(-49, 30));
            scaleDefaultLabel->setID("scale-default-label");

            auto scaleXLabel = CCLabelBMFont::create("ScaleX: ", "bigFont.fnt");
            scaleXLabel->setScale(0.6);
            scaleXLabel->setAnchorPoint(ccp(0, 0.5));
            scaleXLabel->setPosition(ccp(-56, 30));
            scaleXLabel->setID("scale-x-label");
            
            auto scaleYLabel = CCLabelBMFont::create("ScaleY: ", "bigFont.fnt");
            scaleYLabel->setScale(0.6);
            scaleYLabel->setAnchorPoint(ccp(0, 0.5));
            scaleYLabel->setPosition(ccp(-56, 90));
            scaleYLabel->setID("scale-y-label");
            
            auto menu = CCMenu::create();
            menu->setPosition(0, 0);
            menu->setID("scale-label-menu");
            this->addChild(menu);

            menu->addChild(scaleDefaultInput);
            menu->addChild(scaleXInput);
            menu->addChild(scaleYInput);
            menu->addChild(scaleDefaultLabel);
            menu->addChild(scaleXLabel);
            menu->addChild(scaleYLabel);

            m_fields->customScaleDefaultLabel = scaleDefaultLabel;
            m_fields->customScaleXLabel = scaleXLabel;
            m_fields->customScaleYLabel = scaleYLabel;

            m_fields->scaleDefaultInput = scaleDefaultInput;
            m_fields->scaleXInput = scaleXInput;
            m_fields->scaleYInput = scaleYInput;
        }
        
        if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled")) {
            int shortcuts = Mod::get()->getSettingValue<int64_t>("shortcut-amount");
            bool xyShortcuts = Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled");

            auto setupShortcutMenu = [&] (std::string id, CCMenu* menu) {
                menu->setLayout(RowLayout::create()
                ->setGap(3.f)
                ->setGrowCrossAxis(false)
                ->setAxisAlignment(AxisAlignment::Start));
                menu->setContentSize(CCSize(248.75, 49));
                menu->setScale(0.4);
                menu->setID(id);
                this->addChild(menu);
            };

            auto defaultShortcutsMenu = CCMenu::create();
            setupShortcutMenu("default-shortcuts-menu", defaultShortcutsMenu);
            m_fields->defaultShortcutsMenu = defaultShortcutsMenu;

            if (xyShortcuts) {
                auto xShortcutsMenu = CCMenu::create();
                setupShortcutMenu("x-shortcuts-menu", xShortcutsMenu);
                m_fields->xShortcutsMenu = xShortcutsMenu;

                auto yShortcutsMenu = CCMenu::create();
                setupShortcutMenu("y-shortcuts-menu", yShortcutsMenu);
                m_fields->yShortcutsMenu = yShortcutsMenu;
            }

            auto makeShortcut = [&] (float value, int buttonNumber, CCMenu* menu, int scaleType) {
                std::stringstream stringStream;
                stringStream << std::fixed << std::setprecision(2) << value;
                std::string string = stringStream.str();

                string.erase(string.find_last_not_of('0') + 1, std::string::npos);
                if (string.back() == '.') string.pop_back();
                if (!(string.find_first_of("0123456789") != std::string::npos)) string = 1;
                
                auto callback = static_cast<SEL_MenuHandler>(&ScaleControl::scaleFromShortcutXY);
                if (scaleType == 1) callback = static_cast<SEL_MenuHandler>(&ScaleControl::scaleFromShortcutX);
                if (scaleType == 2) callback = static_cast<SEL_MenuHandler>(&ScaleControl::scaleFromShortcutY);

                auto shortcut = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::create(
                CCLabelBMFont::create(string.c_str(), "bigFont.fnt")), this, callback);
                shortcut->setID(fmt::format("shortcut-{}", buttonNumber));

                auto valueNode = CCNode::create();
                valueNode->setID(string);
                shortcut->addChild(valueNode);

                menu->addChild(shortcut);
                menu->updateLayout();
            };

            if (shortcuts >= 1) {
                makeShortcut(Mod::get()->getSettingValue<double>("shortcut-one"), 1, m_fields->defaultShortcutsMenu, 0);
                if (xyShortcuts) {
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-one"), 1, m_fields->xShortcutsMenu, 1);
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-one"), 1, m_fields->yShortcutsMenu, 2);
                }
                m_fields->shortcutAlignment = 40.2;
            }
            if (shortcuts >= 2) {
                makeShortcut(Mod::get()->getSettingValue<double>("shortcut-two"), 2, defaultShortcutsMenu, 0);
                if (xyShortcuts) {
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-two"), 2, m_fields->xShortcutsMenu, 1);
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-two"), 2, m_fields->yShortcutsMenu, 2);
                }
                m_fields->shortcutAlignment = 30.35;
            }
            if (shortcuts >= 3) {
                makeShortcut(Mod::get()->getSettingValue<double>("shortcut-three"), 3, defaultShortcutsMenu, 0);
                if (xyShortcuts) {
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-three"), 3, m_fields->xShortcutsMenu, 1);
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-three"), 3, m_fields->yShortcutsMenu, 2);
                }
                m_fields->shortcutAlignment = 20.2;
                
            }
            if (shortcuts >= 4) {
                makeShortcut(Mod::get()->getSettingValue<double>("shortcut-four"), 4, defaultShortcutsMenu, 0);
                if (xyShortcuts) {
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-four"), 4, m_fields->xShortcutsMenu, 1);
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-four"), 4, m_fields->yShortcutsMenu, 2);
                }
                m_fields->shortcutAlignment = 10.35;
            }
            if (shortcuts >= 5) {
                makeShortcut(Mod::get()->getSettingValue<double>("shortcut-five"), 5, defaultShortcutsMenu, 0);
                if (xyShortcuts) {
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-five"), 5, m_fields->xShortcutsMenu, 1);
                    makeShortcut(Mod::get()->getSettingValue<double>("shortcut-five"), 5, m_fields->yShortcutsMenu, 2);
                }
                m_fields->shortcutAlignment = 0;
            }
        }
        
        if (Mod::get()->getSettingValue<bool>("siderbar-menu-enabled") || (Mod::get()->getSettingValue<bool>("scale-copy-paste-menu") && Mod::get()->getSettingValue<bool>("scale-copy-paste-enabled"))) {
            auto menu = CCMenu::create();
            menu->setLayout(ColumnLayout::create()
            ->setGap(3.f)
            ->setGrowCrossAxis(false)
            ->setAxisAlignment(AxisAlignment::Start));
            menu->setContentSize(CCSize(46.75, 153));
            if (Mod::get()->getSettingValue<bool>("siderbar-position")) {
                menu->setPosition(ccp(135, 30));
            }
            else {
                menu->setPosition(ccp(-135, 30));
            }
            menu->setScale(0.525);
            menu->setID("scale-sidebar-menu");
            this->addChild(menu);
            m_fields->siderbarMenu = menu;
        }
        
        if (Mod::get()->getSettingValue<bool>("siderbar-menu-enabled") || Mod::get()->getSettingValue<bool>("better-lock-enabled")) {
            m_scaleLockButton->setOpacity(0);
            m_scaleLockButton->setEnabled(false);

            
            CCNode* offSprite = CCLabelBMFont::create("OFF", "bigFont.fnt");
            CCNode* onSprite = CCLabelBMFont::create("ON", "bigFont.fnt");
            if (Mod::get()->getSettingValue<bool>("lock-use-sprites")) offSprite = CCSprite::create("unlocked.png"_spr);
            if (Mod::get()->getSettingValue<bool>("lock-use-sprites")) onSprite = CCSprite::create("locked.png"_spr);

            auto toggleButton = CCMenuItemToggler::create(
            CircleButtonSprite::create(offSprite), CircleButtonSprite::create(onSprite, geode::CircleBaseColor::Blue), 
            this, menu_selector(GJScaleControl::onToggleLockScale));
            toggleButton->setID("better-lock-button");
            
            if (Mod::get()->getSettingValue<bool>("siderbar-menu-enabled")) {
                m_fields->siderbarMenu->addChild(toggleButton);
                m_fields->siderbarMenu->updateLayout();
            }
            else {
                m_scaleLockButton->getParent()->addChild(toggleButton);
                toggleButton->setScale(0.525);
            }
        }

        if (Mod::get()->getSettingValue<bool>("scale-copy-paste-enabled")) {
            auto clipboard = CCNode::create();
            auto clipboardX = CCNode::create();
            auto clipboardY = CCNode::create();
            m_fields->clipboardX = clipboardX;
            m_fields->clipboardY = clipboardY;
            clipboardX->setID("1");
            clipboardY->setID("1");
            clipboard->setID("clipboard");
            clipboard->addChild(clipboardX);
            clipboard->addChild(clipboardY);
            this->addChild(clipboard);

            CCNode* copySprite = CCLabelBMFont::create("Copy", "bigFont.fnt");
            CCNode* pasteSprite = CCLabelBMFont::create("Paste", "bigFont.fnt");
            if (Mod::get()->getSettingValue<bool>("copy-paste-use-sprites")) copySprite = CCSprite::create("copy.png"_spr);
            if (Mod::get()->getSettingValue<bool>("copy-paste-use-sprites")) pasteSprite = CCSprite::create("paste.png"_spr);
            geode::CircleBaseColor copyColor = geode::CircleBaseColor::Pink;
            geode::CircleBaseColor pasteColor = geode::CircleBaseColor::Cyan;
            if (Mod::get()->getSettingValue<bool>("copy-paste-swap-colors")) copyColor = geode::CircleBaseColor::Cyan;
            if (Mod::get()->getSettingValue<bool>("copy-paste-swap-colors")) pasteColor = geode::CircleBaseColor::Pink;
            
            auto copyButton = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(copySprite, copyColor),
            this, menu_selector(ScaleControl::scaleCopy));
            copyButton->setID("scale-copy-button");

            auto pasteButton = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(pasteSprite, pasteColor),
            this, menu_selector(ScaleControl::scalePaste));
            pasteButton->setID("scale-paste-button");
            
            if (Mod::get()->getSettingValue<bool>("scale-copy-paste-menu")) {
                if (!Mod::get()->getSettingValue<bool>("copy-paste-swap-position")) {
                    m_fields->siderbarMenu->addChild(copyButton);
                    m_fields->siderbarMenu->addChild(pasteButton);
                }
                else {
                    m_fields->siderbarMenu->addChild(pasteButton);
                    m_fields->siderbarMenu->addChild(copyButton);
                }
                m_fields->siderbarMenu->updateLayout();
            }
            else {
                m_scaleLockButton->getParent()->addChild(copyButton);
                m_scaleLockButton->getParent()->addChild(pasteButton);
                copyButton->setScale(0.525);
                pasteButton->setScale(0.525);
                copyButton->m_baseScale = 0.525;
                pasteButton->m_baseScale = 0.525;
                if (!Mod::get()->getSettingValue<bool>("siderbar-menu-enabled")) {
                    copyButton->setPosition(ccp(-27.54, 0));
                    pasteButton->setPosition(ccp(27.54, 0));
                }
                else {
                    copyButton->setPosition(ccp(-13.77, 0));
                    pasteButton->setPosition(ccp(13.77, 0));
                }
            }
        }

        return true;
    }

    void scaleFromShortcutXY(CCObject* sender) {
        if (EditorUI::get() && static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1)) {
          
            float input = std::stof(static_cast<CCNode*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1))->getID());
            
            float scale = std::max(m_valueX, m_valueY);
            if (input == 0) input = 1;
            if (scale == 0) scale = 1;
            
            EditorUI::get()->scaleXYChanged(input * (m_valueX / scale), input * (m_valueY / scale), m_scaleLocked);
            this->updateInputValues(false, true, input * (m_valueX / scale), input * (m_valueY / scale), m_sliderXY);
            this->updateLabelXY(input);
            if (!Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled") && m_sliderY->isVisible()) {
                this->updateLabelX(input * (m_valueX / scale));
                this->updateLabelY(input * (m_valueY / scale));
            }
            m_sliderXY->setValue(valueFromScale(input));
            m_valueX = input * (m_valueX / scale);
            m_valueY = input * (m_valueY / scale);
        }
    }

    void scaleFromShortcutX(CCObject* sender) {
        if (EditorUI::get() && static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1)) {
          
            float input = std::stof(static_cast<CCNode*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1))->getID());
     
            if (input == 0) input = 1;
            
            EditorUI::get()->scaleXChanged(input, m_scaleLocked);
            this->updateInputValues(false, true, input, m_valueY, m_sliderX);
            this->updateLabelX(input);
            m_sliderX->setValue(valueFromScale(input));
            m_valueX = input;
        }
    }

    void scaleFromShortcutY(CCObject* sender) {
        if (EditorUI::get() && static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1)) {
          
            float input = std::stof(static_cast<CCNode*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(1))->getID());
     
            if (input == 0) input = 1;
            
            EditorUI::get()->scaleYChanged(input, m_scaleLocked);
            this->updateInputValues(false, true, m_valueX, input, m_sliderY);
            this->updateLabelY(input);
            m_sliderY->setValue(valueFromScale(input));
            m_valueY = input;
        }
    }

    void scaleCopy(CCObject* sender) {
        // this could probably be done in a cleaner way but i have no idea how string stream actually works so-
        std::stringstream xStream;
        std::stringstream yStream;
        std::string x;
        std::string y;
        
        xStream << std::fixed << std::setprecision(2) << m_valueX;
        x = xStream.str();
        yStream << std::fixed << std::setprecision(2) << m_valueY;
        y = yStream.str();
        
        x.erase(x.find_last_not_of('0') + 1, std::string::npos);
        if (x.back() == '.') x.pop_back();
        y.erase(y.find_last_not_of('0') + 1, std::string::npos);
        if (y.back() == '.') y.pop_back();

        m_fields->clipboardX->setID(x);
        m_fields->clipboardY->setID(y);
    }

    void scalePaste(CCObject* sender) {
        float x = std::stof(m_fields->clipboardX->getID());
        float y = std::stof(m_fields->clipboardY->getID());

        EditorUI::get()->scaleXChanged(x, m_scaleLocked);
        EditorUI::get()->scaleYChanged(y, m_scaleLocked);
        this->updateInputValues(false, true, x, y, nullptr);
        this->updateLabelX(x);
        this->updateLabelX(y);
        this->updateLabelXY(std::max(x, y));
        m_sliderX->setValue(valueFromScale(x));
        m_sliderY->setValue(valueFromScale(y));
        m_sliderXY->setValue(valueFromScale(std::max(x, y)));
        m_valueX = x;
        m_valueY = y;
    }

    void checkVisibility() {
        if (m_scaleLabel->isVisible()) {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleDefaultLabel->setVisible(true);
                m_fields->scaleDefaultInput->setVisible(true);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled")) {
                m_scaleLockButton->getParent()->setPosition(ccp(0, 90));
                if (!Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                    m_fields->defaultShortcutsMenu->setPosition(ccp(m_fields->shortcutAlignment, 60));
                }
                else {
                    m_fields->defaultShortcutsMenu->setPosition(ccp(m_fields->shortcutAlignment, 60)); // too lazy to make positions not repeat every time this is called, it shouldnt cause problems so its fine i think
                    m_fields->defaultShortcutsMenu->setVisible(true);
                }
            }
        }
        else {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleDefaultLabel->setVisible(false);
                m_fields->scaleDefaultInput->setVisible(false);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled")) {
                if (!Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                    m_fields->defaultShortcutsMenu->setPosition(ccp(m_fields->shortcutAlignment, 120));
                    m_scaleLockButton->getParent()->setPosition(ccp(0, 150));
                }
                else {
                    m_fields->defaultShortcutsMenu->setVisible(false);
                }
            }
        }
        
        if (m_scaleXLabel->isVisible()) {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleXLabel->setVisible(true);
                m_fields->scaleXInput->setVisible(true);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled") && Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                m_fields->xShortcutsMenu->setPosition(ccp(m_fields->shortcutAlignment, 60));
                m_fields->xShortcutsMenu->setVisible(true);
            }
        }
        else {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleXLabel->setVisible(false);
                m_fields->scaleXInput->setVisible(false);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled") && Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                m_fields->xShortcutsMenu->setVisible(false);
            }
        }

        if (m_scaleYLabel->isVisible()) {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleYLabel->setVisible(true);
                m_fields->scaleYInput->setVisible(true);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled") && Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                m_scaleLockButton->getParent()->setPosition(ccp(0, 180));
                m_sliderY->setPosition(ccp(0, 90));
                m_fields->customScaleYLabel->setPosition(ccp(-56, 120));
                m_fields->scaleYInput->setPosition(ccp(42, 118.5));
                m_fields->yShortcutsMenu->setPosition(ccp(m_fields->shortcutAlignment, 150));
                m_fields->yShortcutsMenu->setVisible(true);
            }
        }
        else {
            if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
                m_fields->customScaleYLabel->setVisible(false);
                m_fields->scaleYInput->setVisible(false);
            }
            if (Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled") && Mod::get()->getSettingValue<bool>("x-y-shortcuts-enabled")) {
                m_fields->yShortcutsMenu->setVisible(false);
            }
        }
    }
    
    void updateInputValues(bool live, bool force, float forceX, float forceY, CCObject* slider) {
        if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
            // was using std::format but ig old macs dont work like that so chatgpt told me to use stringstream so ig it works now

            if (slider == m_sliderXY && m_fields->scaleDefaultInput) {
                std::string scale;
                if (!force) {
                    std::stringstream stringStream;   
                    if (live) {
                        stringStream << std::fixed << std::setprecision(2) << std::max(m_changedValueX, m_changedValueY);
                    }
                    else {
                        stringStream << std::fixed << std::setprecision(2) << std::max(m_valueX, m_valueY);
                    }
                    scale = stringStream.str();
                }
                else {
                    std::stringstream stringStream;
                    stringStream << std::fixed << std::setprecision(2) << std::max(forceX, forceY);
                    scale = stringStream.str();
                }
                scale.erase(scale.find_last_not_of('0') + 1, std::string::npos);
                if (scale.back() == '.') scale.pop_back();

                m_fields->scaleDefaultInput->setString(scale, false);
            }
            else if (slider == m_sliderX && m_fields->scaleXInput) {
                std::string scale;
                if (!force) {
                    std::stringstream stringStream;   
                    if (live) {
                        stringStream << std::fixed << std::setprecision(2) << m_changedValueX;
                    }
                    else {
                        stringStream << std::fixed << std::setprecision(2) << m_valueX;
                    }
                    scale = stringStream.str();
                }
                else {
                    std::stringstream stringStream;
                    stringStream << std::fixed << std::setprecision(2) << forceX;
                    scale = stringStream.str();
                }
                scale.erase(scale.find_last_not_of('0') + 1, std::string::npos);
                if (scale.back() == '.') scale.pop_back();

                m_fields->scaleXInput->setString(scale, false);
            }
            else if (slider == m_sliderY && m_fields->scaleYInput) {
                std::string scale;
                if (!force) {
                    std::stringstream stringStream;   
                    if (live) {
                        stringStream << std::fixed << std::setprecision(2) << m_changedValueY;
                    }
                    else {
                        stringStream << std::fixed << std::setprecision(2) << m_valueY;
                    }
                    scale = stringStream.str();
                }
                else {
                    std::stringstream stringStream;
                    stringStream << std::fixed << std::setprecision(2) << forceY;
                    scale = stringStream.str();
                }
                scale.erase(scale.find_last_not_of('0') + 1, std::string::npos);
                if (scale.back() == '.') scale.pop_back();

                m_fields->scaleYInput->setString(scale, false);
            }
            else if (m_fields->scaleDefaultInput && m_fields->scaleXInput && m_fields->scaleYInput) {
                this->updateInputValues(live, force, forceX, forceY, m_sliderXY);
                this->updateInputValues(live, force, forceX, forceY, m_sliderX);
                this->updateInputValues(live, force, forceX, forceY, m_sliderY);
            }
        }
    }   

    void sliderChanged(CCObject* sender) {
        GJScaleControl::sliderChanged(sender);
        if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
            this->updateInputValues(true, false, 0, 0, static_cast<CCNode*>(sender)->getParent()->getParent());
        }
    }
    
    void loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states) {
        GJScaleControl::loadValues(obj, objs, states);
        if (Mod::get()->getSettingValue<bool>("scale-input-enabled")) {
            this->updateInputValues(false, false, 0, 0, nullptr);
        }
        if (Mod::get()->getSettingValue<bool>("scale-input-enabled") || Mod::get()->getSettingValue<bool>("scale-shortcuts-enabled")) {
            this->checkVisibility();
        }
    }
   
    // i really wanna add a "change scale and position multiplier" thing to the scale gui but idk how to only run something with the scale controls are opened (well i do but it isnt found yet for mac)
    // i remember reading a disscusion saying that buttons dont really need to be found because you already have their addresses with devtools or smth but idk how to use those
    // well i have the code saved and pretty much done so if the binding gets found ill add the feature
}; 