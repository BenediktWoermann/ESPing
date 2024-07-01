#include "util.h"

#include <Arduino.h>

// if no category is required (for general stats) just put in "{" for category, because it is the first sign of the json returned by the api
char* getComponentFromJson(char* json, char const * identifier, char const * category){
    if(!json || !identifier || !category) return NULL;
    Serial.println(json);
    //    searches for section of information for specified brawler
    //    char* relevantInfo = strstr(json, brawlername);
    //    searches for the specified identifier
    char *identifierStart = strstr(strstr(json, category), identifier);
    
    char *valueStart = identifierStart + 2 + strlen(identifier);
    //    int valueStartIndex = valueStart[0];
    //    int valueEndIndex = strchr(valueStart, ',')[0];
    int valueLength = strchr(valueStart, ',') - valueStart;
    char* value = partOfString(valueStart, 0, valueLength);

    return value;
}

char* partOfString(char* string, int startIndex, int endIndex){
  if(startIndex < 0 || startIndex >= strlen(string)) startIndex = 0;
  if(endIndex < 0 || endIndex >strlen(string)) endIndex = strlen(string);
  char* part = (char*) malloc(sizeof(char) * (endIndex-startIndex+1));
  memcpy(part, string+startIndex, endIndex-startIndex);
  part[endIndex-startIndex] = '\0';
  return part; 
}