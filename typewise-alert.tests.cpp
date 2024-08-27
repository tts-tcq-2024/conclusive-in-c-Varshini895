#include <gtest/gtest.h>
#include "typewise-alert.h"
TEST(TypeWiseAlertTestSuite, InfersBreachTooLow) {
    TemperatureRange range = {0, 35};  // lower limit 0, upper limit 35
    EXPECT_EQ(inferBreach(-1, range), TOO_LOW);
}
TEST(TypeWiseAlertTestSuite, InfersBreachTooHigh) {
    TemperatureRange range = {0, 35};
    EXPECT_EQ(inferBreach(36, range), TOO_HIGH);
}
TEST(TypeWiseAlertTestSuite, InfersBreachNormal) {
    TemperatureRange range = {0, 35};
    EXPECT_EQ(inferBreach(20, range), NORMAL);
}
TEST(TypeWiseAlertTestSuite, ClassifyTemperatureBreachPassiveCooling) {
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 35), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 36), TOO_HIGH);
}
TEST(TypeWiseAlertTestSuite, ClassifyTemperatureBreachHiActiveCooling) {
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 45), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 46), TOO_HIGH);
}
TEST(TypeWiseAlertTestSuite, ClassifyTemperatureBreachMedActiveCooling) {
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 40), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 41), TOO_HIGH);
}
TEST(TypeWiseAlertTestSuite, SendsLowTemperatureEmail) {
    std::string output = captureOutput([](){ sendToEmail(TOO_LOW); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too low\n");
}
TEST(TypeWiseAlertTestSuite, SendsHighTemperatureEmail) {
    std::string output = captureOutput([](){ sendToEmail(TOO_HIGH); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too high\n");
}
TEST(TypeWiseAlertTestSuite, SendsNoEmailForNormalTemperature) {
    std::string output = captureOutput([](){ sendToEmail(NORMAL); });
    EXPECT_EQ(output, "");
}
TEST(TypeWiseAlertTestSuite, SendsTooLowToController) {
    std::string output = captureOutput([](){ sendToController(TOO_LOW); });
    EXPECT_EQ(output, "feed : 1\n");
}
TEST(TypeWiseAlertTestSuite, SendsTooHighToController) {
    std::string output = captureOutput([](){ sendToController(TOO_HIGH); });
    EXPECT_EQ(output, "feed : 2\n");
}
TEST(TypeWiseAlertTestSuite, SendsNormalToController) {
    std::string output = captureOutput([](){ sendToController(NORMAL); });
    EXPECT_EQ(output, "feed : 0\n");
}
TEST(TypeWiseAlertTestSuite, GetsTemperatureRange) {
    TemperatureRange range;

    range = getTemperatureRange(PASSIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 35);

    range = getTemperatureRange(HI_ACTIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 45);

    range = getTemperatureRange(MED_ACTIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 40);
}
TEST(TypeWiseAlertTestSuite, GetsTemperatureRangeInvalidCoolingType) {
    TemperatureRange range = getTemperatureRange(static_cast<CoolingType>(999));
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 0); // Assuming invalid CoolingType returns {0, 0}
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToControllerHigh) {
    BatteryCharacter batteryChar = {PASSIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_CONTROLLER, batteryChar, 36); });
    EXPECT_EQ(output, "feed : 2\n");
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToControllerLow) {
    BatteryCharacter batteryChar = {PASSIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_CONTROLLER, batteryChar, -1); });
    EXPECT_EQ(output, "feed : 1\n");
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToControllerNormal) {
    BatteryCharacter batteryChar = {PASSIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_CONTROLLER, batteryChar, 30); });
    EXPECT_EQ(output, "feed : 0\n");
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToEmailLow) {
    BatteryCharacter batteryChar = {PASSIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_EMAIL, batteryChar, -5); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too low\n");
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToEmailHigh) {
    BatteryCharacter batteryChar = {HI_ACTIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_EMAIL, batteryChar, 50); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too high\n");
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertToEmailNormal) {
    BatteryCharacter batteryChar = {MED_ACTIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(TO_EMAIL, batteryChar, 30); });
    EXPECT_EQ(output, "");  // No output for NORMAL
}
TEST(TypeWiseAlertTestSuite, CheckAndAlertInvalidTarget) {
    BatteryCharacter batteryChar = {HI_ACTIVE_COOLING, "BrandX"};
    std::string output = captureOutput([&](){ checkAndAlert(static_cast<AlertTarget>(999), batteryChar, 50); });
    EXPECT_EQ(output, ""); // Ensure function handles invalid AlertTarget gracefully
}
