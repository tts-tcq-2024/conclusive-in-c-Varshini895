#pragma once

typedef enum {
  PASSIVE_COOLING,     // Passive cooling method
  HI_ACTIVE_COOLING,   // High active cooling method
  MED_ACTIVE_COOLING   // Medium active cooling method
} CoolingType;

typedef enum {
  NORMAL,    // Temperature is within the normal range
  TOO_LOW,   // Temperature is below the lower limit
  TOO_HIGH   // Temperature is above the upper limit
} BreachType;

typedef struct {
  int lowerLimit;    // Lower temperature limit
  int upperLimit;    // Upper temperature limit
} TemperatureRange;

BreachType inferBreach(double value, TemperatureRange range);

// Function to classify the breach type based on the cooling type and measured temperature
BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC);

// Function to retrieve the temperature range for a specific cooling type
TemperatureRange getTemperatureRange(CoolingType coolingType);

typedef enum {
  TO_CONTROLLER,  // Send alert to the controller
  TO_EMAIL        // Send alert via email
} AlertTarget;

typedef struct {
  CoolingType coolingType;  // Type of cooling used by the battery
  char brand[48];           // Brand name of the battery (48-character array)
} BatteryCharacter;

// Function to check the temperature and send an alert if there is a breach
void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC);

// Function to send the breach type to a controller
void sendToController(BreachType breachType);

// Function to send an email alert with the breach type
void sendToEmail(BreachType breachType);
