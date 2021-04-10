#pragma once
#include <map>
#include <Arduino.h>
typedef void (*EventFunc)(String key, String value);
std::map < String, EventFunc > events;

void addEvent(String key, EventFunc func){
    events.insert(std::pair< String, EventFunc >(key,func));
}
void execEvent(String key, String val){
    for (auto const& event : events)
    {
        if (event.first == key && event.second != NULL){
            event.second(key, val);
        }

    }
}