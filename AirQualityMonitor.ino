/*
  ==========================================================
  CODE CHO MÀN HÌNH OLED 1.3 INCH (SH1106)
  CĂN CHỈNH CHO 128X64 - CỠ CHỮ VỪA PHẢI
  ==========================================================
*/     

#include <Arduino.h>
#include "ACD1200.h"
#include "APM10.h"
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>

// ==========================================================
// CẤU HÌNH OLED 1.3 INCH - SH1106
// ==========================================================

U8G2_SH1106_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// SENSOR CONFIG
// ==========================================================

// ACD1200 via USART2
HardwareSerial SensorSerial(USART2);
ACD1200 co2Sensor(SensorSerial);

// APM10 via USART3
HardwareSerial APM10Serial(USART3);
APM10 pmSensor;

// DHT11
#define DHTPIN   PA4
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED
#define LED_PIN PC13

// BUZZER
#define BUZZER_PIN PA1
#define PM25_WARNING_THRESHOLD 75   // Ngưỡng cảnh báo PM2.5 (ug/m3)
#define PM25_DANGER_THRESHOLD 150   // Ngưỡng nguy hiểm

// Timing
unsigned long lastRead = 0;
unsigned long lastOLEDUpdate = 0;
unsigned long startTime = 0;
unsigned long splashStartTime = 0;
bool splashDisplayed = false;

// Buzzer variables
unsigned long lastBuzzerToggle = 0;
unsigned long buzzerStartTime = 0;
bool buzzerActive = false;
int buzzerCycle = 0;  // 0: tắt, 1: đang kêu, 2: nghỉ

// Data structure for PM
APM10::pmData pmData;

// Lưu trữ dữ liệu
float lastTemperature = 0;
float lastHumidity = 0;
int lastCO2 = 0;
int lastPM25 = 0;
int lastPM10 = 0;
int lastAQI25 = 0;
int lastAQI10 = 0;
bool lastCO2OK = false;
bool lastDhtOK = false;
bool lastPmOK = false;

// ==========================================================
// KHỞI TẠO OLED SH1106
// ==========================================================
void initOLED() {
    Wire.setSCL(PB6);
    Wire.setSDA(PB7);
    Wire.begin();
    
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    
    // Xóa màn hình
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    delay(100);
}

// ==========================================================
// MÀN HÌNH KHỞI ĐỘNG (GRADUATION PROJECT)
// ==========================================================
void showSplashScreen() {
    u8g2.clearBuffer();
    
    // Vẽ khung viền
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.drawFrame(2, 2, 124, 60);
    
    // Dòng 1: GRADUATION
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(128/2 - u8g2.getStrWidth("GRADUATION")/2, 15);
    u8g2.print("GRADUATION");
    
    // Dòng 2: PROJECT
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(128/2 - u8g2.getStrWidth("PROJECT")/2, 32);
    u8g2.print("PROJECT");
    
    // Dòng 3: AIR QUALITY MONITOR
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(128/2 - u8g2.getStrWidth("Air Quality Monitor")/2, 50);
    u8g2.print("Air Quality Monitor");
    
    u8g2.sendBuffer();
}

// ==========================================================
// HÀM CHUYỂN ĐỔI THỜI GIAN (GIÂY -> PHÚT/GIỜ)
// ==========================================================
void formatUptime(unsigned long seconds, char* buffer) {
    unsigned long hours = seconds / 3600;
    unsigned long minutes = (seconds % 3600) / 60;
    
    if (hours > 0) {
        sprintf(buffer, "UPTIME: %luH %luM", hours, minutes);
    } else {
        sprintf(buffer, "UPTIME: %luM", minutes);
    }
}

