#ifndef Newtune595_h
#define Newtune595_h


#include <functional>
#include <Arduino.h>

//Constants
#define number_of_74hc595s 2
#define numOfoutPins 8
#define dataPin 25 //DS or SER or data
#define clockPin 33 //SH_CP or RCLK or clock
#define latchPin 32 //ST_CP or SRCLK or latch


class ESPNewTuneClass {
    private:
        //Variables
        byte registers[number_of_74hc595s];
        int location;
        int pinnumber;
        int setvalue;

        void updateShiftRegister();
        void clear();
        void printpinval();

    public:
        void setup_595();
        void setm(int location,int pinnumber,int setvalue,bool clear_befreset=false, bool updtregis_aftreset=false, unsigned long deytime_aftreset=0);
}













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

