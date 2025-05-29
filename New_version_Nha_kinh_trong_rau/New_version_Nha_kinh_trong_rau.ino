#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi Credentials
#define WIFI_SSID "Phong Tro Tang 3.3"
#define WIFI_PASSWORD "99999999"

// Firebase
#define FIREBASE_HOST "https://greenhouse-46794-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "yE66g4zyWlyHUS8X1DNHU8hwwihhGggab8McqSYC"

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cảm biến khí (MQ-2) mô phỏng CO2
#define GAS_SENSOR_PIN 36
#define CO2_THRESHOLD 2000 // Ngưỡng CO2 (ppm)

// Thiết bị
#define DEN_1 26   
#define QUAT_1 25    
#define TUOI_1 15 
#define DEN_2 14   
#define QUAT_2 27    
#define TUOI_2 33 
#define DEN_3 32   
#define QUAT_3 17    
#define TUOI_3 22 
#define DEN_4 21   
#define QUAT_4 19    
#define TUOI_4 5  

// Biến lưu dữ liệu cảm biến
int nhietDo = 0, doAmKK = 0; // Sử dụng kiểu int cho các biến
int co2PPM = 0;

// Firebase Objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

// Thời gian xử lý không chặn
unsigned long previousMillis = 0;
const long interval = 2000; // Gửi dữ liệu mỗi 2 giây

// Danh sách vườn
const char* vuons[] = {"vuon1", "vuon2", "vuon3", "vuon4"};
const int numVuons = 4;

void setup() {
    Serial.begin(115200);
    Serial.println(F("Khởi động DHT11, MQ-2 và Firebase cho 4 phòng!"));

    // Thiết lập chân 
    int Pins[] = {DEN_1, QUAT_1, TUOI_1, DEN_2, QUAT_2, TUOI_2,
                  DEN_3, QUAT_3, TUOI_3, DEN_4, QUAT_4, TUOI_4};
    for (int i = 0; i < 12; i++) {
        pinMode(Pins[i], OUTPUT);
        digitalWrite(Pins[i], LOW);
    }
    pinMode(GAS_SENSOR_PIN, INPUT);

    // Khởi động cảm biến
    dht.begin();

    // Kết nối WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

    // Cấu hình Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
        Serial.println("Đã kết nối Firebase!");
        for (int i = 0; i < numVuons; i++) {
            Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", 1);
        }
    } else {
        Serial.println("Không thể kết nối Firebase!");
    }

    // Khởi tạo trạng thái
    initializeFirebaseStates();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Kiểm tra WiFi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi ngắt kết nối, thử lại...");
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            int retryCount = 0, maxRetries = 20;
            while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
                Serial.print(".");
                delay(500);
                retryCount++;
            }
            Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi connected." : "\nKhông thể kết nối WiFi!");
            for (int i = 0; i < numVuons; i++) {
                Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", WiFi.status() == WL_CONNECTED ? 1 : 0);
            }
        }

        if (Firebase.ready()) {
            // Đọc và gửi dữ liệu cảm biến
            docDuLieuDHT11();
            docDuLieuMQ2();

            // Điều khiển thiết bị
            for (int i = 0; i < numVuons; i++) {
                String basePath = "/" + String(vuons[i]);
                nhanLenhTuFirebase(basePath + "/den", i == 0 ? DEN_1 : i == 1 ? DEN_2 : i == 2 ? DEN_3 : DEN_4);
                nhanLenhTuFirebase(basePath + "/quat", i == 0 ? QUAT_1 : i == 1 ? QUAT_2 : i == 2 ? QUAT_3 : QUAT_4);
                nhanLenhTuFirebase(basePath + "/tuoi", i == 0 ? TUOI_1 : i == 1 ? TUOI_2 : i == 2 ? TUOI_3 : TUOI_4);
            }
        } else {
            for (int i = 0; i < numVuons; i++) {
                Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", 0);
            }
        }
    }
}

void initializeFirebaseStates() {
    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        Firebase.setInt(firebaseData, basePath + "/temp", 0);
        Firebase.setInt(firebaseData, basePath + "/humi", 0);
        Firebase.setInt(firebaseData, basePath + "/co2", 0);
        Firebase.setInt(firebaseData, basePath + "/den", 0);
        Firebase.setInt(firebaseData, basePath + "/quat", 0);
        Firebase.setInt(firebaseData, basePath + "/tuoi", 0);
        Firebase.setInt(firebaseData, basePath + "/status/connection", 0);

        json.clear();
        json.set("den", 0);
        json.set("quat", 0);
        json.set("tuoi", 0);
        Firebase.setJSON(firebaseData, basePath + "/totalTime", json);

        Serial.println("Khởi tạo trạng thái Firebase cho " + String(vuons[i]) + " hoàn tất!");
    }
}

void docDuLieuDHT11() {
    nhietDo = (int)dht.readTemperature();
    doAmKK = (int)dht.readHumidity();
    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        if (!isnan(nhietDo) && !isnan(doAmKK)) {
            Firebase.setInt(firebaseData, basePath + "/temp", nhietDo);
            Firebase.setInt(firebaseData, basePath + "/humi", doAmKK);
            Serial.printf("%s - Nhiet do: %d°C, Do am: %d%%\n", vuons[i], nhietDo, doAmKK);
        } else {
            Firebase.setString(firebaseData, basePath + "/sensor_status", "DHT11 Error");
            Serial.println(String(vuons[i]) + " - Lỗi đọc DHT11!");
        }
    }
}

void docDuLieuMQ2() {
    int mq2Value = analogRead(GAS_SENSOR_PIN);
    co2PPM = map(mq2Value, 0, 4095, 0, 3000); // Mô phỏng CO2
    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        Firebase.setInt(firebaseData, basePath + "/co2", co2PPM);
        Serial.printf("%s - CO2: %d ppm\n", vuons[i], co2PPM);
        if (co2PPM > CO2_THRESHOLD) {
            json.clear();
            json.set("message", "Nồng độ CO2 quá cao: " + String(co2PPM) + " ppm");
            Firebase.setJSON(firebaseData, basePath + "/logs/" + String(millis()), json);
        }
    }
}

void nhanLenhTuFirebase(const String& path, int pin) {
    if (Firebase.getInt(firebaseData, path)) {
        int trangThai = firebaseData.intData();
        digitalWrite(pin, trangThai ? HIGH : LOW);
        Serial.printf("%s: %s\n", path.c_str(), trangThai ? "Bat" : "Tat");
    } else {
        Serial.printf("Loi doc %s: %s\n", path.c_str(), firebaseData.errorReason().c_str());
    }
}