// ==========================================================
// HÀM ĐIỀU KHIỂN CÒI THEO PM2.5
// ==========================================================
void handleBuzzer(int pm25_value) {
    // Kiểm tra ngưỡng PM2.5
    if (pm25_value >= PM25_WARNING_THRESHOLD) {
        // Nếu chưa active buzzer thì bắt đầu
        if (!buzzerActive) {
            buzzerActive = true;
            buzzerStartTime = millis();
            buzzerCycle = 1;
            lastBuzzerToggle = millis();
            digitalWrite(BUZZER_PIN, HIGH);
            Serial.println("PM2.5 HIGH! Buzzer ON");
        }
        
        // Đang trong chu kỳ kêu 3 giây
        if (buzzerCycle == 1 && (millis() - buzzerStartTime) < 3000) {
            // Tạo hiệu ứng tit tít tít (mỗi 200ms)
            if (millis() - lastBuzzerToggle > 200) {
                lastBuzzerToggle = millis();
                digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
            }
        }
        // Kết thúc 3 giây
        else if (buzzerCycle == 1 && (millis() - buzzerStartTime) >= 3000) {
            digitalWrite(BUZZER_PIN, LOW);
            buzzerCycle = 2;
            buzzerStartTime = millis();
        }
        // Nghỉ 3 giây trước khi lặp lại
        else if (buzzerCycle == 2 && (millis() - buzzerStartTime) >= 3000) {
            buzzerCycle = 1;
            buzzerStartTime = millis();
            lastBuzzerToggle = millis();
            digitalWrite(BUZZER_PIN, HIGH);
        }
    } else {
        // PM2.5 bình thường, tắt còi
        if (buzzerActive) {
            buzzerActive = false;
            buzzerCycle = 0;
            digitalWrite(BUZZER_PIN, LOW);
            Serial.println("PM2.5 Normal. Buzzer OFF");
        }
    }
}

// ==========================================================
// HÀM HIỂN THỊ OLED CHO SH1106
// ==========================================================
void displayOLED() {
    u8g2.clearBuffer();

    int x = 13;
    int y = 8;
    int lineHeight = 11;

    char buffer[32];

    u8g2.setFont(u8g2_font_5x7_tf);

    // DÒNG 1 : CO2
    if (lastCO2OK) {
        sprintf(buffer, "CO2:%4d", lastCO2);
        u8g2.drawStr(x, y, buffer);
        
        u8g2.setFont(u8g2_font_4x6_tf);
        u8g2.drawStr(55, y+1, "ppm");
        u8g2.setFont(u8g2_font_5x7_tf);

        if (lastCO2 > 1000) {
            u8g2.setDrawColor(0);
            u8g2.drawBox(82, y - 3, 35, 7);
            u8g2.setDrawColor(1);
            u8g2.drawStr(84, y, "HIGH");
        }
    } else {
        u8g2.drawStr(x, y, "CO2 ERROR");
    }

    y += lineHeight;

    // DÒNG 2 : PM2.5 + THANH BAR (CÙNG HÀNG Y)
    if (lastPmOK) {
        sprintf(buffer, "PM2.5:%3d", lastPM25);
        u8g2.drawStr(x, y, buffer);

        // Thanh bar có tọa độ Y bằng với dòng chữ PM2.5
        int barLen = map(constrain(lastPM25, 0, 150), 0, 150, 0, 40);
        
        u8g2.drawFrame(70, y, 42, 7);   // y-3 để canh giữa với chữ
        u8g2.drawBox(71, y+1, barLen, 5);  // y-2 để canh giữa với chữ
        
        // Cảnh báo nếu PM2.5 cao
        if (lastPM25 >= PM25_WARNING_THRESHOLD) {
            u8g2.setFont(u8g2_font_4x6_tf);
            u8g2.drawStr(70, y-20, "WARNING");
            u8g2.setFont(u8g2_font_5x7_tf);
        }
    } else {
        u8g2.drawStr(x, y, "PM ERROR");
    }

    y += lineHeight;

    // DÒNG 3 : AQI + TRẠNG THÁI
    if (lastPmOK) {
        sprintf(buffer, "AQI:%3d", lastAQI25);
        u8g2.drawStr(x, y, buffer);

        const char* quality;
        if (lastAQI25 <= 50) {
            quality = "GOOD";
        }
        else if (lastAQI25 <= 100) {
            quality = "MODERATE";
        }
        else if (lastAQI25 <= 150) {
            quality = "UNHEALTHY";
        }
        else {
            quality = "DANGER";
        }
        u8g2.drawStr(55, y, quality);
    } else {
        u8g2.drawStr(x, y, "AQI ERROR");
    }

    y += lineHeight;

    // DÒNG 4 : NHIỆT ĐỘ + ĐỘ ẨM
    if (lastDhtOK) {
        char tempBuffer[8];
        char humBuffer[8];
        
        dtostrf(lastTemperature, 4, 1, tempBuffer);
        dtostrf(lastHumidity, 4, 0, humBuffer);
        
        sprintf(buffer, "T:  %s%C", tempBuffer, 248);
        u8g2.drawStr(x, y, buffer);

        sprintf(buffer, "H:%s%%", humBuffer);
        u8g2.drawStr(70, y, buffer);
    } else {
        u8g2.drawStr(x, y, "DHT11 ERROR");
    }

    y += lineHeight;

    // DÒNG 5 : THỜI GIAN
    unsigned long uptimeSeconds = (millis() - startTime) / 1000;
    formatUptime(uptimeSeconds, buffer);
    u8g2.drawStr(x, y, buffer);

    u8g2.sendBuffer();
}

