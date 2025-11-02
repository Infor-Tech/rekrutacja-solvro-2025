#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

// Configuration
const char *WIFI_SSID = "YOUR_WIFI_SSID";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char *DISCORD_WEBHOOK_URL = "YOUR_DISCORD_WEBHOOK_URL_HERE";

// Thresholds for Alert Triggering (Used for Webhook activation)
const int MQ135_THRESHOLD = 500; // Calibrate this value
const int CO2_THRESHOLD = 1000;  // Standard value, no need to change

const long CHECK_INTERVAL_MS = 300000;
long lastCheckTime = 0;
bool alertSent = false;

const int MQ135_PIN = A0;

SCD4x scd4x;

void connectToWiFi();
float calculateIAQI(uint16_t co2, int mqReading);
String getIAQICategory(float iaqi);
String getHealthImpactDescription(float iaqi);
void sendDiscordNotification(float temp, float humidity, int mqReading, uint16_t co2Reading, float iaqi);
void initializeSCD40();

/**
 * Calculates a sub-score (0-100) for CO2, where 100 is best (low PPM).
 * Uses common indoor air quality metrics.
 */
float getCO2Score(uint16_t co2)
{
  if (co2 <= 400)
    return 100.0f;
  if (co2 <= 800)
    return 100.0f - (co2 - 400) * (50.0f / 400.0f);
  if (co2 <= 1500)
    return 50.0f - (co2 - 800) * (50.0f / 700.0f);
  return 0.0f;
}

/**
 * Calculates a sub-score (0-100) for MQ-135 reading, where 100 is best (low analog reading).
 * MQ-135 Analog (0-1023): <300 (Max 100), 300-600 (Linear drop), >600 (Min 0)
 */
float getMQ135Score(int mqReading)
{
  if (mqReading <= 300)
    return 100.0f;
  if (mqReading <= 600)
    return 100.0f - (mqReading - 300) * (100.0f / 300.0f); // 100 to 0
  return 0.0f;
}

/**
 * Calculates the final composite IAQI (Indoor Air Quality Index) from 0 to 100.
 * Higher score is better. Uses a weighted average.
 */
float calculateIAQI(uint16_t co2, int mqReading)
{
  float co2Score = getCO2Score(co2);
  float mqScore = getMQ135Score(mqReading);

  return (co2Score * 0.6f) + (mqScore * 0.4f);
}

/**
 * Returns a descriptive category based on the IAQI score.
 */
String getIAQICategory(float iaqi)
{
  if (iaqi >= 80.0f)
    return "Excellent (No action needed)";
  if (iaqi >= 60.0f)
    return "Good (Monitor)";
  if (iaqi >= 40.0f)
    return "Moderate (Consider ventilating)";
  if (iaqi >= 20.0f)
    return "Poor (Ventilation highly recommended)";
  return "Hazardous (Immediate ventilation required)";
}

/**
 * Returns a detailed description of health effects based on IAQI score.
 */
