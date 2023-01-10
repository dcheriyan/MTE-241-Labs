#include <stdio.h>
#include <LPC17xx.h>
#include <stdlib.h>

 /*
 * Names:
 *   1.  uWaterloo User ID:  dcheriya@uwaterloo.ca
 *   2.  uWaterloo User ID:  exfu@uwaterloo.ca
 */

//Citations
//https://stackoverflow.com/questions/22065675/get-text-from-user-input-using-c
//https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
//https://www.educative.io/edpresso/what-is-strtol-in-c

//define PART1
//#define PART2
//#define PART3
#define PART4


#ifdef PART1

	//Part 1 Helper Functions
	void Left_LED_ON()
	{
		LPC_GPIO1->FIOSET = (1u << 28);
	}
	void Left_LED_OFF()
	{
		LPC_GPIO1->FIOCLR = (1u << 28);
	}
	char Button_On()
	{
		if ((LPC_GPIO2->FIOPIN & (1u << 10)) == 0u) //check if 0 since button is active low
		{	
			return 1u; //true if button on
		}
		else
		{
			return 0u; //false if button off
		}
	}
	char Joystick_Button_On()
	{
		if ((LPC_GPIO1->FIOPIN & (1u << 20)) == 0u) //check if 0 since button is active low
		{	
			return 1u; //true if button on
		}
		else
		{
			return 0u; //false if button off
		}
	}
	
	//Part 1 Main Loop
	int main() 
	{
		printf("Lab2: Part 1");
		//Set LeftMost LED as output, and joystick as input for stopping condition
		LPC_GPIO1->FIODIR = (1u << 28|(0u << 20));
		//Set Button as Input
		LPC_GPIO2->FIODIR = (0u << 10);
		
		//Stopping Condition Variable
		char status = 1;
		
		//Continually Read Value of Push Button
		while (status ==1)
		{
			if (Button_On()==1u)	//Check if the Button is on (pushed down)
			{
				Left_LED_ON();
			}
			else					//If its not on assume its off
			{
				Left_LED_OFF();
			}
			
			//Stop if the Joystick is pressed (Exit condition for the loop)
			if (Joystick_Button_On()==1u)
			{
				status = 0;
			}
		}
		return 0;
	}
#endif

#ifdef PART2
//question 2
	char Button_On()
	{
		if ((LPC_GPIO2->FIOPIN & (1u << 10)) == 0u) //check if 0 since button is active low
		{	
			return 1u; //true if button on
		}
		else
		{
			return 0u; //false if button off
		}
	}

    void Joystick(){
        //Initialize joystick inputs
        LPC_GPIO1->FIODIR = (0u << 23)|(0u << 25)|(0u << 26)|(0u << 24)|(0u << 20);
		//Initialize Button
		LPC_GPIO2->FIODIR = (0u << 10);
		
        char status = 1;
        while (status){//repeatedly check until program aborted

            if ((LPC_GPIO1->FIOPIN & (1u << 23)) == 0u)			//Check if North
                printf("North\n");
            else if ((LPC_GPIO1->FIOPIN & (1u << 25)) == 0u)	//Check if South
                printf("South\n");
            else if ((LPC_GPIO1->FIOPIN & (1u << 26)) == 0u)	//Check if West
                printf("West\n");
            else if ((LPC_GPIO1->FIOPIN & (1u << 24)) == 0u)	//Check if East
                printf("East\n");
            else 												//Otherwise Assume Center
                printf("Centre\n");

            if ((LPC_GPIO1->FIOPIN & (1u << 20)) == 0u)			//Check if Pressed
                printf("Pressed\n");
            else 												//Otherwise Assume Released
                printf("Not Pressed\n");
			
			//Stopping Condition
			if (Button_On()==1u)
				status = 0; //end loop if button is pressed
		}
    }

    int main(){
        Joystick();
    }
#endif

