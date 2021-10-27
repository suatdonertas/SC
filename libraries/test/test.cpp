#include "test.h"
Configuration::Configuration(const char* filename)
{
  jsonFile = filename;
}

bool Configuration::load() {
  SPIFFS.begin();
  File configFile = SPIFFS.open(jsonFile, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
  }

  size_t size = configFile.size();
  char buf[size + 1];
  configFile.readBytes(buf, size);

  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _jsonData = _jsonBuffer.parseObject(buf);

  if (!_jsonData.success()) {
    Serial.println("Failed to parse config file");
  };

  for (JsonObject::iterator it = _jsonData.begin(); it != _jsonData.end(); ++it)
  {
    const char* key = it->key;
    auto value = it->value.as<char*>();
    char* cfgkey = _jsonBuffer.strdup(key);
    char* cfgvalue = _jsonBuffer.strdup(value);
    this->put(cfgkey, cfgvalue);
  }

  configFile.close();
}

bool Configuration::save() {
  File configFile = SPIFFS.open(jsonFile, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  if (configuration.empty())
  {
    Serial.println("Configuration empty");
  };
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _jsonData = _jsonBuffer.createObject();

  for (auto const& x : configuration)
  {
    _jsonData.set(_jsonBuffer.strdup(x.first), _jsonBuffer.strdup(x.second));
  }

  _jsonData.printTo(configFile);

  configFile.close();
  return true;
}

bool Configuration::put(const char* key, char* value) {
  configuration[key] = value;
}

bool Configuration::dump() {
  for (const auto &p : configuration) {
    Serial.print( "configuration[");
    Serial.print(p.first);
    Serial.print("] = ");
    Serial.println(p.second);
  };
}