#include "GUI/CCEditBox/CCEditBoxImpl.h"
#include "cocos2d.h"
#include <emscripten.h>
#include <map>

USING_NS_CC;
USING_NS_CC_EXT;
class WebEditBox;
static std::map<int, WebEditBox*> editors;
static int nextEditor = 1;

class WebEditBox : public CCEditBoxImpl {
    int id, maximum, fontSize;
    bool editing, password;
    EditBoxInputMode mode;
    std::string text, placeholder, font;
    CCSize size;
    ccColor3B color, placeholderColor;
    CCLabelTTF* label;
    void draw() {
        if (!label) return;
        std::string visible = text.empty() ? placeholder : text;
        if (password && !text.empty()) visible.assign(cc_utf8_strlen(text.c_str(), -1), '*');
        label->setString(visible.c_str());
        label->setColor(text.empty() ? placeholderColor : color);
        label->setFontName(font.c_str()); label->setFontSize(fontSize);
        label->setDimensions(CCSize(std::max(1.f, size.width - 8), size.height));
        label->setPosition(ccp(4, size.height / 2));
    }
public:
    WebEditBox(CCEditBox* box) : CCEditBoxImpl(box), id(nextEditor++), maximum(-1), fontSize(20), editing(false), password(false),
        mode(kEditBoxInputModeSingleLine), font("sans-serif"), color(ccWHITE), placeholderColor(ccGRAY), label(0) { editors[id] = this; }
    ~WebEditBox() {
        editors.erase(id);
        EM_ASM({ if (window.NRB.editing === $0) window.NRB.closeInput(); }, id);
    }
    bool initWithSize(const CCSize& value) {
        size = value; label = CCLabelTTF::create("", font.c_str(), fontSize);
        label->setAnchorPoint(ccp(0, 0.5)); label->setHorizontalAlignment(kCCTextAlignmentLeft);
        m_pEditBox->addChild(label, 10); draw(); return true;
    }
    void setFont(const char* name, int pixels) { font = name ? name : "sans-serif"; fontSize = pixels; draw(); }
    void setFontColor(const ccColor3B& value) { color = value; draw(); }
    void setPlaceholderFont(const char*, int) {}
    void setPlaceholderFontColor(const ccColor3B& value) { placeholderColor = value; draw(); }
    void setInputMode(EditBoxInputMode value) { mode = value; }
    void setInputFlag(EditBoxInputFlag value) { password = value == kEditBoxInputFlagPassword; draw(); }
    void setMaxLength(int value) { maximum = value; }
    int getMaxLength() { return maximum; }
    void setReturnType(KeyboardReturnType) {}
    bool isEditing() { return editing; }
    void setText(const char* value) { text = value ? value : ""; draw(); }
    const char* getText() { return text.c_str(); }
    void setPlaceHolder(const char* value) { placeholder = value ? value : ""; draw(); }
    void doAnimationWhenKeyboardMove(float, float) {}
    void openKeyboard() {
        editing = true;
        if (m_pDelegate) m_pDelegate->editBoxEditingDidBegin(m_pEditBox);
        EM_ASM({ window.NRB.edit(Module, $0, UTF8ToString($1), UTF8ToString($2), $3, $4, $5); },
            id, text.c_str(), placeholder.c_str(), maximum, password, mode);
    }
    void closeKeyboard() { editing = false; EM_ASM({ window.NRB.closeInput(); }); }
    void setPosition(const CCPoint&) {} // Label inherits the edit box world transform.
    void setVisible(bool value) { if (label) label->setVisible(value); if (!value && editing) closeKeyboard(); }
    void setContentSize(const CCSize& value) { size = value; draw(); }
    void setAnchorPoint(const CCPoint&) {}
    void visit() {}
    void onEnter() { draw(); }
    void changed(const char* value, bool finished) {
        setText(value);
        if (!m_pDelegate) { editing = !finished; return; }
        m_pEditBox->retain(); // A delegate may close and destroy the containing page.
        CCEditBox* box = m_pEditBox;
        m_pDelegate->editBoxTextChanged(box, text);
        if (finished && editors.count(id)) {
            editing = false;
            m_pDelegate->editBoxEditingDidEnd(box);
            if (editors.count(id) && m_pDelegate) m_pDelegate->editBoxReturn(box);
        }
        box->release();
    }
};
namespace cocos2d { namespace extension {
CCEditBoxImpl* __createSystemEditBox(CCEditBox* box) { return new WebEditBox(box); }
}}
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_edit(int id, const char* value, int finished) {
    if (editors.count(id)) editors[id]->changed(value, finished != 0);
}
