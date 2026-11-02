#include "cocos2d.h"
#include "CCSAXParser.h"
#include "support/tinyxml2/tinyxml2.h"
#include <sys/stat.h>
#include <vector>

NS_CC_BEGIN
class WebFileUtils : public CCFileUtils {
public:
    bool init() { m_strDefaultResRootPath = "/assets/"; return CCFileUtils::init(); }
    std::string getWritablePath() { return "/persist/"; }
    unsigned char* getFileData(const char* path, const char* mode, unsigned long* length) {
        unsigned char* original = CCFileUtils::getFileData(path, mode, length);
        if (!original) return 0;
        unsigned char* result = new unsigned char[*length + 1];
        memcpy(result, original, *length); result[*length] = 0; delete[] original;
        return result;
    }
    bool isFileExist(const std::string& path) {
        struct stat value;
        std::string full = isAbsolutePath(path) ? path : m_strDefaultResRootPath + path;
        return stat(full.c_str(), &value) == 0 && S_ISREG(value.st_mode);
    }
};
CCFileUtils* CCFileUtils::sharedFileUtils() {
    if (!s_sharedFileUtils) { s_sharedFileUtils = new WebFileUtils(); s_sharedFileUtils->init(); }
    return s_sharedFileUtils;
}
int CCTime::gettimeofdayCocos2d(cc_timeval* value, void*) {
    // Emscripten 3.1.74 has 64-bit time_t and 32-bit long. The native engine
    // casts these structs, which would overwrite the caller's stack on wasm32.
    if (!value) return -1;
    timeval now; int result = gettimeofday(&now, 0);
    value->tv_sec = static_cast<long>(now.tv_sec); value->tv_usec = static_cast<long>(now.tv_usec);
    return result;
}
double CCTime::timersubCocos2d(cc_timeval* start, cc_timeval* end) {
    if (!start || !end) return 0;
    return (end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0;
}

// TinyXML2 is already part of the engine. Traverse it as SAX events instead of
// adding a second XML parser or attempting to access native libxml2 libraries.
static void visitXML(tinyxml2::XMLNode* node, CCSAXParser* parser) {
    if (tinyxml2::XMLElement* element = node->ToElement()) {
        std::vector<const char*> attributes;
        for (const tinyxml2::XMLAttribute* a = element->FirstAttribute(); a; a = a->Next()) {
            attributes.push_back(a->Name()); attributes.push_back(a->Value());
        }
        attributes.push_back(0);
        CCSAXParser::startElement(parser, reinterpret_cast<const CC_XML_CHAR*>(element->Name()),
            reinterpret_cast<const CC_XML_CHAR**>(attributes.data()));
        for (tinyxml2::XMLNode* child = node->FirstChild(); child; child = child->NextSibling()) visitXML(child, parser);
        CCSAXParser::endElement(parser, reinterpret_cast<const CC_XML_CHAR*>(element->Name()));
    } else if (tinyxml2::XMLText* text = node->ToText()) {
        CCSAXParser::textHandler(parser, reinterpret_cast<const CC_XML_CHAR*>(text->Value()), strlen(text->Value()));
    }
}
CCSAXParser::CCSAXParser() : m_pDelegator(0) {}
CCSAXParser::~CCSAXParser() {}
bool CCSAXParser::init(const char*) { return true; }
void CCSAXParser::setDelegator(CCSAXDelegator* delegate) { m_pDelegator = delegate; }
bool CCSAXParser::parse(const char* data, unsigned int length) {
    tinyxml2::XMLDocument document;
    if (document.Parse(data, length) != tinyxml2::XML_NO_ERROR) { CCLog("[WebXML] Invalid XML resource"); return false; }
    if (document.RootElement()) visitXML(document.RootElement(), this);
    return document.RootElement() != 0;
}
bool CCSAXParser::parse(const char* path) {
    unsigned long size = 0;
    unsigned char* bytes = CCFileUtils::sharedFileUtils()->getFileData(path, "rb", &size);
    bool result = bytes && parse(reinterpret_cast<const char*>(bytes), size);
    delete[] bytes; return result;
}
void CCSAXParser::startElement(void* ctx, const CC_XML_CHAR* name, const CC_XML_CHAR** attrs) {
    CCSAXParser* parser = static_cast<CCSAXParser*>(ctx);
    if (parser->m_pDelegator) parser->m_pDelegator->startElement(ctx, reinterpret_cast<const char*>(name), reinterpret_cast<const char**>(attrs));
}
void CCSAXParser::endElement(void* ctx, const CC_XML_CHAR* name) {
    CCSAXParser* parser = static_cast<CCSAXParser*>(ctx);
    if (parser->m_pDelegator) parser->m_pDelegator->endElement(ctx, reinterpret_cast<const char*>(name));
}
void CCSAXParser::textHandler(void* ctx, const CC_XML_CHAR* text, int size) {
    CCSAXParser* parser = static_cast<CCSAXParser*>(ctx);
    if (parser->m_pDelegator) parser->m_pDelegator->textHandler(ctx, reinterpret_cast<const char*>(text), size);
}
NS_CC_END
