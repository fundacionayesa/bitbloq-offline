#ifndef Robin_h
#define Robin_h

#include <Servo.h>
#include "BitbloqStepper.h"
#include "BitbloqOscillator.h"
#include "BitbloqUS.h"
#include "robin_sounds.h"



//-- Constants
#define HEAD_TRIM    80
#define HEAD_LEFT    45
#define HEAD_CENTER  0
#define HEAD_RIGHT   -45

#define LEFT         1
#define RIGHT        -1

#define STOP         0
#define LOW_SPEED    5
#define MEDIUM_SPEED 7
#define HIGH_SPEED   8
// #define SPR          4096 // Steeps per revolution
#define SPR          2000
#define Saat         3 //Steeps at a time

//-- Pins
#define PIN_WR1      0 //wheelL[0]
#define PIN_WR2      1 //wheelL[1]
#define PIN_WR3      2 //wheelL[2]
#define PIN_WR4      3 //wheelL[3]

#define PIN_WL1      4 //wheelR[0]
#define PIN_WL2      5 //wheelR[1]
#define PIN_WL3      6 //wheelR[2]
#define PIN_WL4      7 //wheelR[3]

#define PIN_LFL      8 //Line Sensor Left
#define PIN_LFR      9 //Line Sensor Right
#define PIN_Head     10//Head Servo
#define PIN_Buzzer   11

#define PIN_Trigger  13
#define PIN_Echo     12



class Robin
{
  public:
    //-- Robin inizialization
    void init(int Head=PIN_Head, int LFL=PIN_LFL, int LFR=PIN_LFR, int Buzzer=PIN_Buzzer, int USTrigger=PIN_Trigger, int USEcho=PIN_Echo);

    void check();
    
    //-- Attach & detach functions

    void attachHead();
    void detachHead();

    //-- Predetermined Motion Functions
    void turnHead( int head_target);
    void oscillateHead(int A, int O, int T, double phase_diff, float cycles=1);
    void move(int WL, int WR);

    //-- HOME = Robin at rest position
    void home();
    bool getRestWheelState();
    void setRestWheelState(bool state);
    bool getRestHeadState();
    void setRestHeadState(bool state);

    //-- Predetermined Motion Functions
    void stop();
    void fordward(int V=Saat);
    void backward(int V=Saat);
    void right(int V=Saat);
    void left(int V=Saat);

    //-- Sensor functions
    long getDistance(); //US sensor
    int getLine(int side); //LineFollower sensors

    //-- Sounds
    // void _tone (unsigned int noteFrequency, int noteDuration, int silentDuration);
    void _tone (unsigned int noteFrequency, int noteDuration);
    void sim_tone(unsigned int note_Frecuency, int noteDuration, int silentDuration);
    void bendTones (float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration);
  	void playMelody(struct melody melod);

    Robin() : WheelL(SPR,PIN_WL1,PIN_WL2,PIN_WL3,PIN_WL4), WheelR(SPR,PIN_WR1,PIN_WR2,PIN_WR3,PIN_WR4){};
    Stepper WheelL;
    Stepper WheelR;

  private:

    
    Oscillator head;
    US us;

    int head_pin;
    int head_position;
    int pinBuzzer;
    int pinLFL;
    int pinLFR;

    int flag_t=0;
    int flag_m=0;
    unsigned long pt_t;
    int pt_m;
    int i_m=0;
    int note, dur, sil;
    struct melody melodia;

    long p_dist;
    unsigned long t_dist=0;
    

    unsigned long final_time;
    unsigned long partial_time;
    float increment;

    //bool isRobinResting;
    bool isRobinHeadResting;
    bool isRobinWheelResting;


    int parseV(int V, int low, int medium, int high);

     
};

#endif
