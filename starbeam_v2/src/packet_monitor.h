// packet_monitor.h - Passive WiFi Packet Monitor for Project Starbeam V2

#ifndef PACKET_MONITOR_H
#define PACKET_MONITOR_H

#include <Arduino.h>
#include <esp_wifi.h>

class PacketMonitor {
public:
    static void init();
    static void runMonitor();
    static void stop();
    static void setChannel(uint8_t channel);
    static void showStatus();

    static int getAverageRssi()        { return averageRssi; }
    static uint32_t getPacketCount()   { return packetCount; }
    static uint32_t getMgmtCount()     { return managementCount; }
    static uint32_t getDataCount()     { return dataCount; }
    static uint32_t getCtrlCount()     { return controlCount; }
    static uint8_t getCurrentChannel() { return currentChannel; }
    static bool isRunning()            { return running; }

private:
    static void IRAM_ATTR onPacket(void* buf, wifi_promiscuous_pkt_type_t type);
    static void drawStatus();

    static bool running;
    static uint8_t currentChannel;
    static volatile uint32_t packetCount;
    static volatile uint32_t managementCount;
    static volatile uint32_t dataCount;
    static volatile uint32_t controlCount;
    static volatile int rssiSum;
    static volatile uint32_t rssiSamples;
    static int averageRssi;
};

#endif // PACKET_MONITOR_H
