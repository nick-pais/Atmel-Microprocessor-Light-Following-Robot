#include <avr/io.h>
#include <avr/interrupt.h>

void motor_init(void);
void TCC0_init(void);
void tcf0_init(void);
void interrupt_init(void);

uint8_t backup_flag = 0;
uint8_t fix_dir = 0;

uint8_t volatile left_wheel;
uint8_t volatile right_wheel;
uint8_t volatile backup;

int main(void)
{
	/* init motors */
	motor_init();
		
	/* init PWM */
	tcc0_init();
	
	/* init backup timer */
	tcf0_init();

	interrupt_init();
	
	while (1)
	{
		left_wheel = PORTJ.IN & PIN7_bm;
		right_wheel = PORTJ.IN & PIN6_bm;
		backup = PORTJ.IN & PIN5_bm;
		
		/* foward */
		//PORTC.OUTSET = PIN3_bm | PIN4_bm;
		//PORTC.OUTCLR = PIN2_bm | PIN5_bm;
		
		/* left */
		//PORTC.OUTSET = PIN3_bm;
		//PORTC.OUTCLR = PIN2_bm | PIN4_bm | PIN5_bm;
		
		/* right */
		//PORTC.OUTSET = PIN4_bm;
		//PORTC.OUTCLR = PIN2_bm | PIN3_bm | PIN5_bm;
		
		/* backup */
		//PORTC.OUTSET = PIN2_bm | PIN5_bm;
		//PORTC.OUTCLR = PIN3_bm | PIN4_bm;
		
		if (backup)
		{
			//backup = 0;
			/* set prescalar, start TC */
			TCF0.CTRLA = TC_CLKSEL_DIV64_gc;
			PORTC.OUTSET = PIN2_bm | PIN5_bm;
			PORTC.OUTCLR = PIN3_bm | PIN4_bm;
			while(!(backup_flag));
			backup_flag = 0;
			if (left_wheel && right_wheel)
			{
				PORTC.OUTSET = PIN3_bm | PIN4_bm | PIN5_bm;
				PORTC.OUTCLR = PIN2_bm;
			}
			else if (left_wheel)
			{
				PORTC.OUTSET = PIN3_bm | PIN4_bm | PIN5_bm;
				PORTC.OUTCLR = PIN2_bm;
			}
			else if (right_wheel)
			{
				PORTC.OUTSET = PIN2_bm | PIN3_bm | PIN4_bm;
				PORTC.OUTCLR = PIN5_bm;
			}
			while(!(fix_dir));
			fix_dir = 0;
		}
		else
		{
			if (left_wheel && right_wheel)
			{
				PORTC.OUTSET = PIN3_bm | PIN4_bm;
				PORTC.OUTCLR = PIN2_bm | PIN5_bm;
			}
			else if (left_wheel)
			{
				PORTC.OUTSET = PIN3_bm | PIN4_bm | PIN5_bm;
				PORTC.OUTCLR = PIN2_bm;
			}
			else if (right_wheel)
			{
				PORTC.OUTSET = PIN2_bm | PIN3_bm | PIN4_bm;
				PORTC.OUTCLR = PIN5_bm;
			}
			else
			{
				PORTC.OUTSET = PIN3_bm | PIN4_bm;
				PORTC.OUTCLR = PIN2_bm | PIN5_bm;
			}
		}
		
	}
}

ISR(TCF0_OVF_vect)
{
	backup_flag = 1;
	PORTC.OUTSET = PIN5_bm | PIN2_bm | PIN3_bm | PIN4_bm;
	TCF0.CTRLA = TC_CLKSEL_OFF_gc;
	if (left_wheel && right_wheel)
	{
		TCC1.CTRLA = TC_CLKSEL_DIV64_gc;
	}
	else
	{
		TCF1.CTRLA = TC_CLKSEL_DIV64_gc;
	}
}

ISR(TCF1_OVF_vect)
{
	fix_dir = 1;
	PORTC.OUTSET = PIN5_bm | PIN2_bm | PIN3_bm | PIN4_bm;
	TCF1.CTRLA = TC_CLKSEL_OFF_gc;
}

ISR(TCC1_OVF_vect)
{
	fix_dir = 1;
	PORTC.OUTSET = PIN5_bm | PIN2_bm | PIN3_bm | PIN4_bm;
	TCC1.CTRLA = TC_CLKSEL_OFF_gc;
}



