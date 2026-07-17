# Chipxa-AirQualityMonitor

Dự án giám sát chất lượng không khí sử dụng vi điều khiển và các cảm biến đo nồng độ bụi mịn, CO2, nhiệt độ, độ ẩm và hiển thị thông tin lên màn hình OLED.

## Thành phần phần cứng (Hardware Components)

- **Vi điều khiển**: Tương thích Arduino (STM32, ESP32 hoặc Arduino Board tùy thuộc sơ đồ cấu hình chân).
- **Màn hình hiển thị**: OLED 1.3 inch (SH1106, giao tiếp I2C 128x64).
- **Cảm biến CO2**: ACD1200 (giao tiếp USART2).
- **Cảm biến bụi mịn**: APM10 (giao tiếp USART3).
- **Cảm biến nhiệt độ & độ ẩm**: DHT11 (Chân PA4).
- **Thiết bị cảnh báo**:
  - LED cảnh báo (Chân PC13).
  - Buzzer/Còi chip (Chân PA1).

## Các thư viện sử dụng (Libraries)

- `U8g2lib` cho màn hình hiển thị OLED.
- `DHT` cho cảm biến nhiệt độ & độ ẩm.
- Thư viện nội bộ: `ACD1200`, `APM10`.

## Cấu hình chân (Pin Configuration)

- **DHT11**: `PA4`
- **Buzzer**: `PA1`
- **LED**: `PC13`
- **ACD1200**: Serial2 (`USART2`)
- **APM10**: Serial3 (`USART3`)
