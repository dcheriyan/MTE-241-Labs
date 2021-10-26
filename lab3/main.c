#include <cmsis_os2.h> 
#include <LPC17xx.h> 
#include <stdio.h>

//*********Helper Functions for LEDs and Button*********//
void Left_LED_ON()
{
	LPC_GPIO1->FIOSET = (1u << 28);
}
void Left_LED_OFF()
{
	LPC_GPIO1->FIOCLR = (1u << 28);
}

void Lower_LED_ON(int LED_number){
	LPC_GPIO2->FIOSET = (1u << LED_number);
}
void Lower_LED_OFF(int LED_number){
	LPC_GPIO2->FIOCLR = (1u << LED_number);
}
char Button(){
	if ((LPC_GPIO2->FIOPIN & (1u << 10)) == 0u) //check if 0 since button is active low
	{	
		return 1u; //true if button on
	}
	else
	{
		return 0u; //false if button off
	}
}
//******************************************************//

//task 1: joystick binary output
__NO_RETURN void joystick (void *arg){
	
	//Initialize joystick inputs, as well as top LED for LED toggle task
    LPC_GPIO1->FIODIR = (0u << 23)|(0u << 25)|(0u << 26)|(0u << 24)|(0u << 20)|(1u << 28);
	//Set LEDs 5-8, and button for LED toggle task
	LPC_GPIO2->FIODIR = (1u << 3)|(1u << 4)|(1u << 5)|(1u << 6)|(0u << 10);
	
	for (int i = 0;; i++){
//******Lab 2 code*******//	
            if ((LPC_GPIO1->FIOPIN & (1u << 23)) == 0u){		//Check if North
				Lower_LED_OFF(4);
				Lower_LED_OFF(5);
				Lower_LED_ON(6);								//Binary 1
			}	
			else if ((LPC_GPIO1->FIOPIN & (1u << 24)) == 0u){	//Check if East
				Lower_LED_OFF(4);
				Lower_LED_ON(5);
				Lower_LED_OFF(6);								//Binary 2
			}
            else if ((LPC_GPIO1->FIOPIN & (1u << 25)) == 0u){	//Check if South
				Lower_LED_OFF(4);
				Lower_LED_ON(5);
				Lower_LED_ON(6);								//Binary 3
			}
            else if ((LPC_GPIO1->FIOPIN & (1u << 26)) == 0u){	//Check if West
				Lower_LED_ON(4);
				Lower_LED_OFF(5);
				Lower_LED_OFF(6);								//Binary 4
			}
            else { 												//Otherwise Assume Center
                Lower_LED_OFF(4);
				Lower_LED_OFF(5);
				Lower_LED_OFF(6);								//Binary 0
			}
            if ((LPC_GPIO1->FIOPIN & (1u << 20)) == 0u)			//Check if Pressed
                Lower_LED_ON(3); //turn on LED 5		
            else 												//Otherwise Assume Released
                Lower_LED_OFF(3);
//**********************//		
		
		//printf("Joystick- %d\n", i);
		osDelay(1);
	}
}

__NO_RETURN void ADC (void *arg){
	
	//Initialize ADC 
	LPC_SC->PCONP |= 1u << 12; //Power up the Peripheral
	
	LPC_SC->PCLKSEL0 |= (1u << 24);
	LPC_SC->PCLKSEL0 &= ~(1u << 25); //Set the Peripheral clock to CLK/4
	
	LPC_PINCON->PINSEL1 |= 1u << 18; // Set bit 18
	LPC_PINCON->PINSEL1 &= ~(1u << 19); // clear bit 19
	
	LPC_ADC->ADCR |= 1u<< 21;
	
	LPC_ADC->ADCR &= ~(0xff);
	
	LPC_ADC->ADCR |= 1u << 2;
	
	LPC_ADC->ADCR &= ~(1u<<16);

	LPC_ADC->ADCR &= ~(1u << 26);
	LPC_ADC->ADCR &= ~(1u << 25);
	LPC_ADC->ADCR |= 1u<< 24;

	float voltage = 0;
	
	for (int i = 0;; i++){
		if((LPC_ADC->ADGDR & (1u << 31))){
			voltage = (((LPC_ADC->ADGDR & 0xfff0) >> 4) * 3.3/4095);
			printf("Output Voltage: %.4f V\n", voltage);
			LPC_ADC->ADCR &= ~(1u << 16);
			LPC_ADC->ADCR &= ~(1u << 26);
			LPC_ADC->ADCR &= ~(1u << 25);
			LPC_ADC->ADCR |= 1u << 24;
			}
		//printf("ADC - %d\n", i);
		osDelay(1);
	}	
}

__NO_RETURN void LED_toggle (void *arg){
	
	int cycle = 0; //track if button is in a pressing cycle or not
	
	for (int i = 0;; i++){
		if (Button()==1u)
		{
			cycle = 1;	//on button press, begin pressing cycle
		}
		else
		{
			if ((LPC_GPIO1->FIOPIN & (1u << 28)) && cycle) {// if button is in pressing cycle, is currently released, and LED is ON, turn LED off
				Left_LED_OFF();
			}
			else if(~(LPC_GPIO1->FIOPIN & (1u << 28)) && cycle){// if button is in pressing cycle, is currently released, and LED is OFF, turn LED ON
				Left_LED_ON();
			}
			cycle = 0;
		}
		//printf("LED - %d\n", i);
		osDelay(1);
	}
}

//standard thread manager
__NO_RETURN void app_main (void *arguement){
	
	osThreadNew(joystick, NULL, NULL);
	osThreadNew(ADC, NULL, NULL);
	osThreadNew(LED_toggle, NULL, NULL);
	
	for(;;){
		printf("app main ... \n");
		osDelay(50);
	}
}


int main (void) {
	//System Initialization
	SystemCoreClockUpdate();
	
	//run the tasks
	osKernelInitialize();
	osThreadNew(app_main, NULL, NULL); 
	osKernelStart();
	for(;;){}
}
