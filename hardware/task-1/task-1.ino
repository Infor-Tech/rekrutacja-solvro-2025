#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

#define LED_PIN LED_BUILTIN
#define BUZZER_PIN 5
#define LEVEL_SENSOR_PIN_SIGNAL A5
#define LEVEL_SENSOR_PIN_POWER 4
#define TOLERANCE 20

int baselineLevel = 0;
volatile bool wdt_flag = false;

void systemSleep();
void checkSolnikLevel();

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LEVEL_SENSOR_PIN_POWER, OUTPUT);
    pinMode(LEVEL_SENSOR_PIN_SIGNAL, INPUT);

    digitalWrite(LEVEL_SENSOR_PIN_POWER, HIGH);
    delay(10); // Allow sensor to stabilize, no need to use millis() here
    baselineLevel = analogRead(LEVEL_SENSOR_PIN_SIGNAL);
    digitalWrite(LEVEL_SENSOR_PIN_POWER, LOW);

    Serial.begin(9600);

    power_all_disable();
    power_timer1_enable();
}

void loop()
{
    if (wdt_flag)
    {
        wdt_flag = false;
        power_usart0_enable();
        power_adc_enable();
        checkSolnikLevel();
        power_usart0_disable();
        power_adc_disable();
    }
    systemSleep();
}

ISR(WDT_vect)
{
    wdt_flag = true;
}

void systemSleep()
{
    wdt_reset();
    wdt_enable(WDTO_8S);
    WDTCSR |= (1 << WDIE);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    noInterrupts();
    sleep_enable();
    interrupts();
    sleep_cpu();

    sleep_disable();

    wdt_disable();
}

void checkSolnikLevel()
{
    digitalWrite(LEVEL_SENSOR_PIN_POWER, HIGH);
    delay(10); // Allow sensor to stabilize, no need to use millis() here

    int currentLevel = analogRead(LEVEL_SENSOR_PIN_SIGNAL);

    if (abs(currentLevel - baselineLevel) > TOLERANCE)
    {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        Serial.print("Solnik level lower than baseline: ");
        Serial.print(currentLevel);
        Serial.print("/");
        Serial.println(baselineLevel);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }
    digitalWrite(LEVEL_SENSOR_PIN_POWER, LOW);
}