                   
 // file bus_out_service.c



 /*! 
 * \brief bus_out_service - обработка команд SPI интерфейсу   
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/


if(Bus_Control_Flags & SPI_Test) // тестирование интерфейса?
  {                              // да,
    if (SPI_Send_Count.w > 2)    // идет подсчет передач?
     {                            // да, 
        if (--SPI_Send_Count.w == 2) //  последняя посылка?
           SPI_Out_Addr_Index=0;        // да, подготовиться к опросу   
        Buf_Out.buf[0] = 0x81;  // общий выходной адрес 
        Buf_Out.buf[2] = 190;   // команда
        Buf_Out.buf[3] = 0x0f;  // признак подсчета   
        Buf_Out.buf[1] =2;       // разрешить выдачу информации 
        
     }
     else                             //нет, 
      if (SPI_Send_Count.w == 2)        // идет опрос датчиков?
      {
       if (SPI_Out_Addr_Index == 7)       // все датчики опрошены?
        {
         //   SPI_Out_Addr_Index =0;
            SPI_Send_Count.w--;          // завершить опрос    
            SPI_Test_Index_Count = 8;    // подготовиться к выдаче информации в канал технологии
        }
        Buf_Out.buf[0] = SPI_Addr[SPI_Out_Addr_Index];  // текущий адрес считывания 
        
        Buf_Out.buf[2] = 190;   // команда
        Buf_Out.buf[3] = 0;   // снять информацию  
        Buf_Out.buf[4] = SPI_Out_Addr_Index++;   // номер устройства  
        Buf_Out.buf[1] =3;     // разрешить выдачу информации 
        
      }
  }
 else
  {
   switch(SPI_Command)   // выдача информации в буфер
   {  
     case 0:   // команды нет - проверяем коррекцию по температуре ЦАПа
         if (DAC_Tcomp_Reg & (FDAC_Tcomp_New_0_5 + FDAC_Tcomp_New_4_20))
           if (!(DAC_Tcomp_Reg & FDAC_Tcomp_Transfer_Active))
            {  
              DAC_Tcomp_Reg |= FDAC_Tcomp_Transfer_Active;
              Buf_Out.buf[0] = 0x85;  // адрес получателя - ЦАП
              Buf_Out.buf[2] = 185;   // команда - передача в АЦП Ткоэффициентов ЦАПа
              Buf_Out.buf[3] = DAC_Tcomp_Reg & FDAC_Tcomp_N_Mask ; // номер блока Ткоэффициентов ЦАПа
              Buf_Out.buf[1] = 2; 
            }

       break;
     case 1:     // reset
	 __delay_cycles(2000000);
         Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[1] =1;
          SPI_Command = 0;
       break;
      case 2:    //Записать блок CRC
          switch(ReceiveBuffer[5]) 
          {
            case 1:     
               Buf_Out.buf[0] = 0x85;  // адрес получателя - ЦАП
              break;
            case 2:     
               Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
              break;
            case 3:     
               Buf_Out.buf[0] = 0x8d;  // адрес получателя - индикатор 
              break;    
            case 4:     
               Buf_Out.buf[0] = 0x95;  // адрес получателя - zigbee
              break;
            case 5:     
               Buf_Out.buf[0] = 0xa5;  // адрес получателя - profibus
              break;
            case 6:     
               Buf_Out.buf[0] = 0xb9;  // адрес получателя - резерв 
              break;   
            default:
      //         Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
              break; 
           }  
          Buf_Out.buf[2] = SPI_Command;   // команда 
          ptr = (char*) &ReceiveBuffer[5];  //установить указатель
          for (i=0;i<count;i++)
           { 
             Buf_Out.buf[3+i]  = *ptr;  
             ptr++;
           }   
         
          Buf_Out.buf[1] =count+1;
          SPI_Command = 0;
         break;  


       case 3:     // Рассчитать или получить массив CRC 
        
          switch(ReceiveBuffer[5]) 
          {
            case 1:     
               Buf_Out.buf[0] = 0x85;  // адрес получателя - ЦАП
              break;
            case 2:     
               Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
              break;
            case 3:     
               Buf_Out.buf[0] = 0x8d;  // адрес получателя - индикатор 
              break;    
            case 4:     
               Buf_Out.buf[0] = 0x95;  // адрес получателя - zigbee
              break;
            case 5:     
               Buf_Out.buf[0] = 0xa5;  // адрес получателя - profibus
              break;
            case 6:     
               Buf_Out.buf[0] = 0xb9;  // адрес получателя - резерв 
              break;   
            default:
    //           Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
              break; 
           }   
          Buf_Out.buf[2] = SPI_Command;   // команда
          Buf_Out.buf[3] = ReceiveBuffer[5]; //номер устройства
          Buf_Out.buf[4] = ReceiveBuffer[6]; //тип операции
          Buf_Out.buf[1] = 3;
          SPI_Command = 0;
        break;       
       
       
       
      case 4:                  // передать модель и номер датчика
          Buf_Out.buf[0] = 0x8d;  // адрес получателя -  адрес индикатора  0x85, 0x89, 0x8d
          Buf_Out.buf[2] = SPI_Command;   // команда
         
          ptr = (char*) &Pass_ID.Model[0];    //установить указатель
          for (i=3;i<11;i++)
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
         ptr = (char*) &Pass_ID.Zav_Number[0];    //установить указатель
         for (;i<23;i++)
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
         i--;
         Buf_Out.buf[1] = --i;
         SPI_Command = 0;
        break;        
     case 5:                  // передать статус датчика
         Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
         Buf_Out.buf[2] = SPI_Command;   // команда
         
         ptr = (char*) &Actual.Math_Mode;       //установить указатель
         for (i=3;i<37;i++)     //последний - коэффициент усиления пользователя  34
          { 
            Buf_Out.buf[i] = *ptr; 
            ptr++;
          } 
         // HART status 29.03
         Buf_Out.buf[i] = Addr_H_Long[0];     // Buf_Out.buf[37]
         
         if ((Burst_Message[0].Burst_Mode_Control_Code & 0x01)||(Burst_Message[1].Burst_Mode_Control_Code & 0x01)||(Burst_Message[2].Burst_Mode_Control_Code & 0x01))
           Buf_Out.buf[i] |= 0x80;
           
         if (HART_write_protect_value & 0x01)
           Buf_Out.buf[i] |= 0x40;
         
      //   i--;
         // HART status 29.03 end
         Buf_Out.buf[1] = --i;
         SPI_Command = 60;  
        
        break; 
    /*    */ 
     case 6:                  // передать описание датчика
          Buf_Out.buf[0] = 0x81;  // адрес получателя - общий - поменять на адрес индикатора  0x85, 0x89, 0x8d
          Buf_Out.buf[2] = SPI_Command;   // команда

         
          ptr = (char*) &Pass.P_Unit;    //установить указатель
          for (i=3;i<13;i++)
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
         Buf_Out.buf[i++] = DAC_Status; 
         /*************************************************/
         Buf_Out.buf[i++] = Mode_Setting_Reg;  // надо ли?
         
         ptr = (char*) &User_P_unit;    //установить указатель
         for (;i<33;i++)  // 13+2+18
          { 
            Buf_Out.buf[i] = *ptr; 
            ptr++;
          } 
         
         i--;
       
         /*************************************************/
         Buf_Out.buf[1] = --i;
         SPI_Command = 0;
        break;    
       
            /*    
     case 7:                  // передать термокоэффициенты
          Buf_Out.buf[0] = 0x81;  // адрес получателя - общий - поменять на адрес индикатора  0x85, 0x89, 0x8d
          Buf_Out.buf[2] = SPI_Command;   // команда

          ptr = (char*) &Polinom.A_T[0];    //установить указатель
          for (i=3;i<19;i++)
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
         Buf_Out.buf[i] = Polinom.NAt[7]; 
         Buf_Out.buf[1] = --i;
         SPI_Command = 0;
        break; 
         */
     case 8:                  // передать температуру
    /*  */
    //    if ( Buf_Out.buf[1] )
    //      break;
          Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
          Buf_Out.buf[2] = SPI_Command;   // команда 
         ptr = (char*)&Temperature; 
      //   __bic_SR_register(GIE);   //Запретить прерывания          
         for (i=3;i<7;i++)
          { 
           Buf_Out.buf[i] = *ptr++ ; 
          }   
         
         Buf_Out.buf[1] = 5; 
         SPI_Command = 0;
  //       __bis_SR_register(GIE);   // разрешить прерывания          
        break;           
      
  case 9:      // Прочитать - записать номер блока
        
         switch(ReceiveBuffer[5]&0x7f) 
          {
            case 1:     
               Buf_Out.buf[0] = 0x85;  // адрес получателя - ЦАП
              break;
            case 2:     
               Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
              break;
            case 3:     
               Buf_Out.buf[0] = 0x8d;  // адрес получателя - индикатор 
              break;    
            case 4:     
               Buf_Out.buf[0] = 0x95;  // адрес получателя - zigbee
              break;
            case 5:     
               Buf_Out.buf[0] = 0xa5;  // адрес получателя - profibus
              break;
            case 6:     
               Buf_Out.buf[0] = 0xb9;  // адрес получателя - резерв 
              break;   
            default:
 //              Buf_Out.buf[0] = 0x81;  // адрес получателя - общий 
              break; 
           }   
          Buf_Out.buf[2] = SPI_Command;   // команда
       //   count = ReceiveBuffer[3]-1;
          ptr = (char*) &ReceiveBuffer[5];    //установить указатель
          for (i=3;i<ReceiveBuffer[3]+2;i++)
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
           Buf_Out.buf[1] = i - 2;         
