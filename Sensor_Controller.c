#include <avr/io.h>
#include <avr/interrupt.h>

void switch_init();
void adc_init();
void tcc0_init();
void interrupt_init();

int16_t volatile right_cds;
int16_t volatile middle_cds;
int16_t volatile left_cds;
uint8_t left_wheel = 0;
uint8_t right_wheel = 0;
uint8_t backup = 0;

int main(void)
{
	/* enable switches */
	switch_init();
	
	/* ADC init */
	adc_init();
	
	/* TC init */
	tcc0_init();
	
	/* interrupt init */
	interrupt_init();
	
	while (1)
	{
		
		/* LEFT BUMPER PRESSED */
		if (PORTC.IN == 0x40)
		{
			backup = 1;
			left_wheel = 1;
			right_wheel = 0;
			
		}
		/* RIGHT BUMPER PRESSED */
		else if (PORTC.IN == 0x20)
		{
			backup = 1;
			left_wheel = 0;
			right_wheel = 1;
		}
		/* BOFFA BUMPERS PRESSED */
		else if (PORTC.IN == 0x60)
		{
			backup = 1;
			left_wheel = 1;
			right_wheel = 1;
		}
		else
		{
			if (right_cds > middle_cds && right_cds > left_cds)
			{
				left_wheel = 0;
				right_wheel = 1;
				backup = 0;
			}
			else if (left_cds > middle_cds && left_cds > right_cds)
			{
				left_wheel = 1;
				right_wheel = 0;
				backup = 0;
			}
			else if (middle_cds > left_cds && middle_cds > right_cds)
			{
				left_wheel = 1;
				right_wheel = 1;
				backup = 0;
			}
			
			
			
		}
		/* PF7 = left_wheel */
		/* PF6 = right_wheel */
		/* PF5 = backup */
		if (left_wheel)
		{
			PORTF.OUTSET = PIN7_bm;
		}
		else
		{
			PORTF.OUTCLR = PIN7_bm;
		}
		

		if (right_wheel)
		{
			PORTF.OUTSET = PIN6_bm;
		}
		else
		{
			PORTF.OUTCLR = PIN6_bm;
		}
		
		
		if (backup)
		{
			PORTF.OUTSET = PIN5_bm;
		}
		else
		{
			PORTF.OUTCLR = PIN5_bm;
		}
		
		
	}
}



