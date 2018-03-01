#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

int main(int argc, char**args)
{

	if (argc != 2)
	{
		printf("Please add the number as cmd argument.\n");
		exit(1);
	}
	wiringPiSetup();
	
	// set GPIO pin modes
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	
	// switch off LEDS
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	
	
	//convert string arg to int
	int num = atoi(args[1]);
	
	if (num < 0 || num > 15)
	{
		printf("Invalid Number\n");
		exit(1);
	}
	
	if(num & (1<<3))
		digitalWrite(LED4, HIGH);
	if(num & (1<<2))	
		digitalWrite(LED3, HIGH);
	if(num & (1<<1))
		digitalWrite(LED2, HIGH);
	if(num & (1<<0))
		digitalWrite(LED1, HIGH);
	
	printf("Press enter to exit.\n");
	getchar();	

	// switch off LEDS
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	
}