/*          
          Buf_Out.buf[3] = ReceiveBuffer[5]; //номер устройства
          if  (ReceiveBuffer[5] & 0x80) // Запись?
            {  
              Buf_Out.buf[4] = ReceiveBuffer[6]; 
              Buf_Out.buf[5] = ReceiveBuffer[7]; 
              Buf_Out.buf[6] = ReceiveBuffer[8]; 
              Buf_Out.buf[7] = ReceiveBuffer[9];
              Buf_Out.buf[8] = ReceiveBuffer[10];
              if (ReceiveBuffer[10]) // есть продолжение?
               {
                       // работа с дополнительными параметрами
               }
              Buf_Out.buf[1] = 6;          
            }
           else                          // чтение
            {
              Buf_Out.buf[1] = 2;
            }
*/
          SPI_Command = 0;
        break;               
        
     case 11:                  // вывести строку сообщения
         Buf_Out.buf[0] = 0x8d;  // адрес получателя - индикатор
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3] = Display_Msg;     // Номер строки сообщения
    
         Buf_Out.buf[1] =2;
          SPI_Command = 0;
       
        break; 
      case 12:                  // команда - поворот экрана
         Buf_Out.buf[0] = 0x8d;  // адрес получателя - индикатор
         Buf_Out.buf[2] = SPI_Command;   // команда
    
         Buf_Out.buf[1] =1;
          SPI_Command = 0;
       
        break;  
        
      case 13:   // сообщение об ошибке
                //  Error_Code =  Buf_In.buf[3];
                 // view_message (*((unsigned char const**)&LineError + Error_Code-1));// ??? error
                  //Count_Indik_SPI = 15;
          break; 
     /* 
          
      case 59:   // ACK 
                //  Error_Code =  Buf_In.buf[3];
                 // view_message (*((unsigned char const**)&LineError + Error_Code-1));// ??? error
                  //Count_Indik_SPI = 15;
          break;           
    */      
      case 60:   //   передать  HART адрес и константы 
        //  Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
          Buf_Out.buf[0] = 0x81;  // адрес получателя - общий - для замены короткого адреса в индикаторе
          Buf_Out.buf[2] = SPI_Command;   // команда
         
          ptr = (char*) &Addr_H_Long[0];    //установить указатель
          for (i=3;i<9;i++)             
           { 
             Buf_Out.buf[i] = *ptr; 
             ptr++;
           } 
          Buf_Out.buf[i++] = HART_Cmd_0_Data[12]; // количество преамбул от датчика

          for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
           {
            if (P_Unit[j].Code == Actual.P_Unit)                   
            break;
           }
          Buf_Out.buf[i++]  = P_Unit[j].Code;          // единицы измерения
          ptr =  (char*)&P_Unit[j].Kout_P[0];           //Kout_PV;          
            for (;i<15;i++)               //i=11
             { 
               Buf_Out.buf[i] = *ptr++; 
             }
          ptr =  (char*)&Actual.Pmin;        // Primary_Variable_Min
            for (;i<19;i++)               //i=15
             { 
               Buf_Out.buf[i] = *ptr++; ; 
             }            
            ptr =  (char*)&K_PerCent;        // K_PerCent
            for (;i<23;i++)               //i=19
             { 
               Buf_Out.buf[i] = *ptr++; 
             }             
            ptr =  (char*)&K_Loop_Current;   // K_Loop_Current
            for (;i<27;i++)               //i=23
             { 
               Buf_Out.buf[i] = *ptr++; 
             }            
            ptr =  (char*)&Base_Loop_Current;  // Base_Loop_Current
            for (;i<31;i++)               //i=27
             { 
               Buf_Out.buf[i] = *ptr++; 
             }
            ptr =  (char*)&Code_Down;     
            Buf_Out.buf[i++] = *ptr++;
          
            Buf_Out.buf[i] = *ptr; 
          
          Buf_Out.buf[1] = --i;
          // включить передачу данных бурст-режима.
          if (Device_Status_Reg & FBurst_Data_Copy_Active)
           {
             Burst_Message_Number = 0;
             SPI_Command = 65; 
             Device_Status_Reg &= ~FBurst_Data_Copy_Active;
           }
           else
             SPI_Command = 0;  
	//     SPI_Command = 5; // сообщить об изменении PollAddr 
        break;
                      
      case 61:   // команда -  передать команду - тест HARTа 
          Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
          Buf_Out.buf[2] = SPI_Command;   // команда
          Buf_Out.buf[3]= HART_Test_Pattern_Code;      // послать код паттерна  

          Buf_Out.buf[1] = 2;
          SPI_Command = 0;          
        break;
        
      case 63:                 //Burst Period
         Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
         Buf_Out.buf[2] = SPI_Command;   // команда      
         Buf_Out.buf[3] = Burst_Message_Number;
	 /*
         ptr = (char*)(&Burst_Message[Burst_Message_Number].Burst_Mode_Control_Code + ((unsigned char const*)&Burst_Message[0].Update_Time-&Burst_Message[0].Burst_Mode_Control_Code));
         for (i=4;i<12;i++)        // Update_Time + Max_Update_Time                             
          Buf_Out.buf[i] = *ptr++;  
         */
         ptrMessage = (char*)(&Burst_Message[Burst_Message_Number].Burst_Mode_Control_Code + ((unsigned char const*)&Burst_Message[0].Update_Time-&Burst_Message[0].Burst_Mode_Control_Code));
	 ptr = (char*)&Time_Tmp_Buf1; 
	 for (i=4;i<12;i++)        // Update_Time + Max_Update_Time 
	   *ptr++ = *ptrMessage++;
	          Float_Buf_Tmp.f = Time_Tmp_Buf1*0.016;   //0.0105;//0.0086; //0.015625;            // (1.05*1/7.8125/8.0/2.0)
                  Time_Tmp_Buf1 = (unsigned long int) (Float_Buf_Tmp.f+.5);
                  Float_Tmp_Buf = Time_Tmp_Buf2*0.016;   //0.0105;//0.0086; //0.015625; // (1.05*1/7.8125/8.0/2.0)
                  Time_Tmp_Buf2 = (unsigned long int) (Float_Tmp_Buf+.5);
         ptr = (char*)&Time_Tmp_Buf1;
	 for (i=4;i<12;i++)        // Update_Time + Max_Update_Time                             
          Buf_Out.buf[i] = *ptr++;  
         Buf_Out.buf[1] = 12-2;
          SPI_Command = 0; 
        break;        
        
     case 64:   //  Burst Trigger
          Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
          Buf_Out.buf[2] = SPI_Command;   // команда  
          Buf_Out.buf[3] = Burst_Message_Number;
          ptrMessage = (char*)&Burst_Message[Burst_Message_Number].Trigger_Mode;   //установить указатель
          ptr = ptrMessage + ((char*)&Burst_Message[0].Trigger_Value-(char*)&Burst_Message[0].Trigger_Mode);
          
          Buf_Out.buf[4] = *ptrMessage;
	  
	 if (*(ptr+3) == 0xff)
	  {
	  ptr+=3;  // %)
          for (i=5;i<9;i++)                                   
            { 
             Buf_Out.buf[i] = *ptr--;   // Trigger_Value
            } 
	  }
	  else  
          for (i=5;i<9;i++)                                   
            { 
             Buf_Out.buf[i] = *ptr++;   // Trigger_Value
            } 
          Buf_Out.buf[1] = 9-2;
          SPI_Command = 0; 
        break;            
         
      case 65:                    // передача данных бурст режима.
          Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
          Buf_Out.buf[2] = SPI_Command;   // команда  
          Buf_Out.buf[3] = Burst_Message_Number;
          ptrMessage = (char*)&Burst_Message[Burst_Message_Number].Burst_Mode_Control_Code;
          for (i=4;i<10+4;i++)                                   
            { 
              Buf_Out.buf[i] = *ptrMessage++;   
            }
	  
	  ptr = (char*)&Time_Tmp_Buf1; 
	  for (;i<18+4;i++)        // Update_Time + Max_Update_Time 
	   *ptr++ = *ptrMessage++;
	          Float_Buf_Tmp.f = Time_Tmp_Buf1*0.016;   //0.0105;//0.0086; //0.015625; //(1.05*1/7.8125/8.0/2.0)
		  Float_Buf_Tmp.f+= .5;
                  Time_Tmp_Buf1 = (unsigned long int) (Float_Buf_Tmp.f);
                  Float_Tmp_Buf = Time_Tmp_Buf2*0.016;   //0.0105;//0.0086; //0.015625; //(1.05*1/7.8125/8.0/2.0)
		  Float_Tmp_Buf += .5;
                  Time_Tmp_Buf2 = (unsigned long int) (Float_Tmp_Buf);
         ptr = (char*)&Time_Tmp_Buf1;
	 for (i=10+4;i<18+4;i++)        // Update_Time + Max_Update_Time                             
          Buf_Out.buf[i] = *ptr++; 
	  
	  
          ptrMessage++;                     // propusk byte'a
          Buf_Out.buf[i++] = *ptrMessage++;  //  Trigger_Mode
          ptrMessage++;                     // Device_Variable_Classification
	  ptrMessage++;                     // Units_Code
	  if (*(ptrMessage+3) == 0xff)
	  {
	  ptrMessage+=3;  // %)
          for (;i<23+4;i++)                                   
            { 
	      Buf_Out.buf[i] = *ptrMessage--;   // %)  // Trigger_Value
            //  Buf_Out.buf[i] = *ptrMessage++;   //Trigger_Value
            }
	  }
	  else
	   for (;i<23+4;i++)                                   
            { 
              Buf_Out.buf[i] = *ptrMessage++;   //Trigger_Value
            }
	    
          Buf_Out.buf[1] = 27-2;
          if (++Burst_Message_Number >2)
           {
             SPI_Command = 0; 
           }  
        break;         
 /*        
      case 66:   // команда -  сбросить счетчики HARTа
          Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
          Buf_Out.buf[2] = SPI_Command;   // команда
          Buf_Out.buf[1] = 1;
          SPI_Command = 0;          
        break;                          
     */   
      case 67:                 //Burst Device Variables
         Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3] = Burst_Message_Number;
         ptr = (char*)(&Burst_Message[Burst_Message_Number].Burst_Mode_Control_Code + (&Burst_Message[0].Slot[0]-&Burst_Message[0].Burst_Mode_Control_Code));
         for (i=4;i<12;i++)     
          { 
            Buf_Out.buf[i] = *ptr++;
          }
         ptr += &Burst_Message[0].Trigger_Mode - &Burst_Message[0].Command_Number ;
         Buf_Out.buf[i] = *ptr;
         Buf_Out.buf[1] = 13-2;
         /*
         if (Bus_Control_Flags & FHART_Trigger_Reconfig)
           {
             SPI_Command = 64; 
             Bus_Control_Flags &= ~FHART_Trigger_Reconfig;
           }
          else
         */
           SPI_Command = 0; 
        break;  
        
      case 68:                 //Burst Mode Command Number
         Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3] =  Burst_Message_Number;
         
         Buf_Out.buf[4] = Burst_Message[Burst_Message_Number].Command_Number;
         Buf_Out.buf[5] = Burst_Message[Burst_Message_Number].Trigger_Mode;
         Buf_Out.buf[1] = 4;
    /*     if (Bus_Control_Flags & FHART_Trigger_Reconfig)
           {
             SPI_Command = 64; 
             Bus_Control_Flags &= ~FHART_Trigger_Reconfig;
           }
          else */
           SPI_Command = 0; 
        break;        
        
      case 69:                 //Burst Mode Control
         Buf_Out.buf[0] = 0x89;  // адрес получателя - HART
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3] = Burst_Control_Flags >> 4;
         Buf_Out.buf[4] = Burst_Control_Flags & 0x03;
         Buf_Out.buf[5] = Burst_Master_Address;
         Buf_Out.buf[1] = 4;
        //  SPI_Command = 0; 
         SPI_Command = 5; 
        break;
 /*    */ 
      case 70:   //  Device_Status
           Buf_Out.buf[0] = 0x81;  // адрес получателя - HART
           Buf_Out.buf[2] = SPI_Command;   // команда
           Buf_Out.buf[3] = Device_Status_Byte.Primary_Master | (Flash_Device_Status_Byte.Primary_Master & ~FLoop_Current_Not_Active);
           Buf_Out.buf[4] = Device_Status_Byte.Secondary_Master | (Flash_Device_Status_Byte.Secondary_Master & ~FLoop_Current_Not_Active);
           ptr = (char*)&Additional_Device_Status.Byte_Array[0];
           for (i=5;i<14;i++)     
            {  
              Buf_Out.buf[i] = *ptr++ ; 
            }
                                                                          //  Device Variable Status
          Buf_Out.buf[14] = Device_Variable[0].Device_Variable_Status; //  Percent Range
          Buf_Out.buf[15] = Device_Variable[1].Device_Variable_Status; //  Loop Current
          Buf_Out.buf[16] = Device_Variable[2].Device_Variable_Status; //  PV - давление
          Buf_Out.buf[17] = Device_Variable[3].Device_Variable_Status; //  SV - температура 
          Buf_Out.buf[1] = 16;
           SPI_Command = 0;
        break;       
        
        
        
             
      case 180:                 // Записать коэффициенты термокомпенсации ЦАПа 
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда
         count = 33;  
         ptr = (char*) &ReceiveBuffer[5];  //установить указатель
         for (i=0;i<count;i++)
          { 
            Buf_Out.buf[3+i]  = *ptr;  
            ptr++;
          }   
         
         Buf_Out.buf[1] =count+1;
          SPI_Command = 0;      
          break;   

      case 181:               //  Прочитать коэффициенты термокомпенсации ЦАПа
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3]  = ReceiveBuffer[5]; ;  

         Buf_Out.buf[1] = 2;
          SPI_Command = 0;          
          
           break;  
        
       case 184:                   // записать регистр управления ЦАПа    //ответа нет
         Buf_Out.buf[0] = 0x81;  // адрес исполнителя заменить на 0x81 - общий
         Buf_Out.buf[2] = SPI_Command;   // команда
         Buf_Out.buf[3] = Actual.Math_Mode; 
         Buf_Out.buf[4] = Mode_Setting_Reg; 
         Buf_Out.buf[1] =3;
          SPI_Command = 0;
      
        break;    

      case 186:                   // записать  константы режима модуля ЦАП  ***
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда
         count=4*2+2+3*2+2*2+2+2*3*2+2;  
         ptr = (char*) &ReceiveBuffer[5];  //установить указатель
         for (i=0;i<count;i++)
          { 
            Buf_Out.buf[3+i]  = *ptr;  
            ptr++;
          }   
         
         Buf_Out.buf[1] =count+1;
          SPI_Command = 0;
      
        break;     
      case 187:                   // // прочитать константы режима модуля ЦАП  ***
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда   
         Buf_Out.buf[1] =1;
          SPI_Command = 0;
      
        break;       
        
     case 188:                 // переключить режим выходного тока без запоминания  ***
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда
         if  (Actual.Math_Mode & FSw_5mA_Off)
           Buf_Out.buf[3] = 0xff;          // режим 4..20 ма
          else
            Buf_Out.buf[3] = 0;            // режим 0..5 ма
         Buf_Out.buf[1] =2;
          SPI_Command = 0;
      
        break;  
     case 189:                   // // Получить данные АЦП модуля ЦАП
         Buf_Out.buf[0] = 0x85;  //  адрес исполнителя 
         Buf_Out.buf[2] = SPI_Command;   // команда

         Buf_Out.buf[1] =1;
         SPI_Command = 0;
      
        break;  
        
    default: break; 
    
  }  
 
 }