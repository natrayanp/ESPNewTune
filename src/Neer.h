#ifndef Neer_h
#define Neer_h


#include <functional>
#include <Arduino.h>
#include "ESPAsyncWebServer.h"	
//#include <json.hpp>
#include <cJSON.h>
#include <LittleFS.h>
#include <NatStorage.h>




//using json = nlohmann::json;

//int const MOTOR_PINS[] = {1,2};




//Constants



class NeerClass {
    private:
        //Variables
        /*
        json pinconfig;
        json planconfig;
        json scheduleconfig;
        json checksconfig;
        json status;
        */
        cJSON *_checksconfig,*_planconfig,*_scheduleconfig,*_pinconfig;
        AsyncWebServer* _server = nullptr;
        //Check if we have 3 phase and update status
        bool check3phase();
        void loadconfig();
        //void runschedule(void *parameter);
        void doit(char *text);
        void dofile(char *filename);
        

    public:
        //Constructor function        
        //NeerClass();
        void begin(AsyncWebServer* server);
        //void executeschedule(String schedulename);
};

extern NeerClass Neer;
#endif









/*

void mytest(){

clear();
setm(0,0,1); 
updateShiftRegister(); 
delay(1000);

clear();
setm(0,1,1); 
updateShiftRegister(); 
delay(1000);

clear();
setm(0,0,1); 
setm(0,1,1); 
updateShiftRegister(); 
delay(1000);

clear();
setm(1,0,1); 
updateShiftRegister(); 
delay(1000);


setm(1,7,1); 
updateShiftRegister(); 
delay(1000);

setm(0,0,1); 
setm(0,1,1); 
updateShiftRegister(); 
delay(1000);


clear();
updateShiftRegister(); 
delay(1000);

}
*/

