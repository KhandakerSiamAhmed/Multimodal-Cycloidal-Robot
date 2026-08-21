/**
 * @file Joint1_ESP32C6_AS5600.ino
 * @brief Joint 1 Angle Telemetry Server for ESP32-C6 Super Mini & AS5600 Encoder
 * @details Reads 12-bit magnetic encoder on NEMA 17 shaft, connects to WiFi,
 *          supports Over-The-Air (OTA) updates, and hosts JSON server & Web UI.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "pinout.h"
#include "AS5600_Driver.h"
#include "web_dashboard.h"

// ==============================================================================
// 1. WIFI NETWORK CREDENTIALS
// ==============================================================================
const char* WIFI_SSID     = "1233";
const char* WIFI_PASSWORD = "12341234";

// Device hostname on the local network (accessible at http://joint1.local)
const char* OTA_HOSTNAME  = "Joint1-ESP32C6";

// ==============================================================================
// 2. HARDWARE OBJECTS & SERVER INSTANCES
// ==============================================================================
AS5600_Encoder encoder;
WebServer server(80);

unsigned long lastSerialPrintTime = 0;
const unsigned long SERIAL_PRINT_INTERVAL_MS = 30; // Real-time high-speed stream (33 Hz / 30ms)

// Motor modes
enum ControlMode { MODE_CONTINUOUS, MODE_PID };
ControlMode current_mode = MODE_CONTINUOUS;
float target_continuous_rpm = 0.0f; // Start stopped

// PID Profile Variables
float pid_setpoint = 0.0f;
float pid_kp = 1.0f;
float pid_ki = 0.1f;
float pid_kd = 0.05f;
float pid_integral = 0.0f;
float pid_prev_error = 0.0f;

// Motor Control Variables
const float STEPS_PER_REV = 6400.0f; 
const float MAX_RPM = 200.0f;
const float MAX_DEG_PER_SEC = MAX_RPM * 360.0f / 60.0f; // 1200 deg/sec
bool isMotorRunning = false;

const float ACCEL_RPM_PER_SEC = 400.0f; // 0.5s ramp to 200 RPM
const float DT = 0.01f; // 10ms control loop (100 Hz)
float current_cmd_rpm = 0.0f;
float target_cmd_rpm = 0.0f;

int current_dir_state = -1;
uint32_t lastFreqHz = 0;

unsigned long lastControlTimeUs = 0;
float current_speed_rpm = 0.0f;
float last_speed_angle = 0.0f;


// ── Motor direction ────────────────────────────────────────────────────────
// Set to 'true' if the motor rotates in the WRONG direction (PID runaway).
// This flips the DIR pin polarity without rewiring.
const bool MOTOR_DIR_INVERT = true;  // ← flipped: encoder correct but motor runs away

// ── Setpoint safety clamp ──────────────────────────────────────────────────
// Incoming setpoints (from serial / web UI) are clamped to be within this
// many degrees of the current encoder position.  Prevents a stale or
// mis-scaled setpoint from commanding a multi-rotation runaway move.
const float MAX_SETPOINT_JUMP_DEG = 180.0f;

// ── Open-loop driver test mode ─────────────────────────────────────────────
// Send 't' via Serial to toggle. Alternates: HIGH speed 3 s → LOW speed 2 s.
// Used to verify the TMC2209 responds to speed changes independently of PID.
bool     testModeActive    = false;
unsigned long testPhaseStartMs = 0;
bool     testPhaseHigh     = true;   // true = high-speed phase
const float TEST_HIGH_RPM      = 200.0f; // 200 RPM
const float TEST_LOW_RPM       = 20.0f;  // 20 RPM
const unsigned long TEST_HIGH_MS       = 3000;  // 3 s high
const unsigned long TEST_LOW_MS        = 2000;  // 2 s low

// ==============================================================================
// 3. HTTP SERVER ROUTE HANDLERS
// ==============================================================================

/**
 * @brief GET /angle or /api/joint1
 * Returns current joint angle in exact JSON format: {"joint1": 123.45}
 */
void handleGetAngle() {
  float angle = encoder.getDegrees();

  String json = "{\"joint1\":" + String(angle, 2) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.send(200, "application/json", json);
}

