#include "robin_sounds.h"
#include "Arduino.h"

float partitura_betis[22*3] = {
  note_C5, corchea, semicorchea,
  note_D5, corchea, semicorchea,
  note_E5, negra, corchea,
  note_D5, semicorchea, semicorchea,
  note_E5, semicorchea, semicorchea,
  note_D5, semicorchea, semicorchea,
  note_E5, negra, semicorchea,
  note_D5, semicorchea, semicorchea,
  note_E5, semicorchea, semicorchea,
  note_G5, semicorchea, semicorchea,
  note_D5, blanca, semicorchea,

  note_D5, corchea, semicorchea,
  note_E5, corchea, semicorchea,
  note_F5, negra, corchea,
  note_E5, semicorchea, semicorchea,
  note_F5, semicorchea, semicorchea,
  note_E5, semicorchea, semicorchea,
  note_F5, negra, semicorchea,
  note_E5, semicorchea, semicorchea,
  note_F5, semicorchea, semicorchea,
  note_A5, semicorchea, semicorchea,
  note_E5, blanca, semicorchea
};

float partitura_sevilla[20*3] = {
  note_F4, negra, semicorchea,
  note_C4, negra, semicorchea,
  note_C4, corchea, semicorchea,
  note_F4, negra, semicorchea,
  note_G4, negra, semicorchea,
  note_A4, negra, semicorchea,
  note_G4, negra, semicorchea,
  note_C4, negra, semicorchea,
  note_C4, semicorchea, semicorchea,
  note_D4, semicorchea, semicorchea,
  note_E4, semicorchea, semicorchea,
  note_F4, semicorchea, semicorchea,
  note_G4, semicorchea, semicorchea,
  note_A4, negra, semicorchea,
  note_Bb4, negra, semicorchea,
  note_A4, semicorchea, semicorchea,
  note_G4, negra, semicorchea,
  note_F4, negra, semicorchea,
  note_A4, negra, semicorchea,
  note_G4, blanca, negra

};

float partitura_cumple[26*3] = {
  note_G5, corchea, semicorchea,
  note_G5, corchea, semicorchea,
  note_A5, negra, semicorchea,
  note_G5, negra, semicorchea,
  note_C5, negra, semicorchea,
  note_B5, blanca, corchea,

  note_G5, corchea, semicorchea,
  note_G5, corchea, semicorchea,
  note_A5, negra, semicorchea,
  note_G5, negra, semicorchea,
  note_D5, negra, semicorchea,
  note_C5, blanca, corchea,

  note_G5, corchea, semicorchea,
  note_G5, corchea, semicorchea,
  note_G5, negra, semicorchea,
  note_E5, negra, semicorchea,
  note_C5, corchea, semicorchea,
  note_C5, corchea, semicorchea,
  note_B5, negra, semicorchea,
  note_A5, blanca, negra,

  note_F5, corchea, semicorchea,
  note_F5, corchea, semicorchea,
  note_E5, negra, semicorchea,
  note_C5, negra, semicorchea,
  note_D5, negra, semicorchea,
  note_C5, blanca, blanca
};

float partitura_saria[20*3] = {
  note_F5, corchea, semicorchea,
  note_A5, corchea, semicorchea,
  note_B5, negra, semicorchea,
  note_F5, corchea, semicorchea,
  note_A5, corchea, semicorchea,
  note_B5, negra, semicorchea,
  note_F5, corchea, semicorchea,
  note_A5, corchea, semicorchea,
  note_B5, corchea, semicorchea,
  note_E6, corchea, semicorchea,
  note_D6, negra, semicorchea,
  note_B5, corchea, semicorchea,
  note_C6, corchea, semicorchea,
  note_B5, corchea, semicorchea,
  note_G5, corchea, semicorchea,
  note_E5, blanca, semicorchea,
  note_D5, corchea, semicorchea,
  note_E5, corchea, semicorchea,
  note_G5, corchea, semicorchea,
  note_E5, blanca, semicorchea,
};
  
melody HIMNO_BETIS;
melody HIMNO_SEVILLA;
melody CUMPLE;
melody SARIA;

  
void init_HIMNO_BETIS()
{
  HIMNO_BETIS.duration = LONG_BETIS;
  HIMNO_BETIS.rhythm = RITMO_BETIS;
  for (int i=0;i<LONG_BETIS*3;i++)
  {
    HIMNO_BETIS.sheet[i] = partitura_betis[i];
  }
}

void init_HIMNO_SEVILLA()
{
  HIMNO_SEVILLA.duration = LONG_SEVILLA;
  HIMNO_SEVILLA.rhythm = RITMO_SEVILLA;
  for(int i = 0; i < LONG_SEVILLA*3; i++)
  {
    HIMNO_SEVILLA.sheet[i] = partitura_sevilla[i];
  }
} 

void init_CUMPLE()
{
  CUMPLE.duration = LONG_CUMPLE;
  CUMPLE.rhythm = RITMO_CUMPLE;
  for (int i=0;i<LONG_CUMPLE*3;i++)
  {
    CUMPLE.sheet[i] = partitura_cumple[i];
  }
}

void init_SARIA()
{
  SARIA.duration = LONG_SARIA;
  SARIA.rhythm = RITMO_SARIA;
  for (int i=0;i<LONG_SARIA*3;i++)
  {
    SARIA.sheet[i] = partitura_saria[i];   
  }
} 
