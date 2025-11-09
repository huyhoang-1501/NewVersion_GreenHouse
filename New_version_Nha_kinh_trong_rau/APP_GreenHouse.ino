#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi Credentials
#define WIFI_SSID "Phong Tro Tang 3.3"
#define WIFI_PASSWORD "99999999"

// Firebase Credentials
#define FIREBASE_HOST "https://app-greenhouse-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "6wB1R6O1MxiCkfoLPt6utfaHUP06BwCzK1Ftv9vM"

// DHT11 Configuration
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// CO2 Sensor Configuration
#define CO2_PIN 36 // Analog pin for CO2 sensor (e.g., MQ-135)

// Den, Quat, and Tuoi Pin Configuration
#define DEN_PIN 26  // Đèn
#define QUAT_PIN 25 // Quạt
#define TUOI_PIN 15 // Tưới

// Firebase Objects
FirebaseData fbdo;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Timing
unsigned long previousMillis = 0;
const long interval = 500; // Giảm xuống 500ms để tăng tần suất kiểm tra

void setup() {
    Serial.begin(115200);
    Serial.println(F("Khởi động hệ thống cho ESP32 DHT, CO2, Đèn, Quạt & Tưới Control!"));

    // Thiết lập chân Đèn
    pinMode(DEN_PIN, OUTPUT);
    digitalWrite(DEN_PIN, LOW);
    Serial.println("Chân Đèn được thiết lập: GPIO " + String(DEN_PIN));

    // Thiết lập chân Quạt
    pinMode(QUAT_PIN, OUTPUT);
    digitalWrite(QUAT_PIN, LOW);
    Serial.println("Chân Quạt được thiết lập: GPIO " + String(QUAT_PIN));

    // Thiết lập chân Tưới
    pinMode(TUOI_PIN, OUTPUT);
    digitalWrite(TUOI_PIN, LOW);
    Serial.println("Chân Tưới được thiết lập: GPIO " + String(TUOI_PIN));

    // Thiết lập chân CO2 (analog input)
    pinMode(CO2_PIN, INPUT);
    Serial.println("Chân CO2 được thiết lập: GPIO " + String(CO2_PIN));

    // Kiểm tra tất cả thiết bị
    testDevices();

    // Khởi động DHT11
    dht.begin();

    // Kết nối WiFi
    connectWiFi();

    // Cấu hình Firebase
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
        Serial.println("Đã kết nối Firebase!");
    } else {
        Serial.println("Không thể kết nối Firebase!");
    }

    // Khởi tạo trạng thái Firebase
    initializeFirebaseStates();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Kiểm tra WiFi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi ngắt kết nối, thử lại...");
            connectWiFi();
        }

        if (Firebase.ready()) {
            Serial.println("Đang kiểm tra trạng thái...");
            sendSensorData();
            controlDevices();
        } else {
            Serial.println("Firebase không sẵn sàng!");
        }
    }
}

void connectWiFi() {
    Serial.print("Đang kết nối WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int retryCount = 0, maxRetries = 20;
    while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
        Serial.print(".");
        delay(500);
        retryCount++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nĐã kết nối WiFi! IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nKhông thể kết nối WiFi!");
    }
}

void initializeFirebaseStates() {
    // Khởi tạo tất cả giá trị dưới dạng số nguyên
    if (Firebase.setInt(fbdo, "/DEN", 0)) {
        Serial.println("Khởi tạo DEN thành công!");
    } else {
        Serial.println("Lỗi khởi tạo DEN: " + fbdo.errorReason());
    }
    if (Firebase.setInt(fbdo, "/QUAT", 0)) {
        Serial.println("Khởi tạo QUAT thành công!");
    } else {
        Serial.println("Lỗi khởi tạo QUAT: " + fbdo.errorReason());
    }
    if (Firebase.setInt(fbdo, "/TUOI", 0)) {
        Serial.println("Khởi tạo TUOI thành công!");
    } else {
        Serial.println("Lỗi khởi tạo TUOI: " + fbdo.errorReason());
    }
    // Khởi tạo các giá trị cảm biến
    if (Firebase.setFloat(fbdo, "/TEMPERATURE", 0.0)) {
        Serial.println("Khởi tạo TEMPERATURE thành công!");
    } else {
        Serial.println("Lỗi khởi tạo TEMPERATURE: " + fbdo.errorReason());
    }
    if (Firebase.setFloat(fbdo, "/HUMIDITY", 0.0)) {
        Serial.println("Khởi tạo HUMIDITY thành công!");
    } else {
        Serial.println("Lỗi khởi tạo HUMIDITY: " + fbdo.errorReason());
    }
    if (Firebase.setFloat(fbdo, "/CO2", 0.0)) {
        Serial.println("Khởi tạo CO2 thành công!");
    } else {
        Serial.println("Lỗi khởi tạo CO2: " + fbdo.errorReason());
    }
}

