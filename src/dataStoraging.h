#include <SPIFFS.h>
#include <ArduinoJson.h>

bool storeConfigurationData(int fbYear, int fbMonth, int fbDay, int fbHour, int fbMinute, int fbSecond, int fbCycle, int fbDuration);
void loadAndPrintConfiguration();