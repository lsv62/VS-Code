
/*! 
 * \brief m-out - обслуживание передачи по шине на базе программного SPI  в режиме мастера
 *
 *  \par  вызывается для передачи очередного байта при  обработке выходного потока данных. 
 * 
 *  
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

/*

//Bus_Control_Flags 
//#define Master_Send  1
//#define Block_PreBuf 2
//#define SSin_OK      4
#define No_Req_Slave  8
#define New_Buf_In   0x10
#define RESET 0x20
#define SPI_Test 0x40
#define Bus_Error 0x80


//Master_Control_Flags
//#define SPI_Strob_En 0x20
#define Short_Mess 0x40
*/

  {
     switch (Master_Control_Flags & 0x07)
       { case 0:                             // начало работы 
            UCB0TXBUF = SSin;     // адрес - в передатчик
            Master_Out_Index=0;   // указатель - на начало буфера
            Long_Par_Master_Out = SSin; // подсчитать контрольную сумму
            Master_Control_Flags++;     // перейти к передаче счетчика
          break; 
         case 1:                    // передача SSout        
           if (Buf_Out.buf[1])      
            {                            // есть данные для передачи
              UCB0TXBUF = Buf_Out.buf[0];           // данные - в передатчик              
              Long_Par_Master_Out += Buf_Out.buf[0];// подсчитать контрольную сумму
            }
             else
             {                //если счетчик = 0, адрес ответа передается = x81
               Master_Control_Flags |= Short_Mess; //будет только преамбула
               Long_Par_Master_Out += 0x81;   // подсчитать контрольную сумму
               UCB0TXBUF = 0x81;              // SSout - в передатчик
             }
             Master_Control_Flags++;   //передан SSout
           break;
         case 2:                        // передача счетчика         
             if (Master_Control_Flags & Short_Mess)   // короткое сообщение?   
                UCB0TXBUF = 0;              // да, передать 0.
             else
              {                           // есть данные для передачи
                Long_Par_Master_Out += Buf_Out.buf[1];  // подсчитать контрольную сумму
                UCB0TXBUF = Buf_Out.buf[1];             // счетчик - в передатчик
              }
             Master_Control_Flags++;   //передан счетчик
           break;
         case 3:                                  //передается преамбула
             if (Master_Out_Index < 6)   
              {                                            // идет преамбула
             //   Bus_Control_Flags |= Block_PreBuf;       // флаг - преамбула блокирована 
                Long_Par_Master_Out += Pre_Buf.buf[Master_Out_Index];  // подсчитать контрольную сумму
                UCB0TXBUF = Pre_Buf.buf[Master_Out_Index];   // данные - в передатчик
                Master_Out_Index ++;             // сдвинуть указатель
              } 
           else
             {      // преамбула передана
             //  Bus_Control_Flags &= ~Block_PreBuf;    //разблокировать преамбулу
               if (Master_Control_Flags & Short_Mess)    //больше данных нет ?      
                  Master_Control_Flags+=2;   //передача блока завершена
                else                  // есть данные для передачи
                  { 
                     Master_Control_Flags++;   // продолжаем передачу данных    
                     Master_Out_Index =2;      // указатель - на данные
                  }   
                UCB0TXBUF = Long_Par_Master_Out; //передается первая контрольная сумма
                Long_Par_Master_Out = 0;         // сбросить контрольную сумму
             }
          break;
        case 4:              // данные после преамбулы
           if (Master_Out_Index < (Buf_Out.buf[1]+2))    // данные переданы?
             {                                            // идет блок данных
               Long_Par_Master_Out += Buf_Out.buf[Master_Out_Index];  // подсчитать контрольную сумму
               UCB0TXBUF = Buf_Out.buf[Master_Out_Index++];           // данные - в передатчик
             } 
            else                // передача данных завершена
             {                            //передается вторая контрольная сумма
               Master_Control_Flags++;    //передача блока завершена
               if (Buf_Out.buf[2]==1)
                Bus_Control_Flags |= RESET; 
               else
                 if (!(Master_Control_Flags & Short_Mess))
                   Buf_Out.buf[1]=0;           // счетчик данных  - признак готовности к передаче -  обнулить 
              UCB0TXBUF = Long_Par_Master_Out;  // контрольная сумма - в передатчик
#ifdef  Debugging     
                if (Buf_Out.buf[0] == 0x89)      // отладка - убрать
                  HART_command_out_Counter++;      
#endif               
             }
           break;
        case 5:           // отработка приема
              if ((Master_Control_Flags & 0x18) != 0x10)   // прием завершен?                                  
                  Master_Control_Flags++;      // перейти к выключению строба
              UCB0TXBUF = 0;                   // 0 - в передатчик
           break;
         case 6:           // завершить передачу
               Strob_SPI_M_0;                   // выключить строб 
               
                P3SEL &= ~0x0e;                          // P3.1..P3.3 = SIMO, SOMI, CLK  
               UCB0CTL1 |= UCSWRST;              //  Cброс b0 канала
               // подготовка следующей передачи

               if (Bus_Control_Flags & New_Buf_In) // если данные в буфере не обслужены 
                        SSin = 0x81;               // прием не производить
                else 
                {                  
                  if (++SPI_In_Addr_Index >= 3)           // >= 8
                     SPI_In_Addr_Index=0;  
#ifdef  Debugging 
                  M2S[SPI_In_Addr_Index]++;
#endif                  
                  SSin = SPI_Addr[SPI_In_Addr_Index];        
                  if (SPI_Test_Index_Count) 
                    if (!(--SPI_Test_Index_Count))
                       SPI_Send_Count.w--;
                }
             __bic_SR_register(GIE);   //Запретить прерывания
                Pre_Buf.P =  P_Out;      //записать значения Р в SPI буфер
                Pre_Buf.DAC = DAC_Out;    //записать значения кода ЦАПа в SPI буфер
             __bis_SR_register(GIE);   // разрешить прерывания
             
               if (Bus_Control_Flags & RESET)
                  ((void(*)())*( (unsigned int *) 0xfffe))(); 
                
                for (i1=0; i1<Strob_Not_Active; i1++);     
#ifdef  Debugging                 
                if (  Bus_Control_Flags & Bus_Error)
                  Bus_Error_Counter++;
#endif                 
                Master_Control_Flags=0;
                Bus_Control_Flags &= ~0x8f;
                 
                 
                 // for (i1=0; i1<25; i1++);    
                 
                UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
                
                 P3SEL |= 0x0e;                          // P3.1..P3.3 = SIMO, SOMI, CLK  
                Strob_SPI_M_1;
                  for (i1=0; i1<Strob_Not_Active; i1++);     
                
                UCB0IE |=  UCRXIE + UCTXIE;             // Enable USCI_A0 RX/TX interrupt       
            break;
        default:      // сбой в работе        
             Bus_Control_Flags |= Bus_Error;   //ошибка протокола
            break;
  }   
}




        