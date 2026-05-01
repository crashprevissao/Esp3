// packet_monitor.cpp - Passive WiFi Packet Monitor for Project Starbeam V2
// Ported from cypherbox/src/packet_monitor.cpp (SD/PCAP recording removed)

#include "packet_monitor.h"
#include "display.h"
#include "input.h"
#include "terminal.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include "../config.h"

// Max frame size to process in ISR (standard 802.11 MPDU limit)
#define SNAP_LEN 2346

bool PacketMonitor::running = false;
uint8_t PacketMonitor::currentChannel = 1;
volatile uint32_t PacketMonitor::packetCount = 0;
volatile uint32_t PacketMonitor::managementCount = 0;
volatile uint32_t PacketMonitor::dataCount = 0;
volatile uint32_t PacketMonitor::controlCount = 0;
volatile int PacketMonitor::rssiSum = 0;
volatile uint32_t PacketMonitor::rssiSamples = 0;
int PacketMonitor::averageRssi = 0;

void PacketMonitor::init() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    esp_wifi_set_promiscuous(false);
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous_rx_cb(&PacketMonitor::onPacket);

    packetCount = 0;
    managementCount = 0;
    dataCount = 0;
    controlCount = 0;
    rssiSum = 0;
    rssiSamples = 0;

    running = true;
    esp_wifi_set_promiscuous(true);
    Serial.println("Packet monitor started");
}

void PacketMonitor::setChannel(uint8_t channel) {
    if (channel < 1 || channel > 13) channel = 1;
    currentChannel = channel;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    Serial.printf("Packet monitor channel: %u\n", currentChannel);
}

void IRAM_ATTR PacketMonitor::onPacket(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    if (!pkt || type == WIFI_PKT_MISC) return;
    if (pkt->rx_ctrl.sig_len > SNAP_LEN) return;

    packetCount++;
    if (type == WIFI_PKT_MGMT)      managementCount++;
    else if (type == WIFI_PKT_DATA) dataCount++;
    else if (type == WIFI_PKT_CTRL) controlCount++;

    rssiSum += pkt->rx_ctrl.rssi;
    rssiSamples++;
}

void PacketMonitor::drawStatus() {
    if (rssiSamples > 0) averageRssi = rssiSum / (int)rssiSamples;
    Display::displayInfo(
        "Pkt Monitor CH" + String(currentChannel),
        "RSSI " + String(averageRssi) + " dBm",
        "Pkt:" + String(packetCount) + " M:" + String(managementCount) +
            " D:" + String(dataCount),
        "[UP/DN]=CH  [SEL]=Exit"
    );
}

void PacketMonitor::runMonitor() {
    init();
    drawStatus();
    delay(250);

    unsigned long lastDraw = millis();

    while (true) {
        if (Input::isButtonPressed(BUTTON_UP)) {
            setChannel(currentChannel == 1 ? 13 : currentChannel - 1);
            delay(180);
        }
        if (Input::isButtonPressed(BUTTON_DOWN)) {
            setChannel(currentChannel == 13 ? 1 : currentChannel + 1);
            delay(180);
        }
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            delay(200);
            stop();
            return;
        }

        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            stop();
            return;
        }

        if (millis() - lastDraw > 750) {
            drawStatus();
            Serial.printf("ch:%u pkt:%lu mgmt:%lu data:%lu ctrl:%lu rssi:%d\n",
                          currentChannel, packetCount, managementCount,
                          dataCount, controlCount, averageRssi);
            lastDraw = millis();
        }

        yield();
    }
}

void PacketMonitor::stop() {
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    running = false;
    Serial.println("Packet monitor stopped");
}

void PacketMonitor::showStatus() {
    if (rssiSamples > 0) averageRssi = rssiSum / (int)rssiSamples;
    Serial.printf("Packet monitor: ch=%u pkt=%lu mgmt=%lu data=%lu ctrl=%lu rssi=%d\n",
                  currentChannel, packetCount, managementCount, dataCount,
                  controlCount, averageRssi);
    Display::displayInfo(
        "Packet Monitor",
        "CH " + String(currentChannel) + " RSSI " + String(averageRssi),
        "Packets " + String(packetCount),
        "Mgmt " + String(managementCount)
    );
    delay(1600);
}
