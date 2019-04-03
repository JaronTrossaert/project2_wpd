// Include Libraries
#include "Arduino.h"
#include "DS18B20.h"


// Pin Definitions
#define DS18B20WP_PIN_DQ	5 // Temperature sensor Digital output to Arduino Digital Input 1 (pin number 5 in the IDE)
#define SensorPin A0  // pH sensor Analog output to Arduino Analog Input 0
#define Offset -2.07 // Deviation compensation for the pH sensor
#define samplingInterval 20 //
#define printInterval 800 //
#define ArrayLength 40  // 


// Global variables and defines

// Object initialization
DS18B20 ds18b20wp(DS18B20WP_PIN_DQ);

// Define vars for pH sensor
int pHArray[ArrayLength]; // Store the average value of the pH sensor feedback
int pHArrayIndex = 0;

// Define vars for testing menu
const int timeout = 60000;  // Define timeout of 60 sec
char menuOption = 0;
long time0;

// Setup the essentials for the circuit to work
void setup()
{
  // Setup Serial which is useful for debugging
  // Use the Serial Monitor to view printed messages
  delay(10000);
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect. Needed for native USB
  Serial.println("Waterproof Project");
}

// Main logic of the circuit. It defines the interaction between the components. After setup, it runs over and over again, in an eternal loop.
void loop()
{
  if (menuOption == '1') {
    // DS18B20 1-Wire Temperature Sensor - Test Code
    // Temperature sensor value in Celsius. For Fahrenheit, use ds18b20wp.ReadTempF()
    float ds18b20wpTempC = ds18b20wp.readTempC();
    Serial.print(F("Temp (°C): "));
    Serial.println(ds18b20wpTempC);

  }

  if (menuOption == '2') {
    // SEN0161 pH Sensor - Test Code
    // pH sensor value
    static unsigned long samplingTime = millis();
    static unsigned long printTime = millis();
    static float pHValue, voltage;
    if (millis() - samplingTime > samplingInterval)
    {
      pHArray[pHArrayIndex++] = analogRead(SensorPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      voltage = averagearray(pHArray, ArrayLength) * 5.0 / 1024;
      pHValue = 3.5 * voltage + Offset;
      samplingTime = millis();
    }
    if (millis() - printTime > printInterval)
    {
      Serial.print("pH value: ");
      Serial.println(pHValue, 2);
      printTime = millis();
    }
  }

  if (menuOption == '3') {
    static unsigned long samplingTime = millis();
    static unsigned long printTime = millis();
    static float pHValue, voltage;
    float ds18b20wpTempC = ds18b20wp.readTempC();
    if (millis() - samplingTime > samplingInterval)
    {
      pHArray[pHArrayIndex++] = analogRead(SensorPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      voltage = averagearray(pHArray, ArrayLength) * 5.0 / 1024;
      pHValue = 3.5 * voltage + Offset;
      samplingTime = millis();
    }
    if (millis() - printTime > printInterval)
    {
      Serial.print(F("Temp (°C): "));
      Serial.println(ds18b20wpTempC);
      Serial.print("pH value: ");
      Serial.println(pHValue, 2);
      Serial.println(millis());
      printTime = millis();
    }
  }

  if (millis() - time0 > timeout)
  {
    Serial.println(F("test"));
    menuOption = menu();
  }

}

double averagearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to average!/n");
    return 0;
  }
  if (number < 5) { // Less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;
          max = arr[i];
        } else {
          amount += arr[i];
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}

// Menu function for selecting the components to be tested
// Follow serial monitor for instructions
char menu()
{
  Serial.println(F("\nWhich component would you like to test?"));
  Serial.println(F("(1) DS18B20 1-Wire Temperature Sensor"));
  Serial.println(F("(2) SEN0161 pH Sensor"));
  Serial.println(F("(3) All the components"));
  Serial.println(F("(menu) send anything else or press on board reset button\n"));
  while (!Serial.available());

  // Read data from serial monitor if received
  while (Serial.available())
  {
    char c = Serial.read();
    if (isAlphaNumeric(c))
    {
      if (c == '1') {
        Serial.println(F("Now testing DS18B20 1-Wire Temperature Sensor"));
      }
      else if (c == '2') {
        Serial.println(F("Now testing SEN0161 pH Sensor"));
      }
      else if (c == '3') {
        Serial.println(F("Now testing all the components"));
      }
      else
      {
        Serial.println(F("illegal input!"));
        return 0;
      }
      time0 = millis();
      return c;
    }
  }
}