/**
 * @brief GET /status
 * Returns comprehensive telemetry (angle, cumulative degrees, raw ticks, magnet diagnostics)
 */
void handleGetStatus() {
  float angle = encoder.getDegrees();
  float cumAngle = encoder.getCumulativeDegrees();
  uint16_t raw = encoder.readRawAngle();
  AS5600_Status st = encoder.getStatus();

  String json = "{";
  json += "\"joint1\":" + String(angle, 2) + ",";
  json += "\"speed_rpm\":" + String(current_speed_rpm, 2) + ",";
  json += "\"cumulative\":" + String(cumAngle, 2) + ",";
  json += "\"raw\":" + String(raw) + ",";
  json += "\"agc\":" + String(st.agc) + ",";
  json += "\"magnet_detected\":" + String(st.magnetDetected ? "true" : "false") + ",";
  json += "\"magnet_weak\":" + String(st.magnetTooWeak ? "true" : "false") + ",";
  json += "\"magnet_strong\":" + String(st.magnetTooStrong ? "true" : "false") + ",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"status\":\"" + st.statusText + "\"";
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

/**
 * @brief POST /api/zero
 * Calibrate current position to 0 degrees
 */
void handleSetZero() {
  encoder.setZero();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Zero position set\"}");
}

/**
 * @brief GET /
 * Serves the interactive visual web dashboard
 */
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

/**
 * @brief Handle 404 Not Found
 */
void handleNotFound() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "application/json", "{\"error\":\"Not Found\"}");
}

// ==============================================================================
// 4. OTA (OVER-THE-AIR) UPDATE SETUP
// ==============================================================================
void setupOTA() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  // ArduinoOTA.setPassword("admin123"); // Uncomment if password protection is desired

  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("\n[OTA] Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] Update Completed Successfully! Rebooting...");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("[OTA] Service initialized. Hostname: " + String(OTA_HOSTNAME));
}

// ==============================================================================
// 5. WIFI INITIALIZATION
// ==============================================================================
void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 24) { // ~6 second timeout
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    // Exact required serial output upon connection:
    Serial.println();
    Serial.println("connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("HTTP API URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/angle");
  } else {
    Serial.println();
    Serial.println("[WiFi] Connection timeout. Running in standalone high-speed Serial mode.");
  }
}

// ==============================================================================
// 6. SETUP ROUTINE
// ==============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000); // Allow USB CDC connection to stabilize on ESP32-C6

  Serial.println("\n===========================================");
  Serial.println("   Joint 1 - ESP32-C6 + AS5600 Encoder     ");
  Serial.println("===========================================");

  // Initialize TMC2209 Motor Driver Pins
  pinMode(TMC_EN_PIN, OUTPUT);
  pinMode(TMC_DIR_PIN, OUTPUT);
  
  // Initialize LEDC for Hardware PWM on STEP pin
  ledcAttach(TMC_STEP_PIN, 1000, 8); // Attach with dummy 1kHz frequency
  ledcWrite(TMC_STEP_PIN, 0);        // 0% duty cycle (stopped)
  
  // ── Motor direction ────────────────────────────────────────────────
  // If the motor rotates in the WRONG direction (PID runaway / going away
  // from the target instead of toward it), flip this to 'true'.
  // LOW/HIGH mapping: false = positive cmd → LOW; true = positive cmd → HIGH.
  digitalWrite(TMC_DIR_PIN, LOW); // Set default direction
  digitalWrite(TMC_EN_PIN, LOW);  // Enable motor (Active LOW)
  Serial.println("TMC2209 Stepper Driver initialized with Hardware LEDC.");

  // Initialize AS5600 Magnetic Encoder
  Serial.printf("Initializing AS5600 I2C (SDA: GPIO %d, SCL: GPIO %d)...\n", I2C_SDA_PIN, I2C_SCL_PIN);
  if (!encoder.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY)) {
    Serial.println("WARNING: AS5600 encoder not detected on I2C bus (0x36)!");
    Serial.println("Please verify SDA/SCL wiring and 3.3V power connection.");
  } else {
    Serial.println("AS5600 encoder initialized successfully.");
    AS5600_Status st = encoder.getStatus();
    Serial.println("Magnet Status: " + st.statusText);
  }

  // Connect to WiFi
  setupWiFi();

  // Initialize ArduinoOTA
  setupOTA();

  // Configure Web Server endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/angle", HTTP_GET, handleGetAngle);
  server.on("/api/joint1", HTTP_GET, handleGetAngle);
  server.on("/status", HTTP_GET, handleGetStatus);
  server.on("/api/zero", HTTP_POST, handleSetZero);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started.");
}

