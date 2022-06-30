#ifndef Neer_h
#define Neer_h


#include <functional>
#include <Arduino.h>
#include "ESPAsyncWebServer.h"	
//#include <json.hpp>
#include <cJSON.h>
#include <LittleFS.h>
#include <NatStorage.h>

#include <ShiftRegister74HC595.h>

int numberOfShiftRegisters = 1; // number of shift registers attached in series
int serialDataPin = 0; // DS
int clockPin = 1; // SHCP
int latchPin = 2; // STCP



//using json = nlohmann::json;

//int const MOTOR_PINS[] = {1,2};



//Constants
define ALLOWED_TASKS 3


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
        bool forcestop = false;
        bool isfianlstep = false;
        TaskHandle_t* xHandle[ALLOWED_TASKS];
        //void runschedule(void *parameter);
        //void doit(char *text);
        //void dofile(char *filename);
        
        ShiftRegister74HC595<numberOfShiftRegisters> sr(serialDataPin, clockPin, latchPin);
        

    public:

        struct threaditem{
            char *itemtype;           // 'S' - Schedule; 'P' - Plan
            char *itemname;           // name of the schedule or Plan
            int  arraylocation;       // tasks number (to identify it in xHandle array)
        } threadit;

        struct taskitem{
            char *previousplan;
            char *currentplan;      // name of the schedule or Plan
            char *previousplanrunseq; // Run previous instruction; B = Before current plan; A = After current plan
            bool isfirstplan;
            bool islastplan;     
            char *runtype;
            cJSON *planitem;
            cJSON *stepsitem;
        } taskit;

        struct scheduleitem{
            char *name;
            bool isplan;
        } schit;

        //Constructor function        
        //NeerClass();
        void begin(AsyncWebServer* server);
        void begintask();
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

