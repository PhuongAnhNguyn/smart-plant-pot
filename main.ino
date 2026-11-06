#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================== KHỞI TẠO OLED ==================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================== 1. CAU HINH WIFI ==================

const char* AP_SSID = "ChauCayThongMinh";
const char* AP_PASS = "12345678";

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// ================== 2. CAU HINH CAM BIEN ==================

// DHT11
#define DHTPIN A1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Cam bien do am dat
#define SOIL_PIN A2

// Gia tri hieu chinh tam thoi
// Can thay bang gia tri do thuc te cua cam bien
#define SOIL_DRY_VALUE 4000
#define SOIL_WET_VALUE 1400

// ================== 3. WEB SERVER ==================

WebServer server(80);

// ================== 4. BIEN DU LIEU ==================

float nhietDo = NAN;
float doAmKhongKhi = NAN;
float doAmDat = 0;

int giaTriDatRaw = 0;

String trangThaiCay = "Dang khoi dong";
String loiKhuyen = "Dang doc du lieu cam bien";

bool dhtHopLe = false;

unsigned long lanDocCuoi = 0;
const unsigned long chuKyDoc = 2000;

// ================== 5. GIAO DIEN WEBSITE ==================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Smart Plant Pot</title>

<style>
* {
    box-sizing: border-box;
}

body {
    margin: 0;
    padding: 20px;
    font-family: Arial, sans-serif;
    background: #eef5ee;
    color: #243b2a;
}

.container {
    max-width: 800px;
    margin: auto;
}

h1 {
    text-align: center;
    color: #24733d;
}

.subtitle {
    text-align: center;
    color: #617566;
}

.grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 15px;
    margin-top: 25px;
}

.card {
    background: white;
    padding: 22px;
    border-radius: 15px;
    box-shadow: 0 3px 12px #00000012;
}

.card h3 {
    margin-top: 0;
    color: #55745c;
}

.value {
    font-size: 35px;
    font-weight: bold;
    color: #24733d;
}

.unit {
    font-size: 16px;
}

.status {
    background: white;
    padding: 22px;
    border-radius: 15px;
    margin-top: 18px;
    box-shadow: 0 3px 12px #00000012;
}

.status h2 {
    color: #24733d;
}

.footer {
    text-align: center;
    color: #718075;
    font-size: 13px;
    margin-top: 25px;
}
</style>
</head>

<body>

<div class="container">

<h1>🌱 SMART PLANT POT</h1>

<p class="subtitle">
He thong giam sat suc khoe cay trong thoi gian thuc
</p>

<div class="grid">

    <div class="card">
        <h3>🌡 Nhiet do</h3>
        <div class="value">
            <span id="temp">--</span>
            <span class="unit">°C</span>
        </div>
    </div>

    <div class="card">
        <h3>💧 Do am khong khi</h3>
        <div class="value">
            <span id="air">--</span>
            <span class="unit">%</span>
        </div>
    </div>

    <div class="card">
        <h3>🌿 Do am dat</h3>
        <div class="value">
            <span id="soil">--</span>
            <span class="unit">%</span>
        </div>
    </div>

</div>

<div class="status">
    <h2>Trang thai cay</h2>
    <h3 id="status">Dang ket noi...</h3>
    <p id="advice">Dang tai du lieu...</p>
    <p>Gia tri ADC dat: <b id="raw">--</b></p>
    <p>Cap nhat lan cuoi: <b id="time">--</b></p>
</div>

<div class="footer">
Smart Plant Pot | ESP32 IoT System
</div>

</div>

<script>
async function updateData() {
    try {
        const response = await fetch('/data');

        if (!response.ok) {
            throw new Error("HTTP " + response.status);
        }

        const data = await response.json();

        document.getElementById("temp").innerText =
            data.nhietDo === null ? "--" : data.nhietDo.toFixed(1);

        document.getElementById("air").innerText =
            data.doAmKhongKhi === null ? "--" : data.doAmKhongKhi.toFixed(1);

        document.getElementById("soil").innerText =
            data.doAmDat;

        document.getElementById("status").innerText =
            data.trangThai;

        document.getElementById("advice").innerText =
            data.loiKhuyen;

        document.getElementById("raw").innerText =
            data.adcDat;

        document.getElementById("time").innerText =
            data.thoiGian + " giay";

    } catch (error) {
        document.getElementById("status").innerText =
            "Mat ket noi voi ESP32";

        document.getElementById("advice").innerText =
            "Hay kiem tra Wi-Fi va tai lai trang.";
    }
}