// ==============================================================================
// 7. SERIAL COMMAND HANDLER & DATA STREAMING
// ==============================================================================
bool streamJsonOnly = false; // When true, outputs clean JSON lines for digital twin scripts
unsigned long serialStreamInterval = 30; // ms between serial updates (33 Hz instant stream)

void handleSerialCommands() {
  while (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    if (line.startsWith("{")) {
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, line);
      if (!error) {
        if (doc.containsKey("mode")) {
          String mode = doc["mode"].as<String>();
          if (mode == "continuous") {
            current_mode = MODE_CONTINUOUS;
          } else if (mode == "pid") {
            current_mode = MODE_PID;
            pid_setpoint = encoder.getCumulativeDegrees();
            pid_integral = 0; // reset integral on mode switch
          }
        }
        if (doc.containsKey("rpm")) {
          target_continuous_rpm = doc["rpm"].as<float>();
        }
        if (doc.containsKey("setpoint")) {
          pid_setpoint = doc["setpoint"].as<float>();
        }
        if (doc.containsKey("kp")) pid_kp = doc["kp"].as<float>();
        if (doc.containsKey("ki")) pid_ki = doc["ki"].as<float>();
        if (doc.containsKey("kd")) pid_kd = doc["kd"].as<float>();
      }
      continue;
    }

    char cmd = line[0];
    if (cmd == 'a' || cmd == 'A') {
      float angle = encoder.getDegrees();
      Serial.printf("{\"joint1\": %.2f}\n", angle);
      Serial.flush();
    } 
    else if (cmd == 's' || cmd == 'S') {
      float angle = encoder.getDegrees();
      float cum = encoder.getCumulativeDegrees();
      uint16_t raw = encoder.readRawAngle();
      AS5600_Status st = encoder.getStatus();
      Serial.printf("{\"joint1\": %.2f, \"cumulative\": %.2f, \"raw\": %u, \"agc\": %u, \"magnet\": \"%s\"}\n",
                    angle, cum, raw, st.agc, st.statusText.c_str());
      Serial.flush();
    } 
    else if (cmd == 'z' || cmd == 'Z') {
      encoder.setZero();
      Serial.println("{\"status\":\"ok\",\"message\":\"Zero calibrated via Serial\"}");
      Serial.flush();
    }
    else if (cmd == 't' || cmd == 'T') {
      testModeActive = !testModeActive;
      if (testModeActive) {
        testPhaseHigh    = true;
        testPhaseStartMs = millis();
        isMotorRunning   = false; // let the test loop take over
        digitalWrite(TMC_DIR_PIN, LOW); // always CW for this test
        uint32_t freq = (uint32_t)((TEST_HIGH_RPM / 60.0f) * STEPS_PER_REV);
        ledcChangeFrequency(TMC_STEP_PIN, freq, 8);
        ledcWrite(TMC_STEP_PIN, 128); // 50% duty cycle
        Serial.println("{\"test_mode\":\"ON\",\"phase\":\"HIGH\",\"note\":\"3s fast / 2s slow loop\"}");
      } else {
        isMotorRunning = false;
        ledcWrite(TMC_STEP_PIN, 0); // Stop PWM
        Serial.println("{\"test_mode\":\"OFF\"}");
      }
      Serial.flush();
    }
    else if (cmd == 'j' || cmd == 'J') {
      streamJsonOnly = !streamJsonOnly;
      Serial.printf("{\"stream_mode\": \"%s\"}\n", streamJsonOnly ? "JSON_ONLY" : "HUMAN_READABLE");
      Serial.flush();
    }
    else if (cmd == '+' || cmd == 'f' || cmd == 'F') {
      if (serialStreamInterval > 10) serialStreamInterval -= 10;
      Serial.printf("[Config] Stream interval set to %lu ms (~%lu Hz)\n", serialStreamInterval, 1000 / serialStreamInterval);
      Serial.flush();
    }
    else if (cmd == '-' || cmd == 'l' || cmd == 'L') {
      if (serialStreamInterval < 500) serialStreamInterval += 20;
      Serial.printf("[Config] Stream interval set to %lu ms (~%lu Hz)\n", serialStreamInterval, 1000 / serialStreamInterval);
      Serial.flush();
    }
    else if (cmd == 'h' || cmd == '?' || cmd == 'H') {
      Serial.println("\n--- Joint 1 Serial Commands ---");
      Serial.println(" 'a' : Get current angle JSON {\"joint1\": ...}");
      Serial.println(" 's' : Get full status JSON");
      Serial.println(" 'z' : Zero / tare current position");
      Serial.println(" 'j' : Toggle pure JSON stream mode on/off");
      Serial.println(" '+' : Increase stream update frequency");
      Serial.println(" '-' : Decrease stream update frequency");
      Serial.println(" 't' : Toggle open-loop driver test (3s fast / 2s slow)");
      Serial.println(" 'h' : Show this help menu\n");
      Serial.flush();
    }
  }
}

