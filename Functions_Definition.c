/********************************** Functions Definition *************************************/

#include <avr\io.h>
#include <avr\interrupt.h>

/*********************************** Global variables **************************************/

extern signed char seconds;
extern signed char minutes;
extern signed char hours;
extern unsigned char activate_countdown_mode;

/**************************** Timer1 Initialization Function *******************************/

void Timer1_Init (void){

	/* Configure Timer/Counter1 Control Register:
	 * 1. Normal port operation, OC1A/OC1B disconnected	COM1A1 = COM1B1 = COM1A0 = COM1B0 = 0
	 * 2. Force Output Compare for Compare unit A		FOC1A = 1
	 * 3. Force Output Compare for Compare unit B		FOC1B = 1
	 * 4. CTC Mode (Mode 4)								WGM13 = 0 , WGM12 = 1 , WGM11 = 0 , WGM10 = 0
	 * 5. F(timer) = F(CPU)/1024						CS12 = 1 , CS11 = 0 , CS10 = 1
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);

	TCNT1 = 0;				/* Initial Value */
	OCR1A = 15625;			/* Compare Value */
	TIMSK |= (1<<OCIE1A);	/* Timer/Counter1, Output Compare A Match Interrupt Enable */
}

/************************** Interrupt Service Routine for timer 1***************************/

/******************************* Interrupt every 1 second **********************************/

ISR (TIMER1_COMPA_vect){

	if ( activate_countdown_mode == 1 )
	{
		/********************** Count down mode (Handle timer values) *********************/
		seconds--;
		if (seconds == -1){
			seconds = 59;
			minutes--;
		}
		if (minutes == -1){
			minutes = 59;
			hours--;
		}
	}
	else
	{
		/*********************** Count up mode (Handle timer values) **********************/
		seconds++;
		if (seconds == 60){
			seconds = 0;
			minutes++;
		}
		if (minutes == 60){
			minutes = 0;
			hours++;
		}
	}
}

/******************************* INT0 Initialization Function *******************************/

void INT0_Init (void){

	/* Configure PD2 as input pin */
	DDRD &= ~(1<<PD2);

	/* Activate the internal pull up resistance */
	PORTD |= (1<<PD2);

	/* Configure MCU Control Register(MCUCR):
	 * The falling edge of INT0 generates an interrupt request	ISC01 = 1 , ISC00 = 0
	 */
	MCUCR |= (1<<ISC01);

	/* Configure General Interrupt Control Register(GICR):
	 * External Interrupt Request 0 Enable	INT0 = 1
	 */
	GICR |= (1<<INT0);
}

/**************************** Interrupt Service Routine for INT0 ****************************/

/*************************** Reset stop watch time every interrupt **************************/

ISR (INT0_vect){
	/* Reset stop watch time */
	seconds = minutes = hours = 0;

	/* Reset TCNT1 Register */
	TCNT1 = 0;
}

/********************************* INT1 Initialization Function *****************************/

void INT1_Init (void){

	/* Configure PD3 as input pin */
	DDRD &= ~(1<<PD3);

	/* Configure MCU Control Register(MCUCR):
	 * The rising edge of INT1 generates an interrupt request	ISC11 = 1 , ISC10 = 1
	 */
	MCUCR |= (1<<ISC11) | (1<<ISC10);

	/* Configure General Interrupt Control Register(GICR):
	 * External Interrupt Request 1 Enable	INT1 = 1
	 */
	GICR |= (1<<INT1);
}

/****************************** Interrupt Service Routine for INT1 ***************************/

/***************************** Pause stop watch time every interrupt *************************/

ISR (INT1_vect){

	/* Reset TCNT1 Register */
	TCNT1 = 0;

	/* Configure Timer/Counter1 Control Register B(TCCR1B):
	 * No clock source (Timer/Counter stopped)	CS12 = CS11 = CS10 = 0 (The first 3 bits in register)
	 */
	TCCR1B &= 0xF8;
}

/*********************************** INT2 Initialization Function ***************************/

