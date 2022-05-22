#include "Newtune595.h"


//Constants
#define number_of_74hc595s 2
#define numOfoutPins 8
#define dataPin 25 //DS or SER or data
#define clockPin 33 //SH_CP or RCLK or clock
#define latchPin 32 //ST_CP or SRCLK or latch
//Variables
byte registers[number_of_74hc595s];
int location;
int pinnumber;
int setvalue;

void setup_595() 
{
//Serial.begin(115200);

pinMode(latchPin, OUTPUT);
pinMode(dataPin, OUTPUT); 
pinMode(clockPin, OUTPUT);



clear();
updateShiftRegister();

}

/*
void loop() 
{

mytest();
}
*/


void updateShiftRegister()
{ 
    digitalWrite(latchPin, LOW);
    for(int i = number_of_74hc595s-1; i >= 0; i--)
    { 
        shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);
    }
    digitalWrite(latchPin, HIGH);
    delay(100);
}

void clear()
{
    for(int j = 0; j < number_of_74hc595s; j++)
    {
        registers[j]=0; 
    }
    printpinval(); 
    //delay(100);
    //updateShiftRegister();
    //delay(100);
    Serial.println("clear completed");
}

void setm(int location,int pinnumber,int setvalue)
{
    Serial.println(setvalue);
    Serial.println(location);

    if(setvalue==1){
        Serial.println("insidestva");
        bitSet(registers[location], pinnumber);
    } else {
        bitClear(registers[location], pinnumber);
    }
    printpinval();
    Serial.println("bit set");
}

void printpinval(){
    for(int j = 0; j < number_of_74hc595s; j++)
    { 
        for (int i = 0; i < numOfoutPins; i++) {
            bool b = bitRead(registers[j], i);
            Serial.print(b);
        }
    } 
    Serial.println(" ");
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