String getHealthImpactDescription(float iaqi)
{
  if (iaqi >= 80.0f)
  {
    return "Air quality is **Excellent**. The environment is ideal with no known health risks.";
  }
  else if (iaqi >= 60.0f)
  {
    return "Air quality is **Good**. The environment is acceptable, though minor symptoms like subtle stuffiness might be noted by highly sensitive individuals.";
  }
  else if (iaqi >= 40.0f)
  {
    return "Air quality is **Moderate**. Reduced mental performance, lethargy, and increased risk of symptoms for individuals with asthma or allergies. Ventilation is advised.";
  }
  else if (iaqi >= 20.0f)
  {
    return "Air quality is **Poor**. This level impacts concentration, mental clarity, and sleep quality. Headaches, drowsiness, and increased heart rate are possible. **Ventilation is crucial now.**";
  }
  else
  {
    return "Air quality is **Hazardous**. Severe health risks are possible, including dizziness, nausea, and serious cognitive impairment. **Immediate and aggressive ventilation is required.**";
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("\n--- Indoor Air Quality Monitor ---");

  Wire.begin();
  initializeSCD40();
  pinMode(MQ135_PIN, INPUT);

  connectToWiFi();
}

void loop()
{
  if (millis() - lastCheckTime >= CHECK_INTERVAL_MS)
  {
    lastCheckTime = millis();

    int mqReading = analogRead(MQ135_PIN);

    uint16_t co2 = 0;
    float temperature_C = NAN;
    float humidity = NAN;

    // The SCD40 only returns new data every ~5 seconds. Check for availability.
    if (scd4x.readMeasurement())
    {
      co2 = scd4x.getCO2();
      temperature_C = scd4x.getTemperature();
      humidity = scd4x.getHumidity();
    }
    else
    {
      Serial.println("SCD40: Waiting for new data...");
    }

    float iaqi = calculateIAQI(co2, mqReading);
    String iaqiCategory = getIAQICategory(iaqi);

    Serial.println("----------------------------------------");
    Serial.printf("IAQI Score: %.1f/100 (%s)\n", iaqi, iaqiCategory.c_str());
    Serial.printf("CO2 Reading (SCD40): %d PPM (Threshold: %d PPM)\n", co2, CO2_THRESHOLD);
    Serial.printf("MQ-135 Gas Reading (Analog): %d (Threshold: %d)\n", mqReading, MQ135_THRESHOLD);

    if (isnan(temperature_C))
    {
      Serial.println("Environmental Data (SCD40): N/A (Sensor Error)");
    }
    else
    {
      Serial.printf("Temp (SCD40): %.2f C | Humidity (SCD40): %.2f %%\n", temperature_C, humidity);
    }

    // Air Quality Check - Trigger if EITHER CO2 or MQ-135 is high (or IAQI is poor)
    bool isAirQualityBad = (co2 > CO2_THRESHOLD) || (mqReading >= MQ135_THRESHOLD) || (iaqi < 40.0f);

    if (isAirQualityBad)
    {
      Serial.println("\n!!! AIR QUALITY ALERT !!!");
      Serial.println("Poor ventilation detected. Opening window recommended.");

      // Send notification only if it hasn't been sent since the last good reading
      if (!alertSent)
      {
        sendDiscordNotification(temperature_C, humidity, mqReading, co2, iaqi);
        alertSent = true;
      }
      else
      {
        Serial.println("Alert already sent. Waiting for air quality to improve...");
      }
    }
    else
    {
      Serial.println("Air Quality: GOOD. All readings are within acceptable limits.");
      alertSent = false; // Reset the alert flag once air quality is good again
    }
  }
}

void initializeSCD40()
{
  if (scd4x.begin() == false)
  {
    Serial.println("SCD40 not found. Check wiring!");
  }
  else
  {
    scd4x.startPeriodicMeasurement();
  }
}

void connectToWiFi()
{
  Serial.printf("Connecting to %s\n", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected.");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  }
  else
  {
    Serial.println("\nFailed to connect to WiFi. Running in offline mode.");
  }
}

void sendDiscordNotification(float temp, float humidity, int mqReading, uint16_t co2Reading, float iaqi)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Cannot send Discord notification: WiFi is not connected.");
      return;
    }
  }

  String healthDescription = getHealthImpactDescription(iaqi);
  String iaqiCategory = getIAQICategory(iaqi);

  String embedTitle;
  int embedColor; // Discord color in decimal (e.g., 3066993 for Green, 15158332 for Red)

  if (iaqi >= 60.0f)
  {
    embedTitle = "Air Quality Moderate: Check Readings";
    embedColor = 16763904; // Yellow/Orange
  }
  else
  {
    embedTitle = "Critical Alert: Open Windows Immediately!";
    embedColor = 15158332; // Red
  }

  // Sensor data for the Discord embed
  char fields[512];
  snprintf(fields, sizeof(fields),
           "**IAQI Score:** `%.1f/100`\n"
           "**CO2 Concentration:** `%d PPM`\n"
           "**Gas Reading (MQ-135):** `%d`\n"
           "**Temperature (SCD40):** `%.1f °C`\n"
           "**Humidity (SCD40):** `%.1f %%`\n"
           "**Device IP:** `%s`",
           iaqi,
           co2Reading,
           mqReading,
           temp,
           humidity,
           WiFi.localIP().toString().c_str());

  // JSON payload for the Discord Webhook
  String jsonPayload = "{\"embeds\": [";
  jsonPayload += "{";
  jsonPayload += "\"title\": \"" + embedTitle + "\",";
  jsonPayload += "\"description\": \"**Air Quality Index Category:** **" + iaqiCategory + "**\\n\\n**Health Impact:** " + healthDescription + "\",";
  jsonPayload += "\"color\": " + String(embedColor) + ",";
  jsonPayload += "\"fields\": [";
  jsonPayload += "{\"name\": \"Current Sensor Readings\", \"value\": \"" + String(fields) + "\"}";
  jsonPayload += "],";
  jsonPayload += "\"footer\": {\"text\": \"Air Monitor Alert System | Check Interval: 5 minutes\"}";
  jsonPayload += "}";
  jsonPayload += "]}";

  // Send the POST request
  HTTPClient http;
  http.begin(DISCORD_WEBHOOK_URL);
  http.addHeader("Content-Type", "application/json");

  Serial.print("Sending Discord notification... ");
  int httpCode = http.POST(jsonPayload);

  if (httpCode > 0)
  {
    Serial.printf("HTTP Code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_NO_CONTENT)
    {
      Serial.println("Discord Notification sent successfully!");
    }
    else
    {
      Serial.printf("Webhook failed with response: %s\n", http.getString().c_str());
    }
  }
  else
  {
    Serial.printf("HTTP Request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}