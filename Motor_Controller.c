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

void motor_init(void)
{
	/* PWMA, AIN2, AIN1, PWMB, BIN2, BIN1 as outputs driven low by default */
	PORTC.OUTCLR =  PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
	PORTC.DIRSET =  PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
	
	/* /STBY as output driven high by default */
	PORTA.OUTSET = PIN0_bm;
	PORTA.DIRSET = PIN0_bm;

	/* Motor signals */
	PORTJ.DIRCLR = PIN7_bm | PIN6_bm | PIN5_bm;
}

void tcc0_init(void)
{
	/* Compare Capture channel A enable, Single Slope PWM mode */
	TCC0.CTRLB = TC0_CCBEN_bm | TC0_CCAEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	
	/* reset count */
	TCC0.CNT = 0;
	
	/* define period */
	TCC0.PER = 0x50;
	
	/* clear overflow flag */
	TCC0.INTFLAGS = TC0_OVFIF_bm;
	
	/* define top of PWM */
	TCC0_CCA = 0x50;
	TCC0.CCB = 0x50;
	
	/* set prescalar, start TC for PWM */
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
}

void tcf0_init(void)
{
	/* normal WG mode */
	TCF0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCF1.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC1.CTRLB = TC_WGMODE_NORMAL_gc;

	/* reset count */
	TCF0.CNT = 0;
	TCF1.CNT = 0;
	TCC1.CNT = 0;

	/* define period */
	TCF0.PER = 2000000/64;
	TCF1.PER = 2000000/64;
	TCC1.PER = (2000000*2)/64;

	/* clear overflow flag */
	TCF0.INTFLAGS = TC0_OVFIF_bm;
	TCF1.INTFLAGS = TC1_OVFIF_bm;
	TCC1.INTFLAGS = TC1_OVFIF_bm;
	
	/* low level TC interrupt */
	TCF0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	TCF1.INTCTRLA = TC_OVFINTLVL_LO_gc;
	TCC1.INTCTRLA = TC_OVFINTLVL_LO_gc;
}

void interrupt_init(void)
{
	/* low level PMIC interrupts enabled */
	PMIC.CTRL = PMIC_LOLVLEN_bm;
	
	/* set interrupt flag */
	sei();
}

