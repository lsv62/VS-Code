
/*! 
 * \brief SPI_init - инициализация интерфейса SPI
 *
 *  \par  
 * 
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void SPI_init(void) 
{
 P3DIR |= 0x0a;
 P3DIR &= ~0x04;
 P3OUT &= ~0x0a;
 
   UCB0CTL1 |= UCSWRST;              //  Cброс b0 канала
  /* отладка*/
 P3SEL |= 0x0e;                          // P3.1..P3.3 = SIMO, SOMI, CLK  

 UCB0CTL0 |= UCMST + UCMSB + UCSYNC; //+UCCKPL  + UCCKPH !!!!  // 3-pin, 8-bit SPI master
                                            // Clock polarity high, MSB
 UCB0CTL1 |= UCSSEL1 + UCSSEL0;       // SMCLK  
 UCB0BR0=DividerSPI0;                      
 UCB0BR1=DividerSPI1;                  // UCA0BR0,1=SMCLK/9600 
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

   Strob_SPI_M_0; 
  Pdir_Strob_SPI_M |= Strob_SPI_M;    // вкл. на выход
/* отладка*/
  // старт
   Buf_Out.buf[1]=0;           // счетчик данных  - признак готовности к передаче -  обнулить  
   Strob_SPI_M_1;
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCB0IE |=  UCRXIE + UCTXIE;             // Enable USCI_b0 RX/TX interrupt  
  Master_Control_Flags=0;
  Bus_Control_Flags = 0; // &= 0x8f;

}


unsigned char SPIRXBUF, itest=0;


// подпрограмма обслуживания прерывания передачи модуля USCI
#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  int i1;
   switch(UCB0IV&7)
  {
    case 0:break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
         SPIRXBUF = UCB0RXBUF;
     //    Test_inBuf[itest++]=SPIRXBUF;
         #include "m-in.c"   
        
   //   break;
    case 4:                                    // Vector 4 - TXIFG
   // UCB0TXBUF = SSin; 
       
        #include "m-out.c"
        UCB0IE &=  ~UCTXIE;
      break;                             
    default: break;
  }
}