// APM10 is now read using the pmSensor.collectData() method from the APM10 library.

// ==========================================================
void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    
    // Khởi tạo buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.begin(115200);
    
    startTime = millis();

    initOLED();
    Serial.println("OLED SH1106 Initialized (1.3 inch)");

    SensorSerial.begin(1200);
    co2Sensor.begin(1200);
    Serial.println("ACD1200 Initialized on USART2 (PA2, PA3)");

    Serial.println("Initializing APM10...");
    APM10Serial.begin(1200);
    pmSensor.beginUART(APM10Serial);
    delay(500);
    Serial.println("APM10 Initialized on USART3 (PB10, PB11)");

    dht.begin();
    Serial.println("DHT11 Initialized on PA4");
    
    delay(500);

    Serial.println("==================================================");
    Serial.println("ACD1200 + DHT11 + APM10 + OLED SH1106 (1.3 inch)");
    Serial.println("BUZZER on PA1 - Warning at PM2.5 >= 75");
    Serial.println("==================================================");

    // Hiển thị màn hình khởi động
    showSplashScreen();
    splashStartTime = millis();
    splashDisplayed = true;
    
    delay(100);
    Serial.println("Sensors Ready!");
}

// ==========================================================
void loop() {
    static unsigned long lastBlink = 0;
    
    // Kiểm tra màn hình khởi động (hiển thị trong 3 giây)
    if (splashDisplayed) {
        if (millis() - splashStartTime < 3000) {
            // Vẫn đang hiển thị màn hình khởi động, không làm gì thêm
            return;
        } else {
            // Kết thúc màn hình khởi động
            splashDisplayed = false;
            // Xóa màn hình để chuẩn bị hiển thị dữ liệu
            u8g2.clearBuffer();
            u8g2.sendBuffer();
            lastOLEDUpdate = millis() - 1000; // Force update ngay lập tức
        }
    }

    if (millis() - lastBlink > 500) {
        lastBlink = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    if (millis() - lastRead > 2000) {
        lastRead = millis();

        // READ CO2
        lastCO2OK = co2Sensor.readCO2();
        if (lastCO2OK) {
            lastCO2 = co2Sensor.getCO2();
            Serial.print("CO2: "); Serial.print(lastCO2); Serial.println(" ppm");
        } else {
            Serial.println("CO2 Read Error");
        }

        // READ DHT11
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
        
        if (!isnan(humidity) && !isnan(temperature) && humidity > 0 && humidity <= 100) {
            lastTemperature = temperature;
            lastHumidity = humidity;
            lastDhtOK = true;
            
            Serial.print("DHT11 OK - Temp: ");
            Serial.print(temperature);
            Serial.print("C, Hum: ");
            Serial.print(humidity);
            Serial.println("%");
        } else {
            lastDhtOK = false;
            Serial.println("DHT11 Read Error");
        }
        
        // READ APM10
        lastPmOK = pmSensor.collectData(pmData);
        if (lastPmOK) {
            lastPM25 = pmData._PM2_5;
            lastPM10 = pmData._PM10;
            lastAQI25 = pmSensor.PM2_5_TO_AQI(lastPM25);
            lastAQI10 = pmSensor.PM10_TO_AQI(lastPM10);
            Serial.print("PM2.5: "); Serial.print(lastPM25); Serial.println(" ug/m3");
            Serial.print("AQI: "); Serial.println(lastAQI25);
        } else {
            Serial.println("APM10 Read Error");
        }
        
        // Điều khiển còi dựa trên PM2.5
        if (lastPmOK) {
            handleBuzzer(lastPM25);
        }

        // STATUS LED
        if (lastCO2OK || lastDhtOK || lastPmOK) {
            digitalWrite(LED_PIN, LOW);
            delay(30);
            digitalWrite(LED_PIN, HIGH);
        }

        Serial.println("================================");
        Serial.println("================================");
    }
    
    // Update OLED every 1 second (chỉ khi đã qua màn hình khởi động)
    if (!splashDisplayed && (millis() - lastOLEDUpdate > 1000)) {
        lastOLEDUpdate = millis();
        displayOLED();
    }
}
