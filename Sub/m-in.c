

/*! 
 * \brief m-in - обслуживание передачи по шине на базе программного SPI  в режиме мастера
 *
 *  \par  запускается при получении очередного байта для обработки входного потока данных 
 *  Формат данных в буфере - см. SPI.xls
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/


  switch (Master_Control_Flags & 0x18)
  { case 0:                             // не было передачи от слейва
           if (SPIRXBUF)   // принят SS?
            {
              
#ifdef  Debugging     
              Bus_Input_Point1_Counter++;        
#endif              

              if (SPIRXBUF != SSin)    
              { 
#ifdef  Debugging    
                 ERROR[ERROR_INDEX++]=SPIRXBUF;
                 if (ERROR_INDEX>=10)
                   ERROR_INDEX=0;
                Bus_Error_Point1_Counter++;                    
                Master_Control_Flags |= 16;               
#endif                
                Bus_Control_Flags |= Bus_Error;       //ошибка протокола - адрес опроса не совпал.                
              }
              else
              {
#ifdef  Debugging                   
                S2M[SPI_In_Addr_Index]++;
#endif                 
              }
              
              Long_Par_Master_In = SPIRXBUF;              // начать подсчет контрольной суммы
              Buf_In.buf[0]= SPIRXBUF;                    // сохранить в буфер
              Master_Control_Flags |= 8;                  // перейти к приему счетчика
            }
        break;
    case 8:                      // счетчик                 
         Long_Par_Master_In += SPIRXBUF;              //подсчет контрольной суммы
         Buf_In.buf[1]= SPIRXBUF;                     // сохранить в буфер
         if (Buf_In.buf[1] > (sizeof(Buf_In.buf)-2))  // счетчик больше размера буфера                  
          {
            Bus_Control_Flags |= Bus_Error;   //ошибка протокола
#ifdef  Debugging     
            Bus_Error_Point2_Counter++;    
#endif             
            /*  отладка
            //выдать отладочный строб
            P4DIR |= 0x01;
            P4OUT |= 0x01;
            P4OUT &= ~0x01;
             */
            Master_Control_Flags += 16;       // прием прекратить
          }
         else
           Master_Control_Flags += 8;   // счетчик активен   
         Master_In_Index=2;             // подготовиться к приему данных 
   /*
         if ((SSin == 0x8d) &&  (Buf_In.buf[1]))
         {
          SPI_error_count++;        
         }
*/
        break;     
              
    case 16:                   // Прием отклика от слейва 
         if (Master_In_Index > (Buf_In.buf[1]+1)) // Прием завершен?  
           {      // да,           
             if (Long_Par_Master_In != SPIRXBUF)  // контрольная сумма совпала?
              {
                Bus_Control_Flags |= Bus_Error;   //ошибкa контрольной суммы                 
#ifdef  Debugging     
                Bus_Error_Point3_Counter++;  
#endif                 

              }
              else
              {
                if (Buf_In.buf[1])                 // есть данные от слейва?
                  {
                    Bus_Control_Flags |= New_Buf_In; // данные в буфере валидны 
#ifdef  Debugging     
                    if (SSin == 0x89)
                      HART_command_in_Counter++;      
#endif                     
                  }
                 else
                    Bus_Control_Flags |= No_Req_Slave; // обслуживание не нужно
               }
              Master_Control_Flags += 8; // прием завершить, перейти к простой вычитке данных..
             
           } 
          else     // нет, продолжается прием
           {   
               Long_Par_Master_In += SPIRXBUF;  //подсчет контрольной суммы
               Buf_In.buf[Master_In_Index++]= SPIRXBUF;  // сохранить в буфер
           }      
       
        break;  
      case 24:                      //продолжаем принимать,все игнорируя  
          
        break;  
  default:      // сбой в работе        
         Bus_Control_Flags |= Bus_Error;   //ошибка протокола
#ifdef  Debugging     
         Bus_Error_Point4_Counter++;    
#endif                  
        break;   
  }

  

