// file  bus_in_service.c

 /*! 
 * \brief bus_in_service - обработка команд, полученных по SPI интерфейсу  
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

//unsigned int* ptrInt;

 if (!(Buf_In.buf[2]))
   {     // принят обслуженный ответ
      switch ( Buf_In.buf[3]) 
         {
           case 2:                    //Записать блок CRC 
                  ptr = (char*) &Buf_In.buf[4];  //установить указатель
                  for (i=6;i<Buf_In.buf[1]+4;i++)
                   { 
                    ReceiveBuffer[i] = *ptr;  
                    ptr++;
                   }   
                  ReceiveBuffer[5] = 2;   //  код НТ команды
		  ReceiveBuffer[3] = Buf_In.buf[1];
		  InitBufUSART_Tx();  
               break;              
           case 3:                   // Рассчитать или получить массив CRC 
                  ptr = (char*) &Buf_In.buf[4];  //установить указатель
                  for (i=6;i<Buf_In.buf[1]+5;i++)
                   { 
                    ReceiveBuffer[i] = *ptr;  
                    ptr++;
                   }   
                  ReceiveBuffer[5] = 1;   //  код НТ команды
		  ReceiveBuffer[3] = Buf_In.buf[1];
		  InitBufUSART_Tx();  
               break;  
               
               
           case 9:                    // Прочитать - записать номер блока
                  ptr = (char*) &Buf_In.buf[4];  //установить указатель
                  for (i=6;i<Buf_In.buf[1]+5;i++)
                   { 
                    ReceiveBuffer[i] = *ptr;  
                    ptr++;
                   }   
                  ReceiveBuffer[5] = 31;   //  код НТ команды
		  ReceiveBuffer[3] = Buf_In.buf[1];
		  InitBufUSART_Tx();  
               break;                

           case 62:                    // Прочитать  счетчики HARTа
                   ptr = (char*) &Buf_In.buf[4];  //установить указатель
                   for (i=7;i<11;i++)
                    { 
                     ReceiveBuffer[i] = *ptr;  
                     ptr++;
                    }   
                  ReceiveBuffer[5] = 6;   //  код НТ команды
                  ReceiveBuffer[6] = 2;                    
           	  ReceiveBuffer[3] =1 + 2 + 4;
		  InitBufUSART_Tx();  
               break;                 
               
           case 180:           // Записать коэффициенты термокомпенсации ЦАПа 
                  ReceiveBuffer[6] = Buf_In.buf[4]; 
                  ReceiveBuffer[7] = Buf_In.buf[5]; 
                  ReceiveBuffer[5] = 20;   // ответ на команду 20
		  ReceiveBuffer[3] = 4;

		  InitBufUSART_Tx();  
               break;         
               
               
           case 181:           // Прочитать коэффициенты термокомпенсации ЦАПа 
                   ptr = (char*) &Buf_In.buf[4];  //установить указатель
                   for (i=6;i<6+33;i++)
                    { 
                     ReceiveBuffer[i] = *ptr;  
                     ptr++;
                    }   
                  ReceiveBuffer[5] = 21;   //  код НТ команды
                    
           	  ReceiveBuffer[3] =35;
		  InitBufUSART_Tx();  
               break;         
                              
               
      /*      case 184:                  //записать упр. регистр  ***
                 Actual.Math_Mode = Buf_In.buf[8];          
                   //проверить - если вых ток не менялся не менять уставки
                 if ((Actual.Math_Mode & FSw_5mA_Off) != (User.Math_Mode & FSw_5mA_Off))
                 {
                   ptr = (char*) &Actual.DAC_Code_Min;  //установить указатель DAC_Code_4_mA
                   for(i=4;i<12;i++)
                    {
                     *ptr = Buf_In.buf[i];;  
                     ptr++;
                    }  
                 }
                 if  (Actual.Math_Mode & FTransfer_Lin_Drop  )  // обратная х-ка
                   {
                     Pup = Actual.Pmin;
                     Pdown = Actual.Pmax;
                   }
                  else
                   {                     // прямая или корнеизвлекающая х-ки
                     Pup = Actual.Pmax;
                     Pdown = Actual.Pmin;
                   }
               
                 Calculate_Span();
                 Calculate_S0();
                 Actual_2_User();   // запомнить изменения
                 SPI_Command = 5; // команда - передать статус датчика  
      
               break ;  */
           
           /*           */
        case 185:                  // передача в АЦП Ткоэффициентов ЦАПа
               if (Buf_In.buf[4] == (DAC_Tcomp_Reg & FDAC_Tcomp_N_Mask))
                {
                  switch (Buf_Out.buf[4])          // выбор блока Ткоэффициентов ЦАПа
                  {
                    case 0:
                        Buf_In.buf[2] = write_data2flash( (unsigned long int)((char *) &DAC_T_0_5 [0]), (char *) &Buf_In.buf[5], 32) ;
                        if (Buf_In.buf[2])
                           {
                             Error_Code = Write2Flash_Error;
                             DAC_Tcomp_Reg = FDAC_Tcomp_Write_Error;
                           }
                          else
                            DAC_Tcomp_Reg++;
                      break;
                    case 1:
                        Buf_In.buf[2] = write_data2flash( (unsigned long int)((char *) &DAC_T_0_5 [8]), (char *) &Buf_In.buf[5], 32) ;
                        if (Buf_In.buf[2])
                           {
                             Error_Code = Write2Flash_Error; 
                             DAC_Tcomp_Reg = FDAC_Tcomp_Write_Error;
                           }
                          else
                           {
                             DAC_Tcomp_Reg &= ~FDAC_Tcomp_New_0_5;
                             DAC_Tcomp_Reg |= FDAC_Tcomp_0_5_Enable;
                             if ( DAC_Tcomp_Reg & FDAC_Tcomp_New_4_20)
                               DAC_Tcomp_Reg++;
                           }
                      break;
                    case 2:
                        Buf_In.buf[2] = write_data2flash( (unsigned long int)((char *) &DAC_T_4_20 [0]), (char *) &Buf_In.buf[5], 32) ;
                        if (Buf_In.buf[2])
                           {
                             Error_Code = Write2Flash_Error; 
                             DAC_Tcomp_Reg = FDAC_Tcomp_Write_Error;
                           }
                          else
                            DAC_Tcomp_Reg++;                        
                      break;
                    case 3:
                        Buf_In.buf[2] = write_data2flash( (unsigned long int)((char *) &DAC_T_4_20 [8]), (char *) &Buf_In.buf[5], 32) ;
                        if (Buf_In.buf[2])
                           {
                             Error_Code = Write2Flash_Error;  
                             DAC_Tcomp_Reg = FDAC_Tcomp_Write_Error;
                           }                             
                          else
                           {
                             DAC_Tcomp_Reg &= ~FDAC_Tcomp_New_4_20;
                             DAC_Tcomp_Reg |= FDAC_Tcomp_4_20_Enable;
                           }
                      break;
                    default:  break;
                  }
                    DAC_Tcomp_Reg &= ~FDAC_Tcomp_Transfer_Active;
                    if (!(DAC_Tcomp_Reg & (FDAC_Tcomp_New_0_5 + FDAC_Tcomp_New_4_20)))
                      if  (Flash_DAC_Tcomp_Reg !=DAC_Tcomp_Reg)
                       {
                        if (write_data2flash( (unsigned long int)((char *) &Flash_DAC_Tcomp_Reg), (char *) &DAC_Tcomp_Reg, 1)) ;//запись состояния
                           Error_Code = Write2Flash_Error;  
                       }
                }

               break;     
   
         case 186:                    // записать константы режима модуля ЦАП  ***

                  ReceiveBuffer[6] = Buf_In.buf[4];  
                  ReceiveBuffer[5] = 18;   // ответ
		  ReceiveBuffer[3] = 3;
          //     !!   SPI_Command = 184;
		  InitBufUSART_Tx();  
      
               break;        
           
          case 187:                   // // прочитать константы режима модуля ЦАП  ***
              //    ReceiveBuffer[6] = DAC_Control_Reg[0]; 
              //    ReceiveBuffer[7] = DAC_Control_Reg[1]; 
                  ptr = (char*) &Buf_In.buf[4];  //установить указатель
                  for (i=6;i<Buf_In.buf[1]-2+6;i++)
                   { 
                    ReceiveBuffer[i] = *ptr;  
                    ptr++;
                   }   
                  ReceiveBuffer[5] = 19;   // ответ
		  ReceiveBuffer[3] = Buf_In.buf[1];
		  InitBufUSART_Tx();  
      
               break;  
               
      /*     case 188:        // переключить режим выходного тока без запоминания  ***

                  ptr = (char*) &Actual.DAC_Code_4_mA;  //установить указатель  
                 for(i=4;i<12;i++)
                  {
                   *ptr = Buf_In.buf[i];;  
                   ptr++;
                  }
                 Calculate_Span();
                 Calculate_S0();
                 SPI_Command = 5; // команда - передать статус датчика  
               break;       
             */  
           
          case 189:  // ответ на команду чтения АЦП модуля ЦАП
                   ptr = (char*) &Buf_In.buf[4];  //установить указатель
                   for (i=0;i<Buf_In.buf[1]-2;i++)
                   { 
                    ReceiveBuffer[6+i] = *ptr;  
                    ptr++;
                   }   
                  ReceiveBuffer[5] = 58;   // ответ
		  ReceiveBuffer[3] = Buf_In.buf[1];
		  InitBufUSART_Tx();              
              break; 
           
           
          case 190:  // ответ на команду проверки интерфейса
                SPI_Test_Buf[Buf_In.buf[6]].c[0] = Buf_In.buf[4] ; // данные - в буфер
                SPI_Test_Buf[Buf_In.buf[6]].c[1] = Buf_In.buf[5] ; // для отправки в канал технологии               
              break;
           default:    break;
          }
                
    }
 else  //команда для исполнения
 {
   if ((Buf_In.buf[0] == 0x89)&& ((Buf_In.buf[2] == 128)||(Buf_In.buf[2] == 129)))     // HART команда
   {
     Bus_Control_Flags |= FHART_CMD_Active;
#ifdef  Debugging      
     HART_In_Message_Counter++;
#endif      
     #include "HART_command.c"

     
   }
    else
      switch ( Buf_In.buf[2]) 
       {
         case 29:  // запрос ID датчика
               SPI_Command = 4; // команда - передать ID датчика   
              break;
         case 30:  // запрос описания датчика
               SPI_Command = 6; // команда - передать описание датчика             
             break;   
       //**********************************************************      
         case 31:  // user unit saving    
               if ( write_data2flash( (unsigned long int)((char *) &User_P_unit), (char *) &Buf_In.buf[3], 18) )
                    Error_Code = Write2Flash_Error; 
               SPI_Command = 6; // команда - передать термокоэффициенты             
             break;         
    //**************************************
         case 32:  // запрос статуса датчика
               SPI_Command = 5; // команда - передать статус датчика             
             break;
         case 33:  // изменение статуса датчика                       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       
              ptr = (char*) &Actual.Math_Mode;       //установить указатель
               for (i=3;i<37;i++)
                 { 
                  *ptr = Buf_In.buf[i]; 
                   ptr++;
                 } 
              if (!(Actual.Filter_Limit))
                       dampfer_zero();

              if (!(F0_5_Module_Set & (DAC_Status<<8)))
                Actual.Math_Mode |= FSw_5mA_Off;  
              
              #include "Pup_Pdown_select.c"
              #include "Iout_regim_set.c"
              Calculate_Span();
              Calculate_S0();
              Actual_2_User();
              SPI_Command = 5; // команда - передать статус датчика     

             break;
        case 34:          //команда - установить зав. значения
                Plant_2_User();
                // это уже сделано в Plant_2_User :  SPI_Command = 5; // команда - передать статус датчика */
             break;     
             
        case 35:          //команда - установить 0
                zero_shift_set() ; 
                // это уже сделано в zero_shift_set : SPI_Command = 5; // команда - передать статус датчика */
             break;

        case 36:        // команда установить код ЦАПа
               DAC_Count = 255;  //таймер зарядить
           //    Device_Status_Byte.Primary_Master |= FLoop_Current_Fixed;
           //    Device_Status_Byte.Secondary_Master |= FLoop_Current_Fixed;
               DAC_Code.c[0] = Buf_In.buf[3];
               DAC_Code.c[1] = Buf_In.buf[4];
             break;
        case 37:       // команда установить сдвиг ЦАПа 
               Shift_Count = 255; //таймер поставить
               Actual.DAC_Code_Shift = Buf_In.buf[3] + (Buf_In.buf[4]<<8);
             break;   
        case 38:       // команда установить сдвиг нуля 
               Shift_Count = 255; //таймер поставить
               Float_Buf_Tmp.c[0]= Buf_In.buf[3];  
               Float_Buf_Tmp.c[1]= Buf_In.buf[4];
               Float_Buf_Tmp.c[2]= Buf_In.buf[5];  
               Float_Buf_Tmp.c[3]= Buf_In.buf[6];
               Actual.Zero_Shift = Float_Buf_Tmp.f;

             break;  
       case 39:  // Запросить температуру cенсора
          //     SPI_Command = 8; // команда - передать температуру            
             break;              
       case 40:          //  команда - тест HARTа
            if (Buf_In.buf[3] & 0x80) 
              {     // local address
                  Buf_In.buf[3] &= ~0x80;
                  if ( write_data2flash( (unsigned long int)((char *) &Addr_H_Long[0]), (char *) &Buf_In.buf[3], 1) )
                     {
                       Error_Code = Write2Flash_Error; 
                     }
                    Buf_In.buf[8] = Flash_Device_Status_Byte.Primary_Master;   //Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed
                    Buf_In.buf[9] = Flash_Device_Status_Byte.Secondary_Master;
                    if (!(Addr_H_Long[0]))
                      {
                        Buf_In.buf[8] &= ~(FLoop_Current_Fixed + FLoop_Current_Not_Active); // разблокировать токовый выход   
                        Buf_In.buf[9] &= ~(FLoop_Current_Fixed + FLoop_Current_Not_Active);
                      }
                     else
                      {
                        Buf_In.buf[8] |= FLoop_Current_Fixed + FLoop_Current_Not_Active; // заблокировать токовый выход  
                        Buf_In.buf[9] |= FLoop_Current_Fixed + FLoop_Current_Not_Active; // заблокировать токовый выход  
                        Buf_In.buf[10] = Actual.DAC_Code_4_mA;// DAC_Code_Fixed = Actual.DAC_Code_4_mA;
                        Buf_In.buf[11] =  (Actual.DAC_Code_4_mA&0xff00)>>8; 
                      }
                    if ( write_data2flash( (unsigned long int)((char *) &Flash_Device_Status_Byte.Primary_Master), (char *) &Buf_In.buf[8], 4) )
                     {
                       Error_Code = Write2Flash_Error; 
                       Buf_Out.buf[4] = 6;  //  Device-Specific Command Error                     
                     }
               Cnfg_Counter_Increment(); 
               SPI_Command = 60; 
              }
             else
              {
               if (Buf_In.buf[3] & 0x40)
                 {    // write protect
                   Buf_In.buf[3] &= ~0x40;
                   if ( write_data2flash( (unsigned long int)((char *) &HART_write_protect_value), (char *) &Buf_In.buf[3], 1) )
                     {
                       Error_Code = Write2Flash_Error; 
                     }
		  // SPI_Command = 5; // сообщить об изменении write_protect_value
                 }
                else
                 {
                  HART_Test_Pattern_Code =Buf_In.buf[3];
                  if (HART_Test_Pattern_Code<5)
                    SPI_Command = 61; // команда -  передать команду - тест HARTа 
                   else
                    {
                     if (HART_Test_Pattern_Code==5)
                      {
                       HART_Test_Ctrl_Register &= ~FTest_Meander_Mask;
                       Hart_Meander_Test_Counter = 26;
                       Meander_Pulse_Counter = 0;
                      }
                      else
                       {
                        Additional_Device_Status.Standardized_Status_0 |= FSimulation_Active ; // взводится флаг симуляции
                        Device_Status_Byte.Primary_Master |= FMore_Status_Available;        // одновременно устанавливается флаги More Status Available для первого и второго мастера
                        Device_Status_Byte.Secondary_Master |= FMore_Status_Available;
                       }
                    }
                 }
              }
           break; 
 
  /*      case 59:  // запрос длинного HART адреса
               SPI_Command = 60; // команда -  передать длинный HART адрес      
           break;     */ 
       
        case 79:          //команда - сверить ЦАП ID и регистр управления ***
              ptr = (char*) &DAC_Copy.DAC_Code_4_mA;  //установить указатель
               for (i=8;i<16;i++)
               { 
                *ptr  = Buf_In.buf[i];  
                   ptr++;
               }    
              if (( Buf_In.buf[7] != DAC_Status ) || (Buf_In.buf[3] != Attached_DAC_ID[0]) || (Buf_In.buf[6] != Attached_DAC_ID[3]) || (Buf_In.buf[4] != Attached_DAC_ID[1]) || (Buf_In.buf[5] != Attached_DAC_ID[2]))
              { // новый ЦАП
                if ( write_data2flash( (unsigned long int)((char *) &Attached_DAC_ID[0]), (char *) &Buf_In.buf[3], 5) )
                    Error_Code = Write2Flash_Error; 
                ptr = (char*) &Actual.DAC_Code_4_mA;  //установить указатель
                for (i=8;i<16;i++)
                 { 
                   *ptr  = Buf_In.buf[i];  
                   ptr++;
                 }
                if (FT_4_20_Calibration_Executed & (DAC_Status<<8))
                 {
                   DAC_Tcomp_Reg = 2;
                   DAC_Tcomp_Reg |= FDAC_Tcomp_New_4_20;
                 }
                if (FT_0_5_Calibration_Executed & (DAC_Status<<8))
                 {
                   DAC_Tcomp_Reg &= ~FDAC_Tcomp_N_Mask;
                   DAC_Tcomp_Reg |= FDAC_Tcomp_New_0_5;
                 }

                if ((!(F4_20_Module_Set & (DAC_Status<<8))) && (F0_5_Module_Set & (DAC_Status<<8)))
                  Actual.Math_Mode &= ~FSw_5mA_Off;                 
                if (!(F0_5_Module_Set & (DAC_Status<<8)))
                  Actual.Math_Mode |= FSw_5mA_Off;  
 
                
                Actual_2_User();
                #include "Iout_regim_set.c"

                Calculate_Span();
                Calculate_S0();
                SPI_Command = 5; // команда - передать статус датчика  
              }
              if ((Buf_In.buf[16] !=  Actual.Math_Mode) || (Buf_In.buf[17] !=  Mode_Setting_Reg))   
                SPI_Command = 184; 

             break; 
           
         case 80:          //команда - получить температуру
                   __disable_interrupt(); 
                   DAC_Temperature.c[0] = Buf_In.buf[3]; 
                   DAC_Temperature.c[1] = Buf_In.buf[4];      
                   __enable_interrupt(); 
             break; 
         case 85:          //команда - превышено напряжение
 
                   tmp_Buf.c[0] = Buf_In.buf[3]; 
                   tmp_Buf.c[1] = Buf_In.buf[4]; 
                   
                   Jurnal_Record.Param_Code=3;
                   Jurnal_Record.Units_Code = 58;  // вольт по кодировке харта
                   Jurnal_Record.Parametr  = (float)tmp_Buf.w;     //(float)tmp_Buf.w * K_adc_U
                   write_record_2_journal(); //Записать в журнал    
                  
             break;       
         default:    break;
        }
       }
      
       
 