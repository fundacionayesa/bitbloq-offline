#include "Arduino.h"

#include "robin.h"

void Robin::init(int Head, int LFL, int LFR, int Buzzer, int USTrigger, int USEcho)
{

  WheelL.setSpeed(HIGH_SPEED);
  WheelR.setSpeed(HIGH_SPEED);

  head_pin = Head;

  attachHead();

  head.SetTrim(HEAD_TRIM - 90);

  head_position = HEAD_CENTER;

  //US sensor init with the pins:
  us.init(USTrigger, USEcho, 15000);

  //Buzzer  sensor pin:
  pinBuzzer = Buzzer;

  pinMode(Buzzer, OUTPUT);

  //LineFollower sensor pins:
  pinLFL = LFL;
  pinLFR = LFR;

  pinMode(LFL, INPUT);
  pinMode(LFR, INPUT);
  //We need all the pins
   Serial.end();
  // Serial.start(9600);
}

void Robin::check()
{
  //  Serial.println("check");
  if (flag_t == 1)
  {
    sim_tone(1, 1, 1);
    //    Serial.println("tone, checked");
  }
  //  if (flag_m == 1) {
  //    playMelody(melodia);
  //  }
}

///////////////////////////////////////////////////////////////////
//-- ATTACH & DETACH FUNCTIONS ----------------------------------//
///////////////////////////////////////////////////////////////////

/* void Robin::detachWheels(){
  wheelL.free();
  wheelR.free();

  } */

void Robin::attachHead()
{
  head.attach(head_pin);
  if (getRestHeadState())
  {
    setRestHeadState(false);
  }
}

void Robin::detachHead()
{
  head.detach();
  if (!getRestHeadState())
  {
    setRestHeadState(true);
  }
}

///////////////////////////////////////////////////////////////////
//-- BASIC MOTION FUNCTIONS -------------------------------------//
///////////////////////////////////////////////////////////////////
void Robin::turnHead(int head_target)
{

  attachHead();

  head.SetPosition(head_target + 90);
  head_position = head_target;
}

void Robin::oscillateHead(int A, int O, int T, double phase_diff, float cycles)
{

  attachHead();

  head.SetA(A);
  head.SetO(O);
  head.SetT(T);
  head.SetPh(phase_diff);

  double ref = millis();
  for (double x = ref; x <= T * cycles + ref; x = millis())
    head.refresh();
}

void Robin::move(int WL, int WR)
{

  WheelL.step(WL);
  WheelR.step(WR);
}

///////////////////////////////////////////////////////////////////
//-- HOME = Robin at rest position --------------------------//
///////////////////////////////////////////////////////////////////
void Robin::home()
{

  stop();

  if (!getRestHeadState())
  {

    turnHead(HEAD_CENTER); //Move the head at default position in half a second

    detachHead();
  }
}

/* bool Robin::getRestWheelState(){

    return isRobinWheelResting;
  }

  void Robin::setRestWheelState(bool state){

    isRobinWheelResting = state;
  } */

bool Robin::getRestHeadState()
{

  return isRobinHeadResting;
}

void Robin::setRestHeadState(bool state)
{

  isRobinHeadResting = state;
}

///////////////////////////////////////////////////////////////////
//-- PREDETERMINED MOTIONS  -------------------------------------//
///////////////////////////////////////////////////////////////////

void Robin::stop()
{

  move(0, 0);
}

int pasoAdelante[8][4] =
    {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1}};

int pasoAtras[8][4] =
    {
        {1, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0}};

void Robin::fordward(int V)
{

  // move(V, V);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(PIN_WR1, pasoAdelante[i][0]);
    digitalWrite(PIN_WR2, pasoAdelante[i][1]);
    digitalWrite(PIN_WR3, pasoAdelante[i][2]);
    digitalWrite(PIN_WR4, pasoAdelante[i][3]);
    digitalWrite(PIN_WL1, pasoAtras[i][0]);
    digitalWrite(PIN_WL2, pasoAtras[i][1]);
    digitalWrite(PIN_WL3, pasoAtras[i][2]);
    digitalWrite(PIN_WL4, pasoAtras[i][3]);
    delay(1);
  }
}

void Robin::backward(int V)
{

  for (int i = 0; i < 8; i++)
  {
    digitalWrite(PIN_WR1, pasoAtras[i][0]);
    digitalWrite(PIN_WR2, pasoAtras[i][1]);
    digitalWrite(PIN_WR3, pasoAtras[i][2]);
    digitalWrite(PIN_WR4, pasoAtras[i][3]);
    digitalWrite(PIN_WL1, pasoAdelante[i][0]);
    digitalWrite(PIN_WL2, pasoAdelante[i][1]);
    digitalWrite(PIN_WL3, pasoAdelante[i][2]);
    digitalWrite(PIN_WL4, pasoAdelante[i][3]);
    delay(1);
  }
}

void Robin::right(int V)
{

  for (int i = 0; i < 8; i++)
  {
    digitalWrite(PIN_WL1, pasoAtras[i][0]);
    digitalWrite(PIN_WL2, pasoAtras[i][1]);
    digitalWrite(PIN_WL3, pasoAtras[i][2]);
    digitalWrite(PIN_WL4, pasoAtras[i][3]);
    delay(1);
  }
}

