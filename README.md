# Chipxa-AirQualityMonitor

[Tiếng Việt bên dưới]

A professional Air Quality Monitoring system using an OLED display, CO2 sensor, particulate matter (PM) sensor, and temperature/humidity sensor. Developed and maintained by **Chipxa Technology Co., Ltd. (Công ty TNHH Công nghệ Chipxa)**.

Dự án giám sát chất lượng không khí chuyên nghiệp sử dụng màn hình hiển thị OLED, cảm biến đo nồng độ khí CO2, cảm biến bụi mịn PM và cảm biến nhiệt độ/độ ẩm. Được phát triển và bảo trì bởi **Công ty TNHH Công nghệ Chipxa**.

---

## 🏢 Contact & Developer Information / Thông tin nhà phát triển
* **Company / Công ty**: Công ty TNHH Công nghệ Chipxa (Chipxa Technology Co., Ltd.)
* **Website**: [https://chipxa.com](https://chipxa.com)
* **Email**: contact@chipxa.com
* **GitHub**: [https://github.com/chipxatech](https://github.com/chipxatech)

---

## 🔌 Hardware Setup / Cấu hình phần cứng
- **Microcontroller**: STM32 / ESP32 / Arduino (Default configuration uses STM32 pinouts).
- **OLED Display**: 1.3-inch (SH1106 driver, 128x64, I2C: SCL -> PB6, SDA -> PB7).
- **CO2 Sensor (ACD1200)**: Connected via `USART2` (TX/RX -> PA2/PA3).
- **PM Sensor (APM10)**: Connected via `USART3` (TX/RX -> PB10/PB11) or I2C.
- **Temp & Humidity Sensor (DHT11)**: Connected to pin `PA4`.
- **Alert Indicator**:
  - Warning LED: Pin `PC13`
  - Warning Buzzer: Pin `PA1`

---

## 📚 Libraries Overview & Usage Guide / Tổng quan và Hướng dẫn sử dụng thư viện

This repository contains two custom libraries developed by **Chipxa Technology**:
Kho lưu trữ này chứa hai thư viện tùy chỉnh được phát triển bởi **Chipxa**:
1. **ACD1200** - For the ACD1200 CO2 sensor. (Cho cảm biến đo khí CO2 ACD1200).
2. **APM10** - For the APM10 PM1.0/PM2.5/PM10 Particulate Matter sensor. (Cho cảm biến bụi mịn APM10).

---

### 1. ACD1200 CO2 Sensor Library

#### 🇬🇧 English Guide

The `ACD1200` library supports reading CO2 levels via any hardware/software serial interface (`Stream` object). It includes moving average filtering to smooth out readings.

##### **API Reference:**
* `ACD1200(Stream &serialPort)`: Constructor. Pass the serial stream (e.g. `Serial2`).
* `void begin(uint32_t baud = 1200)`: Dummy init method. Remember to call `SerialX.begin(1200)` in your main sketch.
* `bool readCO2()`: Sends request command and parses the returned packet. Returns `true` if read succeeds.
* `uint16_t getCO2()`: Returns the latest raw CO2 concentration in ppm.
* `uint16_t getAverageCO2()`: Returns the moving average CO2 concentration (size of 5 samples).
* `uint8_t* getRawData()`: Returns the raw 9-byte packet received from the sensor.
* `void printRaw(Stream &out)`: Prints the raw packet bytes to the specified stream for debugging.

##### **Quick Code Example:**
```cpp
#include <Arduino.h>
#include <ACD1200.h>

HardwareSerial SensorSerial(USART2); // STM32 USART2
ACD1200 co2Sensor(SensorSerial);

void setup() {
    Serial.begin(115200);
    SensorSerial.begin(1200);
    co2Sensor.begin();
}

void loop() {
    if (co2Sensor.readCO2()) {
        Serial.print("Latest CO2: ");
        Serial.print(co2Sensor.getCO2());
        Serial.println(" ppm");
        
        Serial.print("Avg CO2: ");
        Serial.print(co2Sensor.getAverageCO2());
        Serial.println(" ppm");
    } else {
        Serial.println("Failed to read from ACD1200");
    }
    delay(2000);
}
```

---

#### 🇻🇳 Hướng dẫn bằng Tiếng Việt

Thư viện `ACD1200` hỗ trợ đọc nồng độ CO2 thông qua cổng nối tiếp nối tiếp bất kỳ (đối tượng kế thừa từ lớp `Stream` như `HardwareSerial` hoặc `SoftwareSerial`). Thư viện tích hợp bộ lọc trung bình động (moving average) để giúp dữ liệu đo được ổn định hơn.

##### **Các hàm chính trong thư viện:**
* `ACD1200(Stream &serialPort)`: Hàm khởi tạo. Truyền vào cổng Serial (ví dụ `Serial2`).
* `void begin(uint32_t baud = 1200)`: Hàm khởi tạo bổ trợ. Cần chạy `SerialPort.begin(1200)` trong hàm `setup()`.
* `bool readCO2()`: Gửi lệnh yêu cầu đo và giải mã gói tin trả về từ cảm biến. Trả về `true` nếu thành công.
* `uint16_t getCO2()`: Trả về nồng độ CO2 đo được mới nhất (đơn vị ppm).
* `uint16_t getAverageCO2()`: Trả về nồng độ CO2 trung bình động (tính toán từ 5 mẫu liên tiếp gần nhất).
* `uint8_t* getRawData()`: Lấy mảng byte dữ liệu thô (9 bytes) nhận về từ cảm biến.
* `void printRaw(Stream &out)`: In dữ liệu thô ra màn hình Serial Monitor phục vụ mục đích kiểm tra lỗi (debug).

##### **Ví dụ code nhanh:**
```cpp
#include <Arduino.h>
#include <ACD1200.h>

HardwareSerial SensorSerial(USART2); // Khởi tạo cổng USART2 cho STM32
ACD1200 co2Sensor(SensorSerial);

void setup() {
    Serial.begin(115200);
    SensorSerial.begin(1200); // Tốc độ baudrate của ACD1200 là 1200
    co2Sensor.begin();
}

void loop() {
    if (co2Sensor.readCO2()) {
        Serial.print("CO2 hien tai: ");
        Serial.print(co2Sensor.getCO2());
        Serial.println(" ppm");
        
        Serial.print("CO2 trung binh: ");
        Serial.print(co2Sensor.getAverageCO2());
        Serial.println(" ppm");
    } else {
        Serial.println("Loi ket noi hoac doc cam bien ACD1200");
    }
    delay(2000);
}
```

---
---

### 2. APM10 PM1.0/PM2.5/PM10 Sensor Library

#### 🇬🇧 English Guide

The `APM10` library supports communicating with the APM10 dust sensor via both **I2C** and **UART (Serial)** modes. It also includes utility functions to calculate the US EPA Air Quality Index (AQI) for PM2.5 and PM10.

##### **API Reference:**
* `bool beginI2C(uint8_t i2caddr = APM10_ADDRESS, TwoWire &wirePort = Wire)`: Start communication using I2C mode.
* `bool beginUART(Stream &serPort)`: Start communication using UART mode.
* `bool isConnected()`: Verifies if the sensor is reachable on the bus/serial port.
* `void startMeasurement()`: Commands the sensor to begin active measurement.
* `bool collectData(pmData& data)`: Reads the latest values into a `pmData` struct.
  - Struct `pmData` contains:
    - `data._PM1_0`: PM1.0 concentration ($\mu g/m^3$)
    - `data._PM2_5`: PM2.5 concentration ($\mu g/m^3$)
    - `data._PM10`: PM10 concentration ($\mu g/m^3$)
* `void stopMeasurement()`: Puts the sensor into low-power sleep mode.
* `int PM2_5_TO_AQI(int PM)`: Utility to convert PM2.5 concentration ($\mu g/m^3$) to Air Quality Index (AQI).
* `int PM10_TO_AQI(int PM)`: Utility to convert PM10 concentration ($\mu g/m^3$) to Air Quality Index (AQI).

##### **Quick Code Example (UART Mode):**
```cpp
#include <Arduino.h>
#include <APM10.h>

HardwareSerial APM10Serial(USART3); // STM32 USART3
APM10 pmSensor;
APM10::pmData pmData;

void setup() {
    Serial.begin(115200);
    APM10Serial.begin(1200);
    
    if (pmSensor.beginUART(APM10Serial)) {
        Serial.println("APM10 Connected!");
    } else {
        Serial.println("APM10 Connection Failed!");
    }
}

void loop() {
    if (pmSensor.collectData(pmData)) {
        Serial.print("PM1.0: "); Serial.print(pmData._PM1_0); Serial.println(" ug/m3");
        Serial.print("PM2.5: "); Serial.print(pmData._PM2_5); Serial.println(" ug/m3");
        Serial.print("PM10: ");  Serial.print(pmData._PM10);  Serial.println(" ug/m3");
        
        int aqi = pmSensor.PM2_5_TO_AQI(pmData._PM2_5);
        Serial.print("PM2.5 AQI: "); Serial.println(aqi);
    } else {
        Serial.println("Failed to read from APM10");
    }
    delay(2000);
}
```

---

#### 🇻🇳 Hướng dẫn bằng Tiếng Việt

Thư viện `APM10` hỗ trợ giao tiếp với cảm biến bụi mịn APM10 qua cả 2 giao thức phổ biến là **I2C** và **UART (Serial)**. Ngoài ra, thư viện tích hợp sẵn hàm tính toán chỉ số chất lượng không khí (AQI) chuẩn EPA Hoa Kỳ cho hạt bụi PM2.5 và PM10.

##### **Các hàm chính trong thư viện:**
* `bool beginI2C(uint8_t i2caddr = APM10_ADDRESS, TwoWire &wirePort = Wire)`: Khởi động giao tiếp bằng I2C.
* `bool beginUART(Stream &serPort)`: Khởi động giao tiếp bằng UART (Serial).
* `bool isConnected()`: Kiểm tra xem cảm biến đã kết nối thành công hay chưa.
* `void startMeasurement()`: Gửi lệnh yêu cầu cảm biến bắt đầu quá trình đo đạc.
* `bool collectData(pmData& data)`: Nhận gói dữ liệu và lưu vào biến cấu trúc `pmData`.
  - Cấu trúc `pmData` gồm:
    - `data._PM1_0`: Nồng độ bụi PM1.0 ($\mu g/m^3$)
    - `data._PM2_5`: Nồng độ bụi PM2.5 ($\mu g/m^3$)
    - `data._PM10`: Nồng độ bụi PM10 ($\mu g/m^3$)
* `void stopMeasurement()`: Đưa cảm biến vào chế độ ngủ tiết kiệm điện.
* `int PM2_5_TO_AQI(int PM)`: Chuyển đổi nồng độ bụi PM2.5 ($\mu g/m^3$) sang chỉ số chất lượng không khí AQI tương ứng.
* `int PM10_TO_AQI(int PM)`: Chuyển đổi nồng độ bụi PM10 ($\mu g/m^3$) sang chỉ số chất lượng không khí AQI tương ứng.

##### **Ví dụ code nhanh (Chế độ UART):**
```cpp
#include <Arduino.h>
#include <APM10.h>

HardwareSerial APM10Serial(USART3); // Khởi tạo cổng USART3 cho STM32
APM10 pmSensor;
APM10::pmData pmData;

void setup() {
    Serial.begin(115200);
    APM10Serial.begin(1200); // Cảm biến mặc định giao tiếp ở baudrate 1200
    
    if (pmSensor.beginUART(APM10Serial)) {
        Serial.println("APM10 ket noi thanh cong!");
    } else {
        Serial.println("Khong tim thay cam bien APM10!");
    }
}

void loop() {
    if (pmSensor.collectData(pmData)) {
        Serial.print("PM1.0: "); Serial.print(pmData._PM1_0); Serial.println(" ug/m3");
        Serial.print("PM2.5: "); Serial.print(pmData._PM2_5); Serial.println(" ug/m3");
        Serial.print("PM10: ");  Serial.print(pmData._PM10);  Serial.println(" ug/m3");
        
        int aqi = pmSensor.PM2_5_TO_AQI(pmData._PM2_5);
        Serial.print("Chi so AQI (PM2.5): "); Serial.println(aqi);
    } else {
        Serial.println("Loi doc du lieu tu APM10");
    }
    delay(2000);
}
```

---

## 🛠️ Main System Integration / Chương trình tích hợp hệ thống chính
For the complete implementation, view the main Arduino sketch [AirQualityMonitor.ino](file:///c:/Users/Admin/Desktop/AirQualityMonitor/AirQualityMonitor.ino). It coordinates both libraries, handles OLED layout drawing, DHT11 sampling, and trigger conditions for the active warning buzzer and status LED.

Để xem mã nguồn tích hợp hệ thống hoàn chỉnh, vui lòng truy cập tệp [AirQualityMonitor.ino](file:///c:/Users/Admin/Desktop/AirQualityMonitor/AirQualityMonitor.ino). Chương trình điều phối hoạt động của hai cảm biến trên cùng với cảm biến DHT11, vẽ giao diện trên màn hình OLED và kiểm soát các điều kiện cảnh báo bật còi báo động (buzzer) và nháy LED cảnh báo.
