
/*! 
 * \brief takt_init - инициализация тактового генератора
 *
 *  \par  частота FLL определяется параметром DELTA, 
 *        запускается таймер 0 с периодом, определяемым  параметром DELTA_100ms
 * 
 *
 *  Stack:     не использует \par
 *  Return:    возвращает значение скомпенсированного давления \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
#include  "msp430x54xA.h"

#include  <sub_main_data.h>


void takt_init(void)
{
 //*********************
    
//   P7DIR |= 0x03;                                 // запрет внешнего  
//   P7OUT &= ~0x03;                                // кварца
/* 
 P11DIR |= 0x07;                           // ACLK, MCLK, SMCLK set out to pins
 P11SEL |= 0x07;                           // P11.0,1,2 for debugging purposes.
*/
  UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x1a00;                // близкий к 4 ?vц         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_xn;                      // Select DCO range 7.38 MHz operation
  UCSCTL2 = FLLD_1 + DELTA;                 // Set DCO Multiplier for 2MHz
                                            // (DELTA + 1) * FLLRef = Fdco
                                            // (60 + 1) * 32768 = 1 998 848 Hz
                                            // Set FLL Div = fDCOCLK/2
  
   // timer A0 - 100ms set
  TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA0CCR0 = DELTA_100ms;
  TA0CTL = TASSEL_2 + ID1 + ID0 + MC_1 + TACLR;  // SMCLK/8, upmode, clear TAR   
  TA0EX0 = TAIDEX_7;                             //еще делим на 8 тактовую частоту
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  //*******************
 //  P1DIR |= BIT0;                            // P1.0 output


/*
  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 2 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
  __delay_cycles(62500);

  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
*/
  
  
  // Setup RTC Timer
  RTCCTL01 = RTCMODE ;     // в календарном режиме - делители конфигурируются автоматом    
  RTCPS1CTL = RT1IP_7 + RT1PSIE; // прерывание каждые 2 секунды
}



//обслуживание прерываний таймера А0
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
  #include  <100msTakt.c>
}




//обслуживание прерываний RTC
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
{
  switch(__even_in_range(RTCIV,16))
  {
    case 0: break;                          // No interrupts
    case 2:                                // RTCRDYIFG
        // обслуживание RTC
      /*   if (RTC_status & FRTC_not_ready)
          {
            rtc.sec   = RTCSEC & 0x3f;
	    rtc.min   = RTCMIN & 0x3f;
	    rtc.hour  = RTCHOUR & 0x1f;
	    rtc.wday  = RTCDOW & 0x03;
	    rtc.mday  = RTCDAY & 0x1f;
	    rtc.month = RTCMON & 0x0f;
	    rtc.year  = RTCYEAR;
            RTC_status &= ~FRTC_not_ready;
             RTCCTL01 &= ~RTCRDYIE; 
          }     */      
      break; 
    case 4:                                 // RTCEVIFG
        //   P1OUT ^= 0x01;
      break;
    case 6: break;                          // RTCAIFG
    case 8: break;                          // RT0PSIFG
    case 10:                                // RT1PSIFG
          __bis_SR_register(GIE);              //  разрешить прерывания
          /*
          Tp = 0;
          for(n=0; n < Polinom.NAt[7]; n++)
            Tp = Tp * T_float.f + Polinom.A_T[n]; 
          Temperature = Tp;
             
          if ((Temperature > Pass.Tmax) ||(Temperature < Pass.Tmin )) // или надо текущие?
           {// одновременно устанавливаются флаги Non_Primary_Variable_Out_of_Limits для первого и второго мастера
             Device_Status_Byte.Primary_Master |= FNon_Primary_Variable_Out_of_Limits;        
             Device_Status_Byte.Secondary_Master |= FNon_Primary_Variable_Out_of_Limits;  
           }
           else
            { // одновременно сбрасываются флаги Non_Primary_Variable_Out_of_Limits для первого и второго мастера
              Device_Status_Byte.Primary_Master &= ~FNon_Primary_Variable_Out_of_Limits;       
              Device_Status_Byte.Secondary_Master &= ~FNon_Primary_Variable_Out_of_Limits;       
            }*/
          Device_Status_Reg ^= FTemperature_Calc_Takt;
          if (!(Device_Status_Reg & FTemperature_Calc_Takt))
           Device_Status_Reg |= FTemperature_Calculate; 
      break;                         
    case 12: break;                         // Reserved
    case 14: break;                         // Reserved
    case 16: break;                         // Reserved
    default: break;
  }
}
