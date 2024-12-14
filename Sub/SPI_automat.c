
 if (SPI_In_Addr_Index >= 8)
           SPI_In_Addr_Index=0;
 if (Bus_Control_Flags & New_Buf_In)
       SSin = 0x81;
  else 
       SSin = SPI_Addr[SPI_In_Addr_Index++]; 

// __bic_SR_register(GIE);   //Запретить прерывания
 Pre_Buf.P = P_Out;        //записать значения Р в SPI буфер
 Pre_Buf.DAC = DAC_Out;    //записать значения кода ЦАПа в SPI буфер
// __bis_SR_register(GIE);   // разрешить прерывания
 
    Strob_SPI_M_1;
    UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCB1IE |=  UCRXIE + UCTXIE;             // Enable USCI_A0 RX/TX interrupt  
  Master_Control_Flags=0;
  Bus_Control_Flags &= 0x8f;   
__bis_SR_register_on_exit(GIE);   // разрешить прерывания