#ifdef PART3
//LEDS are labeled 1 - 8 (1 being the left / top-most, 8 being the right-most) Also 1 is the first digit, 8 is the last digit
	void LED_1_ON()
	{
		LPC_GPIO1->FIOSET = (1u << 28);
	}
	void LED_1_OFF()
	{
		LPC_GPIO1->FIOCLR = (1u << 28);
	}
	void LED_2_ON()
	{
		LPC_GPIO1->FIOSET = (1u << 29);
	}
	void LED_2_OFF()
	{
		LPC_GPIO1->FIOCLR = (1u << 29);
	}
	void LED_3_ON()
	{
		LPC_GPIO1->FIOSET = (1u << 31);
	}
	void LED_3_OFF()
	{
		LPC_GPIO1->FIOCLR = (1u << 31);
	}
	void LED_4_ON()
	{
		LPC_GPIO2->FIOSET = (1u << 2);
	}
	void LED_4_OFF()
	{
		LPC_GPIO2->FIOCLR = (1u << 2);
	}
	void LED_5_ON()
	{
		LPC_GPIO2->FIOSET = (1u << 3);
	}
	void LED_5_OFF()
	{
		LPC_GPIO2->FIOCLR = (1u << 3);
	}
	void LED_6_ON()
	{
		LPC_GPIO2->FIOSET = (1u << 4);
	}
	void LED_6_OFF()
	{
		LPC_GPIO2->FIOCLR = (1u << 4);
	}
	void LED_7_ON()
	{
		LPC_GPIO2->FIOSET = (1u << 5);
	}
	void LED_7_OFF()
	{
		LPC_GPIO2->FIOCLR = (1u << 5);
	}
	void LED_8_ON()
	{
		LPC_GPIO2->FIOSET = (1u << 6);
	}
	void LED_8_OFF()
	{
		LPC_GPIO2->FIOCLR = (1u << 6);
	}
	
	char Button_On()
	{
		if ((LPC_GPIO2->FIOPIN & (1u << 10)) == 0u) //check if 0 since button is active low
		{	
			return 1u; //true if button on
		}
		else
		{
			return 0u; //false if button off
		}
	}
	
	//Part 3 Main Loop
	int main()
	{
		printf("Lab2: Part 3 \n");
		//Initial Setup for the LEDs and Buttons
		LPC_GPIO1->FIODIR = (1u << 28 )|(1u << 29)|(1u << 31); //Set LED 1-3
		LPC_GPIO2->FIODIR = (1u << 2)|(1u << 3)|(1u << 4)|(1u << 5)|(1u << 6)|(0u << 10); //Set LEDs 4-8 and Buttons
		
		//Stopping Condition Variable
		char status = 1;
		
		//Variables to ge the input
		char input_string[9];
		char* useless; //needed but unused paramter for conversion function
		long number;
		
		//Continually Set the LEDs to the value of the input
		while (status==1)
		{
			//Getting the Input
			fgets(input_string,9,stdin);
			//Convert char to an int so we can do Math
			number = strtol(input_string, &useless, 10);
			
			//LED 8
			if(number%2==1)			//Take the number mod 2 to to see if the value for that digit 
			{						//is a 1 or 0 and set the LED accordingly
				LED_8_ON();
			}
			else
			{
				LED_8_OFF();
			}
			number = number/2;		//Divide by 2 (integer division) so that the next digit can be assessed

			//LED 7
			if(number%2==1)		//Repeat the same process for each LED (and digit)
			{
				LED_7_ON();
			}
			else
			{
				LED_7_OFF();
			}
			number = number/2;
			
			//LED 6
			if(number%2==1)
			{
				LED_6_ON();
			}
			else
			{
				LED_6_OFF();
			}
			number = number/2;
		
			//LED 5
			if(number%2==1)
			{
				LED_5_ON();
			}
			else
			{
				LED_5_OFF();
			}
			number = number/2;

			//LED 4
			if(number%2==1)
			{
				LED_4_ON();
			}
			else
			{
				LED_4_OFF();
			}
			number = number/2;

			//LED 3
			if(number%2==1)
			{
				LED_3_ON();
			}
			else
			{
				LED_3_OFF();
			}
			number = number/2;
			//LED 2
			if(number%2==1)
			{
				LED_2_ON();
			}
			else
			{
				LED_2_OFF();
			}
			number = number/2;
			
			//LED 1
			if(number%2==1)
			{
				LED_1_ON();
			}
			else
			{
				LED_1_OFF();
			}
			number = number/2;

			//printf("Done %lx \n", number); //Check that we got to 0 and assessed all digits
			
			//Stopping Condition
			if (Button_On()==1u)
			{
				status = 0; //end loop if button is pressed
			}
		}
		return 0;
	}
#endif