void INT2_Init (void){

	/* Configure PB2 as input pin */
	DDRB &= ~(1<<PB2);
	/* Activate the internal pull up resistance */
	PORTB |= (1<<PB2);

	/* Configure MCU Control and Status Register(MCUCSR):
	 * The falling edge of INT2 generates an interrupt request	ISC2 = 0
	 */
	MCUCSR &= ~(1<<ISC01);

	/* Configure General Interrupt Control Register(GICR):
	 * External Interrupt Request 2 Enable	INT2 = 1
	 */
	GICR |= (1<<INT2);
}

/******************************** Interrupt Service Routine for INT2**************************/

/****************************** Resume stop watch time every interrupt ***********************/

ISR (INT2_vect){

	/* Configure Timer/Counter1 Control Register B(TCCR1B):
	 * F(timer) = F(CPU)/1024		CS12 = 1 , CS11 = 0 , CS10 = 1
	 */
	TCCR1B |= (1<<CS12) | (1<<CS10);
}

/******************************* Configuration_Seven_Segment **********************************/

void Seven_Segment_Configuration (void){

	/* Configure the first 4 pins in PORTC as output pins for
	 * six multiplexed seven-segment displays (common anode)
	 */
	DDRC |= 0x0F;

	/* Configure the first 6 pins in PORTA as output pins for
	 * controlling the enable/disable for each 7-segment using
	 * a NPN BJT transistor
	 */
	DDRA |= 0x3F;
}

/******************** Toggle Mode Button Configuration Function *******************************/

void Toggle_Mode_Button_Configuration (void){

	/* Configure PB7 as input pin (Toggle mode button) */
	DDRB &= ~(1<<PB7);
	/* Activate internal pull up resistance for PB7 */
	PORTB |= (1<<PB7);
}

/***************************** Modes Led Configuration Function *******************************/

void Modes_Led_Configuration(void){

	/* Configure PD4 as output pin (Count up led)
	 * Configure PD5 as output pin (Count down led)
	 */
	DDRD |= 0x30;
}

/****************************** Buzzer Configuration Function**********************************/

void Buzzer_Configuration(void){

	/* Configure PD0 as output pin (Buzzer) */
	DDRD |= (1<<PD0);
	/* Initially Buzzer OFF */
	PORTD &= ~(1<<PD0);
}

/****************************** Time Adjustment Configuration ********************************/

void Time_Adjustment_Configuration(void){

	/* Configure PB1 and PB0 as input pins (Hours Adjustment in count down mode)
	 * PB1 --> Increment hours		, PB0 --> Decrement hours
	 */
	DDRB &= ~(1<<PB1) & ~(1<<PB0);
	/* Activate internal pull up resistance for PB1 and PB0 */
	PORTB |= (1<<PB1) | (1<<PB0);

	/* Configure PB4 and PB3 as input pins (Minutes Adjustment in count down mode)
	 * PB4 --> Increment hours		, PB3 --> Decrement hours
	 */
	DDRB &= ~(1<<PB4) & ~(1<<PB3);
	/* Activate internal pull up resistance for PB1 and PB0 */
	PORTB |= (1<<PB4) | (1<<PB3);

	/* Configure PB6 and PB5 as input pins (Seconds Adjustment in count down mode)
	 * PB6 --> Increment hours		, PB5 --> Decrement hours
	 */
	DDRB &= ~(1<<PB6) & ~(1<<PB5);
	/* Activate internal pull up resistance for PB1 and PB0 */
	PORTB |= (1<<PB6) | (1<<PB5);
}

/********************************** Display_7Segment function *******************************/

/**************** This function shows a certain number on a certain 7-segment ***************/

void Display_7Segment (unsigned char number ,unsigned char index ){

	/* Enable specific 7-segment */
	PORTA = (PORTA & 0xC0) | (1<<index);

	/* Display the number at this 7-segment */
	PORTC = (PORTC & 0xF0) | (number & 0x0F );
}

/********************************* End of Function Definition ******************************/
