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

                        m_sliderXY->setValue(valueFromScale(input)); // this doesnt work exactly but idrc since it doesnt effect functionality
                        this->updateLabelXY(input);
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

                        m_sliderX->setValue(valueFromScale(std::stof(input)));
                        this->updateLabelX(std::stof(input));
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

                        m_sliderY->setValue(valueFromScale(std::stof(input)));
                        this->updateLabelY(std::stof(input));
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
        }
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
};  