void sendSensorData() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int co2Raw = analogRead(CO2_PIN); // Đọc giá trị thô từ cảm biến CO2
    float co2ppm = map(co2Raw, 0, 4095, 400, 5000); // Chuyển đổi sang ppm (cần hiệu chỉnh thực tế)

    if (!isnan(humidity) && !isnan(temperature)) {
        if (Firebase.setFloat(fbdo, "/TEMPERATURE", temperature)) {
            Serial.println("Nhiệt độ: " + String(temperature) + "°C");
        } else {
            Serial.println("Lỗi gửi nhiệt độ: " + fbdo.errorReason());
        }
        if (Firebase.setFloat(fbdo, "/HUMIDITY", humidity)) {
            Serial.println("Độ ẩm: " + String(humidity) + "%");
        } else {
            Serial.println("Lỗi gửi độ ẩm: " + fbdo.errorReason());
        }
    } else {
        Firebase.setString(fbdo, "/sensor_status", "DHT11 Error");
        Serial.println("Lỗi đọc DHT11!");
    }

    // Gửi dữ liệu CO2
    if (Firebase.setFloat(fbdo, "/CO2", co2ppm)) {
        Serial.println("CO2: " + String(co2ppm) + " ppm");
    } else {
        Serial.println("Lỗi gửi CO2: " + fbdo.errorReason());
    }
}

void controlDevices() {
    // Điều khiển Đèn
    if (Firebase.getString(fbdo, "/DEN")) {
        String rawValue = fbdo.stringData();
        int state = (rawValue == "1") ? 1 : (rawValue == "0") ? 0 : 0; // Phân tích chuỗi
        Serial.println("Giá trị Đèn từ Firebase: " + rawValue + " (Parsed: " + String(state) + ")");
        digitalWrite(DEN_PIN, state == 1 ? HIGH : LOW);
        Serial.println("Đèn: " + String(state == 1 ? "Bật" : "Tắt") + " (Pin state: " + String(digitalRead(DEN_PIN)) + ")");
        // Gửi xác nhận trạng thái về Firebase
        Firebase.setString(fbdo, "/DEN_STATUS", state == 1 ? "Bật" : "Tắt");
    } else {
        Serial.println("Lỗi đọc trạng thái Đèn: " + fbdo.errorReason());
    }

    // Điều khiển Quạt
    if (Firebase.getString(fbdo, "/QUAT")) {
        String rawValue = fbdo.stringData();
        int state = (rawValue == "1") ? 1 : (rawValue == "0") ? 0 : 0; // Phân tích chuỗi
        Serial.println("Giá trị Quạt từ Firebase: " + rawValue + " (Parsed: " + String(state) + ")");
        digitalWrite(QUAT_PIN, state == 1 ? HIGH : LOW);
        Serial.println("Quạt: " + String(state == 1 ? "Bật" : "Tắt") + " (Pin state: " + String(digitalRead(QUAT_PIN)) + ")");
        // Gửi xác nhận trạng thái về Firebase
        Firebase.setString(fbdo, "/QUAT_STATUS", state == 1 ? "Bật" : "Tắt");
    } else {
        Serial.println("Lỗi đọc trạng thái Quạt: " + fbdo.errorReason());
    }

    // Điều khiển Tưới
    if (Firebase.getString(fbdo, "/TUOI")) {
        String rawValue = fbdo.stringData();
        int state = (rawValue == "1") ? 1 : (rawValue == "0") ? 0 : 0; // Phân tích chuỗi
        Serial.println("Giá trị Tưới từ Firebase: " + rawValue + " (Parsed: " + String(state) + ")");
        digitalWrite(TUOI_PIN, state == 1 ? HIGH : LOW);
        Serial.println("Tưới: " + String(state == 1 ? "Bật" : "Tắt") + " (Pin state: " + String(digitalRead(TUOI_PIN)) + ")");
        // Gửi xác nhận trạng thái về Firebase
        Firebase.setString(fbdo, "/TUOI_STATUS", state == 1 ? "Bật" : "Tắt");
    } else {
        Serial.println("Lỗi đọc trạng thái Tưới: " + fbdo.errorReason());
    }
}

void testDevices() {
    Serial.println("Kiểm tra các thiết bị...");

    // Test Đèn
    Serial.println("Kiểm tra Đèn...");
    digitalWrite(DEN_PIN, HIGH);
    Serial.println("Đèn bật... (Pin state: " + String(digitalRead(DEN_PIN)) + ")");
    delay(2000);
    digitalWrite(DEN_PIN, LOW);
    Serial.println("Đèn tắt... (Pin state: " + String(digitalRead(DEN_PIN)) + ")");
    delay(2000);

    // Test Quạt
    Serial.println("Kiểm tra Quạt...");
    digitalWrite(QUAT_PIN, HIGH);
    Serial.println("Quạt bật... (Pin state: " + String(digitalRead(QUAT_PIN)) + ")");
    delay(2000);
    digitalWrite(QUAT_PIN, LOW);
    Serial.println("Quạt tắt... (Pin state: " + String(digitalRead(QUAT_PIN)) + ")");
    delay(2000);

    // Test Tưới
    Serial.println("Kiểm tra Tưới...");
    digitalWrite(TUOI_PIN, HIGH);
    Serial.println("Tưới bật... (Pin state: " + String(digitalRead(TUOI_PIN)) + ")");
    delay(2000);
    digitalWrite(TUOI_PIN, LOW);
    Serial.println("Tưới tắt... (Pin state: " + String(digitalRead(TUOI_PIN)) + ")");
    delay(2000);
}