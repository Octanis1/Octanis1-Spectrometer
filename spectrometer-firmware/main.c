#include <msp430.h> 

/*
 * main.c
 */

#define SPECTRO_LENGTH (2048 + 200)

int spectroBuffer[SPECTRO_LENGTH];

int main(void) {
	int i;

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* Port IO definitions */
    P1DIR |= BIT2;                            // P1.2 output (mastre clock 100kHz)
    P1SEL |= BIT2;                            // P1.2 used as timer output
    P1DIR |= BIT4;                            // P1.4 output (read enable)
    P2DIR |= BIT0;                            // P2.0 output (integration clock max 100kHz/2048)
    P2SEL |= BIT0;                            // P2.0 used as timer output
    P5DIR &= BIT6;                            // P5.6 as input (A/D triggger)
    P5SEL |= BIT6;                            // P5.6 used as timer input

    P6SEL |= 0xff;							  // All pins of P6 (P6.0 to P6.7) used as analog in

    /* Clock selection (8 MHz using DCO) */
    UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx

    // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
    do
    {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                              // Clear XT2,XT1,DCO fault flags
      SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation
    UCSCTL2 |= 249;                           // Set DCO Multiplier for 8MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (249 + 1) * 32768 = 8MHz
    __bic_SR_register(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
    __delay_cycles(250000);



    // Summary
    // Wait for command over SPI/UART to start conversion
    // Set conversion in progress bit disabling ram readout
    // Enable timer TA0 and TA1 to generate Master and Integration clock
    // Enable DMA single to block (copy adc val to ram)
    // Enable ADC 2.5 V ref, Single conversion, 12bit,
    // Enable timer TA2 as input for ADC sample clock
    // Set read enable bit

    // Conversion happens (count events)

    // Clear read enable bit
    // Clear conversion in progress bit, reenabling ram readout
    // If requested, use DMA to readout ram over SPI/UART

    /* while / sleep loop waiting for conversion command */

    /* Setup clocking timer (TA0 and TA1) */
    TA0CCTL1 = OUTMOD_4;                      // CCR1 toggle mode
    TA0CCR1 = 40-1;                           // 8 MHz / 40 / 2 = 100 kHz
    TA0CCR0 = 40-1;                           // Set CCR0 = CCR1 as counts up to CCR0
    TA0CTL = TASSEL_2 + MC_1 + TACLR;         // ACLK, upmode, clear TAR

    TA1CCTL1 = OUTMOD_4;                      // CCR1 toggle mode
    TA1CCR1 = 50000-1;                        // 8 MHz / 8 / 50000 / 2 = 10 Hz
    TA1CCR0 = 50000-1;                        // Set CCR0 = CCR1 as counts up to CCR0
    TA1CTL = TASSEL_2 + ID_2 + ID_1 + MC_1 + TACLR;         // ACLK, prescaler of 8, upmode, clear TAR

    /* Setup DMA0 */
    //Fixed address to block
    //Single transfer
    //Trigger 24 ADC12IFGx
    //DMA RMW DIS (No DMA during read-modify-write instruction)
    DMACTL0 = DMA0TSEL_24; //ADC12IFGx as trigger for DMA0
    DMACTL4 = DMARMWDIS; //Disable DMA during read-modify-write
    DMA0CTL &= ~DMAIFG; //Erase pending DMA0 interrupts
    DMA0CTL = DMADSTINCR_3 + DMAEN; //Single transfer, inc destination, Int
    DMA0SZ = SPECTRO_LENGTH; //amount of peaks equals amount of data read in
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) &ADC12MEM0);
                                                  // Source block address
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) spectroBuffer);
                                                  // Destination single address

    /* Setup Reference voltage for ADC */
    REFCTL0 = REFMSTR + REFVSEL_2 + REFTCOFF + REFON; //Control from REF registers, 2.5 V reference, Temp sensor off, Reference on

    /* Setup ADC */
    ADC12CTL0 = ADC12SHT0_1 + ADC12ON;            // enable ADC12, 8 ADC12CLK sampling time
    ADC12CTL1 = ADC12CSTARTADD_0 + ADC12SHS_2     // Startaddr 0, Timer SHS source TB0.0, Clock divider 2, ACLK source
    		  + ADC12SHP + ADC12DIV_1 + ADC12SSEL_1;
    ADC12CTL2 = ADC12RES_2;                       // 12 bit ADC resolution
    ADC12MCTL0 = ADC12EOS + ADC12SREF_1 + ADC12INCH_0; // Enf of Sequence, Vref/AVSS reference, Channel A0


    for ( i=0; i<0x30; i++);                      // Delay for reference start-up

    ADC12CTL0 |= ADC12ENC;                        // Enable conversions

    /* Setup input timer (TB0) */
    TB0CTL = TBSSEL_1 + MC_2 /* + TBIE */;        // ACLK, continuous mode /* Interrupt enabled */
    TB0CCTL0 = CM_1 + CCIS_1 + CAP /* + CCIE */;  // Rising edge, CCI0B input, Capture mode, /*Interrupt enabled */

    /* Set read enable bit */
    P1OUT |= BIT4;

    /* Program */
    //Check for DMA if finished

    __bis_SR_register(LPM3_bits);             // Enter LPM3
    __no_operation();                         // For debugger
	return 0;
}