updateData();
setInterval(updateData, 2000);
</script>

</body>
</html>
)rawliteral";

// ================== 6. DANH GIA TINH TRANG CAY ==================

void danhGiaTinhTrangCay() {

    if (doAmDat < 30) {
        trangThaiCay = "Dat dang kho";
        loiKhuyen = "Nen tuoi them nuoc cho cay";
    }
    else if (doAmDat < 45) {
        trangThaiCay = "Dat hoi kho";
        loiKhuyen = "Can theo doi va bo sung nuoc";
    }
    else if (doAmDat <= 75) {
        trangThaiCay = "Dat co do am phu hop";
        loiKhuyen = "Do am dat dang on dinh";
    }
    else {
        trangThaiCay = "Dat qua am";
        loiKhuyen = "Han che tuoi them nuoc";
    }

    if (dhtHopLe) {
        if (nhietDo >= 35) {
            loiKhuyen += " | Nhiet do khong khi cao";
        }
        else if (nhietDo < 15) {
            loiKhuyen += " | Nhiet do khong khi thap";
        }
    }
}

// ================== HIỂN THỊ OLED GIỐNG HỆT SERIAL ==================

void inDuLieuOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.print("Time: ");
    display.print(millis() / 1000);
    display.println(" s");

    display.print("Nhiet do: ");
    if (dhtHopLe) {
        display.print(nhietDo, 1);
        display.println(" C");
    } else {
        display.println("Loi");
    }

    display.print("Do am KK: ");
    if (dhtHopLe) {
        display.print(doAmKhongKhi, 1);
        display.println(" %");
    } else {
        display.println("Loi");
    }

    display.print("ADC dat : ");
    display.println(giaTriDatRaw);

    display.print("Do am dat: ");
    display.print(doAmDat, 0);
    display.println(" %");

    display.print("TT: ");
    display.println(trangThaiCay);

    display.print("IP: ");
    display.println(WiFi.softAPIP());

    display.display();
}

// ================== 7. DOC CAM BIEN ==================

void docCamBien() {

    // Doc DHT11
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
        nhietDo = t;
        doAmKhongKhi = h;
        dhtHopLe = true;
    }
    else {
        dhtHopLe = false;
        Serial.println("CANH BAO: Khong doc duoc DHT11!");
    }

    // Doc cam bien do am dat
    giaTriDatRaw = analogRead(SOIL_PIN);

    // Chuyen ADC thanh phan tram
    int soilPercent = map(
        giaTriDatRaw,
        SOIL_DRY_VALUE,
        SOIL_WET_VALUE,
        0,
        100
    );

    doAmDat = constrain(soilPercent, 0, 100);

    // Danh gia tinh trang
    danhGiaTinhTrangCay();

    // In du lieu ra Serial
    inDuLieuSerial();

    // In du lieu ra OLED
    inDuLieuOLED();
}

// ================== 8. IN DU LIEU SERIAL ==================

void inDuLieuSerial() {

    Serial.println();
    Serial.println("====================================");
    Serial.println("       SMART PLANT POT DATA");
    Serial.println("====================================");

    Serial.print("Thoi gian hoat dong: ");
    Serial.print(millis() / 1000);
    Serial.println(" giay");

    Serial.println("------------------------------------");

    Serial.print("Nhiet do: ");

    if (dhtHopLe) {
        Serial.print(nhietDo);
        Serial.println(" °C");
    }
    else {
        Serial.println("Loi cam bien");
    }

    Serial.print("Do am khong khi: ");

    if (dhtHopLe) {
        Serial.print(doAmKhongKhi);
        Serial.println(" %");
    }
    else {
        Serial.println("Loi cam bien");
    }

    Serial.print("ADC do am dat: ");
    Serial.println(giaTriDatRaw);

    Serial.print("Do am dat: ");
    Serial.print(doAmDat);
    Serial.println(" %");

    Serial.println("------------------------------------");

    Serial.print("Trang thai cay: ");
    Serial.println(trangThaiCay);

    Serial.print("Loi khuyen: ");
    Serial.println(loiKhuyen);

    Serial.println("------------------------------------");

    Serial.print("Dia chi IP: ");
    Serial.println(WiFi.softAPIP());

    Serial.println("====================================");
}

