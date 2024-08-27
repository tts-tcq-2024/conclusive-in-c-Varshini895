#include "typewise-alert.h"
#include <stdio.h>

// Function to get the temperature range based on cooling type
TemperatureRange getTemperatureRange(CoolingType coolingType) {
  switch (coolingType) {
    case PASSIVE_COOLING:
      return (TemperatureRange){0, 35};
    case HI_ACTIVE_COOLING:
      return (TemperatureRange){0, 45};
    case MED_ACTIVE_COOLING:
      return (TemperatureRange){0, 40};
    default:
      // Handle invalid coolingType with an error range
      return (TemperatureRange){0, 0}; 
  }
}

// Function to infer breach type based on value and limits
BreachType inferBreach(double value, TemperatureRange range) {
  if (value < range.lowerLimit) {
    return TOO_LOW;
  }
  if (value > range.upperLimit) {
    return TOO_HIGH;
  }
  return NORMAL;
}

// Function to classify temperature breach based on cooling type and temperature
BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
  TemperatureRange range = getTemperatureRange(coolingType);
  return inferBreach(temperatureInC, range);
}

void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {
  BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);

  switch(alertTarget) {
    case TO_CONTROLLER:
      sendToController(breachType);
      break;
    case TO_EMAIL:
      sendToEmail(breachType);
      break;
    default:
      // Handle invalid alert target, potentially with a log or error message
      printf("Invalid alert target!\n");
      break;
  }
}

void sendToController(BreachType breachType) {
  const unsigned short header = 0xfeed;
  printf("%04x : %d\n", header, breachType);
}

void sendToEmail(BreachType breachType) {
  const char* recepient = "a.b@c.com";
  printf("To: %s\n", recepient);

  switch(breachType) {
    case TOO_LOW:
      printf("Hi, the temperature is too low\n");
      break;
    case TOO_HIGH:
      printf("Hi, the temperature is too high\n");
      break;
    case NORMAL:
      printf("Hi, the temperature is normal\n");
      break;
    default:
      printf("Hi, there is an unknown error\n");
      break;
  }
}
