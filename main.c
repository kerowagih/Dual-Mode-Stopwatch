#include <avr\io.h>
#include <avr\interrupt.h>
#include <util\delay.h>

/* Time adjustment buttons flags */
#define NUM_OF_FLAGS 6

/*********************************** Global Variables ****************************************/
/* Stop watch timer variable */
signed char seconds = 0;
signed char minutes = 0;
signed char hours = 0;

/* Mode variable
 * activate_countdown_mode = 0	(Count up mode)
 * activate_countdown_mode = 1	(Count down mode)
 */
unsigned char activate_countdown_mode = 0;

/* Toggle Mode button flag */
unsigned char toggle_mode_button_flag  = 0;

/* Time adjustment buttons flags */
unsigned char time_adjustment_buttons_flags[NUM_OF_FLAGS] = {0};

/*********************************** Functions Prototype **************************************/
#include "Functions_Prototype.h"

/********************************* Start of main function *************************************/
int main(void){

/******************************* Application Initialization **********************************/

	/* Enable global interrupt (I-Bit) */
	SREG |= (1<<7);

	/* Timer1 initialization */
	Timer1_Init();

	/* INT0_Initialization */
	INT0_Init();

	/* INT1_Initialization */
	INT1_Init();

	/* INT2_Initialization */
	INT2_Init();

	/* 7-segment configuration */
	Seven_Segment_Configuration();

	/* Toggle mode button configuration */
	Toggle_Mode_Button_Configuration();

	/* Modes led configuration */
	Modes_Led_Configuration();

	/* Buzzer configuration */
	Buzzer_Configuration();

	/* Time adjustment (Count down mode) configuration */
	Time_Adjustment_Configuration();

/*********************************** Application Code **************************************/

	for ( ; ; ){

/******************* Display current time in six multiplexed seven-segment ****************/
		Display_7Segment((seconds % 10),5);
		_delay_ms(2);
		Display_7Segment(((seconds/10) % 10),4);
		_delay_ms(2);
		Display_7Segment((minutes % 10),3);
		_delay_ms(2);
		Display_7Segment(((minutes/10) % 10),2);
		_delay_ms(2);
		Display_7Segment((hours % 10),1);
		_delay_ms(2);
		Display_7Segment((hours/10) % 10,0);
		_delay_ms(2);

/****************************** Check Toggle Mode Button **********************************/
		if ( !(PINB & (1<<PB7)) )
		{
			_delay_ms(30);		/* De_bouncing */
			if ( (!(PINB & (1<<PB7))) )
			{
				if ( toggle_mode_button_flag == 0 )
				{
					/* Toggle Mode */
					if (activate_countdown_mode)
					{
						activate_countdown_mode = 0;
					}
					else
					{
						activate_countdown_mode = 1;
					}
					toggle_mode_button_flag = 1;
				}
			}
			else
			{
				toggle_mode_button_flag = 0;
			}
		}

/************************************* Mode code *******************************************/

		if ( activate_countdown_mode == 0 )
		{
/************************* Stop watch mode (count up mode) ********************************/

			/* Turn on the red led */
			PORTD |= (1<<PD4);

			/* Turn off the yellow led */
			PORTD &= ~(1<<PD5);

			/* Turn off the buzzer led */
			PORTD &= ~(1<<PD0);
		}
/********************************* End of count up mode ************************************/
		else
		{
/************************************ Count down mode **************************************/

			/* Turn off the red led */
			PORTD &= ~(1<<PD4);

			/* Turn on the yellow led */
			PORTD |= (1<<PD5);

/********************** check if the time is end in count down mode ***********************/

			if ( seconds == 0 && minutes == 0 && hours == 0 )
			{
				/* Turn on the buzzer led and buzzer*/
				PORTD |= (1<<PD0);

				/* Pause the count down timer */
				TCCR1B &= 0xF8;
			}
			else
			{
				/* Turn off the buzzer led and buzzer*/
				PORTD &= ~(1<<PD0);
			}

/************************ Hours adjustment in count down mode ******************************/

/*********************************** Decrement hour ****************************************/

			if ( !(PINB & (1<<PB0)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB0))) )
				{
					if ( time_adjustment_buttons_flags[0] == 0 )
					{
						/* Hours can not be less than 0 */
						if ( hours != 0 )
						{
							hours--;
						}
						time_adjustment_buttons_flags[0] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[0] = 0;
				}
			}

/************************************* Increment hour ***************************************/

			if ( !(PINB & (1<<PB1)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB1))) )
				{
					if ( time_adjustment_buttons_flags[1] == 0 )
					{
						/* Hours can not be more than 99 */
						if ( hours != 99 )
						{
							hours++;
						}
						time_adjustment_buttons_flags[1] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[1] = 0;
				}
			}

/************************ Minutes adjustment in count down mode ****************************/

/********************************** Decrement minute ***************************************/

			if ( !(PINB & (1<<PB3)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB3))) )
				{
					if ( time_adjustment_buttons_flags[2] == 0 )
					{
						/* Minutes can not be less than 0 */
						if ( minutes != 0 )
						{
							minutes--;
						}
						time_adjustment_buttons_flags[2] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[2] = 0;
				}
			}

/************************************* Increment minute *************************************/

			if ( !(PINB & (1<<PB4)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB4))) )
				{
					if ( time_adjustment_buttons_flags[3] == 0 )
					{
						/* Minutes can not be more than 59 */
						if ( minutes != 59 )
						{
							minutes++;
						}
						time_adjustment_buttons_flags[3] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[3] = 0;
				}
			}

/*************************** Seconds adjustment in count down mode ***************************/

/************************************ Decrement second **************************************/

			if ( !(PINB & (1<<PB5)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB5))) )
				{
					if ( time_adjustment_buttons_flags[4] == 0 )
					{
						/* Seconds can not be less than 0 */
						if ( seconds != 0 )
						{
							seconds--;
						}
						time_adjustment_buttons_flags[4] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[4] = 0;
				}
			}

/*********************************** Increment second ****************************************/

			if ( !(PINB & (1<<PB6)) )
			{
				_delay_ms(30);		/* De_bouncing */
				if ( (!(PINB & (1<<PB6))) )
				{
					if ( time_adjustment_buttons_flags[5] == 0 )
					{
						/* Seconds can not be more than 59 */
						if ( seconds != 59 )
						{
							seconds++;
						}
						time_adjustment_buttons_flags[5] = 1;
					}
				}
				else
				{
					time_adjustment_buttons_flags[5] = 0;
				}
			}

		}
/********************************** End of count down mode ************************************/

	}
/********************************* End of application code ***********************************/

}
/********************************* End of main function *************************************/