// ================== 9. TAO API JSON ==================

void xuLyDuLieuJSON() {

    String json = "{";

    json += "\"nhietDo\":";

    if (dhtHopLe) {
        json += String(nhietDo, 1);
    }
    else {
        json += "null";
    }

    json += ",\"doAmKhongKhi\":";

    if (dhtHopLe) {
        json += String(doAmKhongKhi, 1);
    }
    else {
        json += "null";
    }

    json += ",\"doAmDat\":";
    json += String((int)doAmDat);

    json += ",\"adcDat\":";
    json += String(giaTriDatRaw);

    json += ",\"trangThai\":\"";
    json += trangThaiCay;

    json += "\",\"loiKhuyen\":\"";
    json += loiKhuyen;

    json += "\",\"thoiGian\":";
    json += String(millis() / 1000);

    json += "}";

    server.send(
        200,
        "application/json; charset=utf-8",
        json
    );
}

// ================== 10. KHOI DONG WIFI ==================

void khoiDongWiFi() {

    Serial.println();
    Serial.println("Dang khoi dong Wi-Fi...");

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(
        local_IP,
        gateway,
        subnet
    );

    bool thanhCong = WiFi.softAP(
        AP_SSID,
        AP_PASS
    );

    if (thanhCong) {

        Serial.println("Wi-Fi da khoi dong thanh cong!");

        Serial.print("Ten Wi-Fi: ");
        Serial.println(AP_SSID);

        Serial.print("Mat khau: ");
        Serial.println(AP_PASS);

        Serial.print("Dia chi IP: ");
        Serial.println(WiFi.softAPIP());

    }
    else {
        Serial.println("LOI: Khong the khoi dong Wi-Fi!");
    }
}

// ================== 11. CAU HINH WEB SERVER ==================

void khoiDongWebServer() {

    // Trang chu
    server.on("/", HTTP_GET, []() {

        server.send_P(
            200,
            "text/html; charset=utf-8",
            INDEX_HTML
        );

    });

    // API du lieu
    server.on("/data", HTTP_GET, xuLyDuLieuJSON);

    // Xu ly duong dan khong ton tai
    server.onNotFound([]() {

        server.send(
            404,
            "text/plain",
            "404 - Khong tim thay trang!"
        );

    });

    server.begin();

    Serial.println("Web server da san sang!");
    Serial.println("Truy cap: http://192.168.4.1");
}

// ================== 12. SETUP ==================

void setup() {

    Serial.begin(115200);
    delay(1000);

    // Khoi dong OLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    Serial.println();
    Serial.println("====================================");
    Serial.println("       SMART PLANT POT");
    Serial.println("====================================");

    // Cau hinh ADC
    analogReadResolution(12);
    analogSetPinAttenuation(SOIL_PIN, ADC_11db);

    pinMode(SOIL_PIN, INPUT);

    // Khoi dong DHT11
    dht.begin();

    // Khoi dong Wi-Fi
    khoiDongWiFi();

    // Khoi dong WebServer
    khoiDongWebServer();

    Serial.println();
    Serial.println("HE THONG DA SAN SANG!");
    Serial.println("====================================");

    // Doc du lieu lan dau
    docCamBien();

    lanDocCuoi = millis();
}

// ================== 13. LOOP ==================

void loop() {

    // Xu ly yeu cau tu trinh duyet lien tuc
    server.handleClient();

    // Doc cam bien moi 2 giay
    if (millis() - lanDocCuoi >= chuKyDoc) {

        lanDocCuoi = millis();

        docCamBien();
    }

    delay(2);
}