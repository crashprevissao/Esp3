// captive_portal.h - Captive Portal Module for Project Starbeam V2

#ifndef CAPTIVE_PORTAL_H
#define CAPTIVE_PORTAL_H

#include <Arduino.h>

#define CAPTIVE_PORTAL_MAX_CAPTURES  20
#define CAPTIVE_PORTAL_CAPTURE_LEN   200
#define CAPTIVE_PORTAL_TEMPLATE_COUNT 10

class CaptivePortal {
public:
    static void init();
    static void deinit();
    static void start(int templateIndex = 0);
    static void stop();
    static void handleClient();

    static bool isRunning()           { return running; }
    static int getCaptureCount()      { return captureCount; }
    static int getConnectedClients();
    static String getActiveSSID();
    static int getTemplateCount()     { return CAPTIVE_PORTAL_TEMPLATE_COUNT; }
    static const char* getTemplateName(int index);
    static void setTemplate(int index);

    static String getCapture(int index);
    static void clearCaptures();
    static void printCapturesToSerial();

    // Public for lambda access
    static int activeTemplate;
    static int captureCount;
    static int captureHead;
    struct CaptureEntry {
        unsigned long timestamp;
        char data[CAPTIVE_PORTAL_CAPTURE_LEN];
    };
    static CaptureEntry captures[CAPTIVE_PORTAL_MAX_CAPTURES];
    static String urlDecode(const String& str);
    static void addCapture(const String& templateName, const String& data);

private:
    static void setupRoutes();

    static bool running;
};

#endif // CAPTIVE_PORTAL_H