// ==============================================================================
// 8. MAIN LOOP
// ==============================================================================
void loop() {
  unsigned long currentMicros = micros();
  unsigned long currentMs     = millis();

  // ══════════════════════════════════════════════════════════════════
  // OPEN-LOOP DRIVER TEST  (send 't' to toggle)
  // Bypasses PID completely. Alternates 3 s high / 2 s low speed.
  // ══════════════════════════════════════════════════════════════════
  if (testModeActive) {
    unsigned long elapsed = currentMs - testPhaseStartMs;

    if (testPhaseHigh && elapsed >= TEST_HIGH_MS) {
      // Switch → LOW speed phase
      testPhaseHigh    = false;
      testPhaseStartMs = currentMs;
      uint32_t freq = (uint32_t)((TEST_LOW_RPM / 60.0f) * STEPS_PER_REV);
      ledcChangeFrequency(TMC_STEP_PIN, freq, 8);
      ledcWrite(TMC_STEP_PIN, 128); // 50% duty cycle
      Serial.println("{\"test_mode\":\"ON\",\"phase\":\"LOW\",\"rpm\":20}");
      Serial.flush();
    } else if (!testPhaseHigh && elapsed >= TEST_LOW_MS) {
      // Switch → HIGH speed phase
      testPhaseHigh    = true;
      testPhaseStartMs = currentMs;
      uint32_t freq = (uint32_t)((TEST_HIGH_RPM / 60.0f) * STEPS_PER_REV);
      ledcChangeFrequency(TMC_STEP_PIN, freq, 8);
      ledcWrite(TMC_STEP_PIN, 128); // 50% duty cycle
      Serial.println("{\"test_mode\":\"ON\",\"phase\":\"HIGH\",\"rpm\":200}");
      Serial.flush();
    }

    // Service OTA and serial even in test mode
    ArduinoOTA.handle();
    server.handleClient();
    handleSerialCommands();
    return; // skip PID and normal step generation below
  }

  // Control Loop (100 Hz)
  if (currentMicros - lastControlTimeUs >= (unsigned long)(DT * 1000000.0f)) {
    lastControlTimeUs = currentMicros;

    encoder.update();
    float currentAngle = encoder.getCumulativeDegrees();

    // Calculate measured RPM
    current_speed_rpm = ((currentAngle - last_speed_angle) / DT) / 360.0f * 60.0f;
    last_speed_angle = currentAngle;

    if (current_mode == MODE_CONTINUOUS) {
        target_cmd_rpm = target_continuous_rpm;
    } else {
        // MODE_PID
        float error = pid_setpoint - currentAngle;
        pid_integral += error * DT;
        
        // Anti-windup
        float max_i = MAX_RPM / pid_ki;
        if (pid_ki > 0.001f) {
            if (pid_integral > max_i) pid_integral = max_i;
            if (pid_integral < -max_i) pid_integral = -max_i;
        }
        
        float derivative = (error - pid_prev_error) / DT;
        pid_prev_error = error;
        
        float output = (pid_kp * error) + (pid_ki * pid_integral) + (pid_kd * derivative);
        
        if (output > MAX_RPM) output = MAX_RPM;
        if (output < -MAX_RPM) output = -MAX_RPM;
        
        target_cmd_rpm = output;
    }

    // Acceleration Profile
    if (current_cmd_rpm < target_cmd_rpm) {
        current_cmd_rpm += ACCEL_RPM_PER_SEC * DT;
        if (current_cmd_rpm > target_cmd_rpm) current_cmd_rpm = target_cmd_rpm;
    } else if (current_cmd_rpm > target_cmd_rpm) {
        current_cmd_rpm -= ACCEL_RPM_PER_SEC * DT;
        if (current_cmd_rpm < target_cmd_rpm) current_cmd_rpm = target_cmd_rpm;
    }

    if (fabs(current_cmd_rpm) < 2.0f) {
        if (lastFreqHz != 0) {
            ledcWrite(TMC_STEP_PIN, 0); // Stop hardware PWM when practically zero
            lastFreqHz = 0;
        }
    } else {
        if (current_cmd_rpm > 0) {
            int new_dir = MOTOR_DIR_INVERT ? LOW : HIGH;
            if (current_dir_state != new_dir) {
                current_dir_state = new_dir;
            }
            digitalWrite(TMC_DIR_PIN, current_dir_state);
        } else {
            int new_dir = MOTOR_DIR_INVERT ? HIGH : LOW;
            if (current_dir_state != new_dir) {
                current_dir_state = new_dir;
            }
            digitalWrite(TMC_DIR_PIN, current_dir_state);
        }
        
        float freqHz = (fabs(current_cmd_rpm) / 60.0f) * STEPS_PER_REV;
        if (freqHz < 10.0f) freqHz = 10.0f;
        uint32_t freqHzInt = (uint32_t)freqHz;
        
        if (lastFreqHz != freqHzInt) {
            ledcChangeFrequency(TMC_STEP_PIN, freqHzInt, 8);
            ledcWrite(TMC_STEP_PIN, 128); // 50% duty cycle
            lastFreqHz = freqHzInt;
        }
    }
  }

  // Handle background OTA requests
  ArduinoOTA.handle();

  // Handle incoming HTTP client requests
  server.handleClient();

  // Handle incoming Serial commands
  handleSerialCommands();

  // Periodic Serial output (instant real-time stream)
  unsigned long now = millis();
  unsigned long interval = streamJsonOnly ? serialStreamInterval : (serialStreamInterval < SERIAL_PRINT_INTERVAL_MS ? serialStreamInterval : SERIAL_PRINT_INTERVAL_MS);
  if (now - lastSerialPrintTime >= interval) {
    lastSerialPrintTime = now;
    float currentAngle = encoder.getDegrees();

    if (streamJsonOnly) {
      // Full-telemetry JSON line — matches /status endpoint so the Digital Twin
      // Serial (offline) mode can populate every panel field identically to Wi-Fi mode.
      float cumAngle   = encoder.getCumulativeDegrees();
      uint16_t rawTick = encoder.readRawAngle();
      AS5600_Status st = encoder.getStatus();
      Serial.printf(
        "{\"joint1\":%.2f,\"speed_rpm\":%.2f,\"cmd_rpm\":%.2f,\"dir_pin\":%d,\"cumulative\":%.2f,\"raw\":%u,\"agc\":%u,"
        "\"magnet_detected\":%s,\"magnet_weak\":%s,\"magnet_strong\":%s,"
        "\"status\":\"%s\",\"rssi\":%d}\n",
        currentAngle, current_speed_rpm, current_cmd_rpm, digitalRead(TMC_DIR_PIN), cumAngle, rawTick, st.agc,
        st.magnetDetected  ? "true" : "false",
        st.magnetTooWeak   ? "true" : "false",
        st.magnetTooStrong ? "true" : "false",
        st.statusText.c_str(),
        WiFi.RSSI()
      );
    } else {
      // Human-readable debug output
      Serial.printf("[Joint 1] Angle: %6.2f deg | CmdRPM: %6.2f | DIR_PIN: %d\n", 
                    currentAngle, current_cmd_rpm, digitalRead(TMC_DIR_PIN));
    }
    Serial.flush();
  }
}
