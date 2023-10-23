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

// //outputs a number between 0 and 6: 0->Monday, 1->Tuesday, ..., 6->Sunday
// int getWeekday(int day, int month, int year){
//   enum {January, February, March, April, May, June, July, August, September, Oktober, November, December};
//   int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//   // 01.01.2000 was a saturday
//   int weekday = 5;
//   weekday += day-1;
//   for (int i =  1; i<month; i++){
//     weekday += daysInMonth[i];
//   }
//   if(isLeapYear(year) && month > 2) weekday++;
//   for(int i = 2000; i<year; i++){
//     weekday += isLeapYear(i)?366:365;
//   }
//   return weekday%7;
// }

// // calculates if the given year is a leap year or not
// bool isLeapYear(int year){
//   if(year%4 != 0) return false;
//   if(year%100 != 0) return true;
//   if(year%400 != 0) return false;
//   return true;
// }