void Robin::left(int V)
{

  for (int i = 0; i < 8; i++)
  {
    digitalWrite(PIN_WR1, pasoAdelante[i][0]);
    digitalWrite(PIN_WR2, pasoAdelante[i][1]);
    digitalWrite(PIN_WR3, pasoAdelante[i][2]);
    digitalWrite(PIN_WR4, pasoAdelante[i][3]);
    delay(1);
  }
}

///////////////////////////////////////////////////////////////////
//-- SENSORS FUNCTIONS  -----------------------------------------//
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
//-- Robin getDistance: return Robin's ultrasonic sensor measure
//-----------------------------------------------------------------------
long Robin::getDistance()
{
  // if (t_dist == 0 || millis() - t_dist > 500) {
  //   p_dist = us.read();
  //   t_dist = millis();
  // }
  digitalWrite(PIN_Trigger, LOW);
  delayMicroseconds(2);
  // Sets the PIN_Trigger on HIGH state for 10 micro seconds
  digitalWrite(PIN_Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_Trigger, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(PIN_Echo, HIGH);
  // Calculating the distance
  p_dist = duration * 0.034 / 2;
  Serial.print("Distancia: ");
  Serial.println(p_dist);
  return p_dist;
}

//-----------------------------------------------------------------------
//-- Robin getLine: return Robin's IR sensor measure
//-----------------------------------------------------------------------
int Robin::getLine(int side)
{

  if (side > 0)
  {
    return !digitalRead(PIN_LFL);
  }
  return !digitalRead(PIN_LFR);
}

///////////////////////////////////////////////////////////////////
//-- SOUNDS -----------------------------------------------------//
///////////////////////////////////////////////////////////////////

void Robin::_tone(unsigned int noteFrequency, int noteDuration)
{

  tone(PIN_Buzzer, noteFrequency);
  delay(noteDuration);
  noTone(PIN_Buzzer);
  delay(100);
}

void Robin::sim_tone(unsigned int noteFrequency, int noteDuration, int silentDuration)
{

  if (flag_t == 0)
  {
    flag_t = 1;
    pt_t = millis();
    note = noteFrequency;
    dur = noteDuration;
    sil = silentDuration;
    //    Serial.println("Comienzo de nota");
  }
  if (millis() - pt_t < dur)
  {
    tone(PIN_Buzzer, note);
    //    Serial.println("sonando");
  }
  else if (millis() - pt_t < dur + sil)
  {
    noTone(PIN_Buzzer);
    //    Serial.println("silencio");
  }
  else
  {
    flag_t = 0;
    noTone(PIN_Buzzer);
    //    Serial.println("fin de nota");
    //    Serial.print(millis()-pt_t);
  }
}

void Robin::bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration)
{

  //Examples:
  //  bendTones (880, 2093, 1.02, 18, 1);
  //  bendTones (note_A5, note_C7, 1.02, 18, 0);

  if (silentDuration == 0)
  {
    silentDuration = 1;
  }

  if (initFrequency < finalFrequency)
  {
    for (int i = initFrequency; i < finalFrequency; i = i * prop)
    {
      // _tone(i, noteDuration, silentDuration);
      _tone(i, noteDuration);
    }
  }
  else
  {

    for (int i = initFrequency; i > finalFrequency; i = i / prop)
    {
      // _tone(i, noteDuration, silentDuration);
      _tone(i, noteDuration);

    }
  }
}

void Robin::playMelody(struct melody melod)
{
  if (melodia.sheet[i_m] != melod.sheet[i_m])
  {
    flag_m = 0;
  }
  if (flag_m == 0)
  {
    melodia = melod;
    flag_m = 1;
    pt_m = millis();
    i_m = 0;

    //  Serial.println("init");
    //  Serial.println(melodia.duration);
    //  Serial.println(flag_m);
  }
  if (i_m < melodia.duration)
  {
    //  Serial.println("sonando");
    //  Serial.println(flag_m);
    if ((millis() - pt_m) < (melodia.sheet[i_m * 3 + 1] * dur_semicorchea * melodia.rhythm) + (melodia.sheet[i_m * 3 + 2] * dur_semicorchea * melodia.rhythm))
    {
      sim_tone(melodia.sheet[i_m * 3], melodia.sheet[i_m * 3 + 1] * dur_semicorchea * melodia.rhythm, melodia.sheet[i_m * 3 + 2] * dur_semicorchea * melodia.rhythm);
      //  Serial.println(i_m);
    }
    else
    {
      //  Serial.println(melodia.sheet[i_m*3]);
      i_m++;
      pt_m = millis();
      //  Serial.println(i_m);
    }
  }
  else
  {
    i_m = 0;
    flag_m = 0;
    //  Serial.println("reboot");
    //  Serial.println(flag_m);
  }
}

///////////////////////////////////////////////////////////////////
//-- PREDETERMINED MOTION SEQUENCES -----------------------------//
///////////////////////////////////////////////////////////////////
