#include "stm32f10x.h"
#include "utility.h"
#include "lcd.h"

enum keyIn{
	FREE = -1,
	K1,K2,K3,KA,
	K4,K5,K6,KB,
	K7,K8,K9,KC,
	KS,K0,KP,KD	
};


void keyboardConfiguration(void);

enum keyIn getKeyboardInput(void);

char decodeKey(enum keyIn in);