#ifdef PART4

	char Button_On()
	{
		if ((LPC_GPIO2->FIOPIN & (1u << 10)) == 0u) //check if 0 since button is active low
		{	
			return 1u; //true if button on
		}
		else
		{
			return 0u; //false if button off
		}
	}
	
	
	int main()
	{	
		//Set button as input
		LPC_GPIO2->FIODIR = (0u << 10);

		//Stopping Condition Variable
		int status = 1;
		
		//Continuously Check the Potentiometer / ADC Value
		while(status == 1)
		{	
			//Power up the Peripheral
			LPC_SC->PCONP |= 1u << 12;

			//Set the Peripheral clock to CLK/4
			LPC_SC->PCLKSEL0 &= ~(1u << 24);
			LPC_SC->PCLKSEL0 &= ~(1u << 25);
			
			//Select the pin
			LPC_PINCON->PINSEL1 |= 1u << 18; // Set   bit 18
			LPC_PINCON->PINSEL1 &= ~(1u<<19); // clear bit 19
				
			//Select the channel
			LPC_ADC->ADCR |= 1u << 2; //Set bit 2 so we use channel 2
			LPC_ADC->ADCR &= ~(1u<<0); // clear bit 0
			LPC_ADC->ADCR &= ~(1u<<1); // clear bit 1
			LPC_ADC->ADCR &= ~(1u<<3); // clear bit 3
			LPC_ADC->ADCR &= ~(1u<<4); // clear bit 4
			LPC_ADC->ADCR &= ~(1u<<5); // clear bit 5
			LPC_ADC->ADCR &= ~(1u<<6); // clear bit 6
			LPC_ADC->ADCR &= ~(1u<<7); // clear bit 7
		
			//Select the clock divider to 1 since 25/(1+1) = 12.5 < 13
			LPC_ADC->ADCR |= 1u << 8; //set bit 8 so its 1
			LPC_ADC->ADCR &= ~(1u<<9); // clear bit 9
			LPC_ADC->ADCR &= ~(1u<<10); // clear bit 10
			LPC_ADC->ADCR &= ~(1u<<11); // clear bit 11
			LPC_ADC->ADCR &= ~(1u<<12); // clear bit 12
			LPC_ADC->ADCR &= ~(1u<<13); // clear bit 13
			LPC_ADC->ADCR &= ~(1u<<14); // clear bit 14
			LPC_ADC->ADCR &= ~(1u<<15); // clear bit 15
			
			//set the converter operational
			LPC_ADC->ADCR |= 1u<< 21;	

			//start the conversion
			LPC_ADC->ADCR |= 1u<< 24;	
			LPC_ADC->ADCR &= ~(1u << 25);
			LPC_ADC->ADCR &= ~(1u << 26);
			
			//Wait For the transfer to complete
			while ((LPC_ADC->ADGDR & (1u << 31)) == (0u << 31))
			{
			}
			//Get the bits for the data output
			unsigned int bit0 = ((LPC_ADC->ADGDR & (1u << 4) ) == (1u << 4));
			unsigned int bit1 = ((LPC_ADC->ADGDR & (1u << 5) ) == (1u << 5));
			unsigned int bit2 = ((LPC_ADC->ADGDR & (1u << 6) ) == (1u << 6));
			unsigned int bit3 = ((LPC_ADC->ADGDR & (1u << 7) ) == (1u << 7));
			unsigned int bit4 = ((LPC_ADC->ADGDR & (1u << 8) ) == (1u << 8));
			unsigned int bit5 = ((LPC_ADC->ADGDR & (1u << 9) ) == (1u << 9));
			unsigned int bit6 = ((LPC_ADC->ADGDR & (1u << 10) ) == (1u << 10));
			unsigned int bit7 = ((LPC_ADC->ADGDR & (1u << 11) ) == (1u << 11));
			unsigned int bit8 = ((LPC_ADC->ADGDR & (1u << 12) ) == (1u << 12));
			unsigned int bit9 = ((LPC_ADC->ADGDR & (1u << 13) ) == (1u << 13));
			unsigned int bit10 = ((LPC_ADC->ADGDR & (1u << 14) ) == (1u << 14));
			unsigned int bit11 = ((LPC_ADC->ADGDR & (1u << 15) ) == (1u << 15));
			unsigned output   = (x & (0xfff0)) >> 4;
			//Get the digital output in a decimal form
			unsigned int Dout = (1*bit0)+(2*bit1)+(4*bit2)+(8*bit3)+(16*bit4)+(32*bit5)+(64*bit6)+
			(128*bit7)+(256*bit8)+(512*bit9)+(1024*bit10)+(2048*bit11);
			
			//Convert the digital output to a voltage
			float output_voltage = (Dout*(3.3/4095));
			
			//print the unscaled data
			//printf("Dout is %u. bits are %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u \n",
			//Dout, bit0,bit1,bit2,bit3,bit4,bit5,bit6,bit7,bit8,bit9,bit10,bit11);
			
			//print the voltage
			printf("Output is  %f V \n", output_voltage);
			
			if (Button_On()==1u)
			{
				status = 0; //end loop if button is pressed
			}
		}
		return 0;
	}

#endif
