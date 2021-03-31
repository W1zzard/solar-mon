#define RXD2 16
#define TXD2 17

#include <ModbusMaster.h>
#include <WiFi.h>
#include <AverageValue.h>

#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

ModbusMaster node;
WebServer server(80);

String readString;

const int led = 2;
const char *ssid = "ssid"; //ssid of wifi spot to use
const char *password = "passhere"; //password of wifi spot

AverageValue tempInnerAvg, tempOuterAvg;

void setup() {
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    blinkLed(1);
    delay(1000);

    MDNS.begin("esp32");
    server.on("/metrics", handleMetrics);
    server.begin();
    blinkLed(2);
    delay(1000);

    // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    node.begin(1, Serial2);
    blinkLed(3);
}

void loop() {
    server.handleClient();
}


//read SP current in Amps * 0.01
double readSPCurrent() {
    return readDataOneReg(0x0108) * 0.01d;
}

// read  SP voltage Volts * 0.1 volts
double readSPVoltage() {
    return readDataOneReg(0x0107) * 0.1d;
}

// read  SP power watt
int readSPPower() {
    return readDataOneReg(0x0109);
}

//read battery charge in percent
int readBattCharge() {
    return readDataOneReg(0x0100);
}

//read batt charge current in Amps * 0.01
double readBattChargeCurrent() {
    return readDataOneReg(0x0102) * 0.01d;
}

// read  batt voltage at 0x0101, voltage = data * 0.1 volts
double readBattVoltage() {
    return readDataOneReg(0x0101) * 0.1d;
}

uint16_t readDataOneReg(uint16_t u16ReadAddress) {
    uint8_t result;
    uint16_t transferredData = 0;
    result = node.readHoldingRegisters(u16ReadAddress, 1);
    if (result == node.ku8MBSuccess) {
        transferredData = node.getResponseBuffer(0);
    }
    node.clearResponseBuffer();
    return transferredData;
}

//read one of temperature probably for internal sensor
int readTempIntoAvg() {
    uint8_t result;
    uint16_t internalTemp = 0;
    result = node.readHoldingRegisters(0x0103, 1);
    if (result == node.ku8MBSuccess) {
        uint16_t readValue = node.getResponseBuffer(0);
        //internal
        uint8_t absoluteValue = readValue >> 8 & 0x7f;
        if ((readValue >> 8 & 0x80) == 0) {
            internalTemp = internalTemp + absoluteValue;
        } else {
            internalTemp = internalTemp - absoluteValue;
        }
        tempInnerAvg.addValue(internalTemp);

        //external
        uint16_t externalTemp = 0;
        absoluteValue = readValue & 0x007f;
        if ((readValue & 0x0080) == 0) {
            externalTemp = externalTemp + absoluteValue;
        } else {
            externalTemp = externalTemp - absoluteValue;
        }
        tempOuterAvg.addValue(externalTemp);
    }
    node.clearResponseBuffer();
    return result;
}


void blinkLed(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(led, 1);
        delay(100);
        digitalWrite(led, 0);
        delay(100);
    }
}

void handleMetrics() {
    readTempIntoAvg();
    String response =
            createMetric("solarpanel_voltage", readSPVoltage())
            + createMetric("solarpanel_current", readSPCurrent())
            + createMetric("solarpanel_power", readSPPower())
            + createMetric("batterycharge_voltage", readBattVoltage())
            + createMetric("batterycharge_current", readBattChargeCurrent())
            + createMetric("temp_inner", tempInnerAvg.getValue())
            + createMetric("temp_external", tempOuterAvg.getValue());

    server.send(200, "text/plain", response);
}

String createMetric(String metricName, double value) {
    return "#HELP " + metricName + " metric " + metricName + "\n"
           + "#TYPE " + metricName + " gauge" + "\n"
           + metricName + "{instance=\"arduino\",job=\"solar\"} " + String(value, 2) + "\n";
}

