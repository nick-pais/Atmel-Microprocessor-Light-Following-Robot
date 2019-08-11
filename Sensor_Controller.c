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

ISR(TCC0_OVF_vect)
{
	right_cds = ADCA.CH0.RES;
	middle_cds = ADCA.CH1.RES - 0xA0;
	left_cds = ADCA.CH2.RES + 0x80;
}

void switch_init(void)
{
	PORTC.DIRCLR = PIN6_bm | PIN5_bm;
	PORTF.DIRSET = PIN7_bm | PIN6_bm | PIN5_bm;
}

void adc_init(void)
{
	/* signed mode, 12 bit data */
	ADCA.CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;
	
	/* 2.5 Vref */
	ADCA.REFCTRL = ADC_REFSEL_AREFB_gc;
	
	/* prescalar */
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
	
	/* CDS+ (PA0) and CDS- (PA4) as input sources for the ADC0 channel */
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc | ADC_CH_MUXNEG_PIN4_gc;
	
	/* CDS+ (PA1) and CDS- (PA5) as input sources for the ADC1 channel */
	ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN5_gc;
	
	/* CDS+ (PA2) and CDS- (PA6) as input sources for the ADC1 channel */
	ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc | ADC_CH_MUXNEG_PIN6_gc;
	
	/* differential with 1x gain input */
	ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
	ADCA.CH1.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
	ADCA.CH2.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
	
	/* Sweep CH0, CH0,CH1,CH2,CH3 as inputs, CH0 used as input channel */
	ADCA.EVCTRL = ADC_SWEEP_012_gc | ADC_EVSEL_0123_gc | ADC_EVACT_CH012_gc;
	
	/* enable ADC */
	/* this is disabled for part 5 of the lab */
	ADCA.CTRLA = ADC_ENABLE_bm;
}

void tcc0_init(void)
{
	/* normal TC WG mode */
	TCC0.CTRLB = TC_WGMODE_NORMAL_gc;
	
	/* reset count of TC */
	TCC0.CNT = 0;
	
	/* set period for 250 Hz (part 4 & 5) */
	TCC0.PER = 2000000 / (250 * 64);
	
	/* clear overflow flag */
	TCC0.INTFLAGS = TC0_OVFIF_bm;
	
	/* overflow on TCC0 will trigger event */
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	EVSYS.CH1MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	EVSYS.CH2MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	
	/* low level TC interrupt */
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	
	/* 64 prescalar. start TC */
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
}

void interrupt_init(void)
{
	/* low level PMIC interrupts enabled */
	PMIC.CTRL = PMIC_LOLVLEN_bm;
	
	/* set interrupt flag */
	sei();
}
