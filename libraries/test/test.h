#ifndef test_h
#define test_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include "FS.h"
#include "spiffs.h"
#include "map"
class Configuration {
  public:
    Configuration(const char* filename);
    bool load();
    bool save();
    bool dump();
    bool put(const char* key, char* value);
    struct cmp_str
    {
      bool operator()(char const *a, char const *b)
      {
        return strcmp(a, b) < 0;
      }
    };
    std::map<const char*, const char*, cmp_str> configuration;

  private:
    const char* jsonFile;

};
#endif
