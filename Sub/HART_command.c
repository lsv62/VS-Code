
//file "HART_command.c"

/*!
 * \brief HART_command - обработка  HART команд, полученных по SPI интерфейсу
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/



unsigned char SV_Unit ;
unsigned char Old_HART;
Old_HART = 0;
SV_Unit = 32;
while(Buf_Out.buf[1]);

   //      HART_Comm_Buf[Index_HART_Comm++]= 0;
   //      HART_Comm_Buf[Index_HART_Comm++]= 0;
    Buf_Out.buf[0] = 0x89; // ответ HART - модему
    Buf_Out.buf[2] = 0; // ответ на команду
    Buf_Out.buf[4] = 0;  // ошибок нет

  switch (Buf_In.buf[3])
   { case 0:                        //Read Unique Identifier   // количество байт в команде =0
         Buf_Out.buf[3] = 0;  // выполнена команда 0,
         ptr = (char*)&HART_Cmd_0_Data[0];
         for (i=6; i<sizeof(HART_Cmd_0_Data)+6; i++)
           Buf_Out.buf[i] = *ptr++;
       //  Buf_Out.buf[6+14]=
       //  Buf_Out.buf[6+15]=
         Buf_Out.buf[1] = 4+sizeof(HART_Cmd_0_Data);  //запуск передачи
       break;
      case 1:                      //  Read Primary Variable    // количество байт в команде =0
          Buf_Out.buf[3]  = 1;                                //  выполнена команда 1
          for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
           {
            if (P_Unit[j].Code == Actual.P_Unit)
            break;
           }
          Buf_Out.buf[6]  = P_Unit[j].Code;                      // единицы измерения
          Float_Buf_Tmp.f = P_Out*P_Unit[j].Kout_P[0];
          for (i=0;i<4;i++)                                   //  первичная переменная
           Buf_Out.buf[7+i] = Float_Buf_Tmp.c[3-i];
          Buf_Out.buf[1]  = 4+1+4;                            //запуск передачи
        break;
     case 2:              //Read Loop Current And Percent Of Range  // количество байт в команде =0
          Buf_Out.buf[3]  = 2;                               // выполнена команда 2
          Float_Buf_Tmp.f = ((float)DAC_Out-(float)Code_Down)/(Code_Up-Code_Down); //Приведенный ток
          PerCent.f = Float_Buf_Tmp.f*100;                   // Ток в процентах
          if (Actual.Math_Mode & FSw_5mA_Off)
              Float_Buf_Tmp.f = Float_Buf_Tmp.f*16.0 +4.0;   // режим 4..20 ма
           else
              Float_Buf_Tmp.f = Float_Buf_Tmp.f*5.0;         // режим 0..5 ма

          for (i=0;i<4;i++)                                  // ток в мА
            Buf_Out.buf[6+i] = Float_Buf_Tmp.c[3-i];
          for (i=0;i<4;i++)                                  // ток в %
            Buf_Out.buf[10+i] = PerCent.c[3-i];
          Buf_Out.buf[1] = 4 + 4 + 4;                        //запуск передачи

        break;
      case 3:             //Read Dynamic Variables And Loop Current  // количество байт в команде =0
          Buf_Out.buf[3] = 3;                                // выполнена команда 3
          Float_Buf_Tmp.f = ((float)DAC_Code.w - (float)Code_Down) / (Code_Up - Code_Down); //Приведенный ток
          if (Actual.Math_Mode & FSw_5mA_Off)
              Float_Buf_Tmp.f = Float_Buf_Tmp.f * 16.0 + 4.0;// режим 4..20 ма
            else
              Float_Buf_Tmp.f = Float_Buf_Tmp.f * 5.0;       // режим 0..5 ма
          for (i=0;i<4;i++)                                 // ток в мА
             Buf_Out.buf[6+i] = Float_Buf_Tmp.c[3-i];

          for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
           {
            if (P_Unit[j].Code == Actual.P_Unit)
            break;
           }
          Buf_Out.buf[10]  = P_Unit[j].Code;                      // единицы измерения первичной переменной
          Float_Buf_Tmp.f = P_Out*P_Unit[j].Kout_P[0];
          for (i=0;i<4;i++)                                   //  первичная переменная
           Buf_Out.buf[11+i] = Float_Buf_Tmp.c[3-i];

       //    Float_Buf_Tmp.f = temperature();
           Buf_Out.buf[15] = SV_Unit;   // единицы температуры
           ptr = ((char*)&Temperature) + 3;
           __bic_SR_register(GIE);   //Запретить прерывания
           for (i=0;i<4;i++)
            {
              Buf_Out.buf[16+i] = *ptr-- ;
            }
           __bis_SR_register(GIE);   // разрешить прерывания
           Buf_Out.buf[1] = 4 + 4 + 1 + 4 + 1 + 4;//   + 1 + 4 + 1 + 4;  //запуск передачи
        break;

   //   case 4:   // reserved
       //  break;
  //    case 5:   // reserved
       //  break;

      case 6: //    Write Polling Address   //  количество байт в команде = 1(старые версии) или 2
           Buf_Out.buf[3] = 6;                                // выполнена команда 6
           if (!(Buf_In.buf[1]-2))
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
               if (Buf_In.buf[1]-2 == 1)
               {
                 if (Buf_In.buf[4])
                    Buf_In.buf[5] = 0;
                  else
                    Buf_In.buf[5] = 1;
               }
               if (Buf_In.buf[4]>63)
                {
                  Buf_Out.buf[4] = 2;  //  Invalid Poll Address Selection
                }
               else
                {
                 if (Buf_In.buf[5]>1)
                  {
                    Buf_Out.buf[4] = 12;  //  Invalid Mode Selection
                  }
                 else
                  {
                    if ( write_data2flash( (unsigned long int)((char *) &Addr_H_Long[0]), (char *) &Buf_In.buf[4], 1) )
                     {
                       Error_Code = Write2Flash_Error;
                       Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                     }
                    Buf_In.buf[8] = Flash_Device_Status_Byte.Primary_Master;   //Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed
                    Buf_In.buf[9] = Flash_Device_Status_Byte.Secondary_Master;
                    if (Buf_In.buf[5])
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
                  }
                }

             }
            }
           if (Buf_Out.buf[4])
            {
              Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
            }
            else
              {
               Cnfg_Counter_Increment();
               Buf_Out.buf[6] = Addr_H_Long[0];
               if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed)
                    Buf_Out.buf[7] = 0;
                  else
                   Buf_Out.buf[7] = 1;
               Buf_Out.buf[1] = 4 + 2;  //запуск передачи
               SPI_Command = 60;
              }
         break;
      case 7: //    Read Loop Configuration   // количество байт в команде =0
            Buf_Out.buf[3] = 7;                                // выполнена команда 7
            Buf_Out.buf[6] = Addr_H_Long[0];
            if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed)    // токовый выход блокирован?
              Buf_Out.buf[7] = 0;
             else
               Buf_Out.buf[7] = 1;
            Buf_Out.buf[1] = 4 + 2;  //запуск передачи
         break;
      case 8: //    Read Dynamic Variable Classifications   // количество байт в команде =0
            Buf_Out.buf[3] = 8;                                // выполнена команда 8
            Buf_Out.buf[6] = 65;
            Buf_Out.buf[7] = 64;
            Buf_Out.buf[8] = 250;    // добавлено по требованию теста R6  - надо ли в 7?
            Buf_Out.buf[9] = 250;    // добавлено по требованию теста R6  - надо ли в 7?

            Buf_Out.buf[1] = 4 + 4;  //запуск передачи
         break;
      case 9: //    Read Device Variables with Status      // количество байт в команде от 1 до 8
           Buf_Out.buf[3] = 9;                                // выполнена команда 9
           if (Buf_In.buf[1]==2)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
              ptr =(char*) &Buf_Out.buf[7];
              Buf_Out.buf[6] = 0;  // Extended Field Device Status (Common Table 17)
              for (j=0;j<(Buf_In.buf[1]-2);j++)  // zdes' tolko zagotovka, otvet formiruetsja v HART module (tolko dlja command 9)
               {
		 *ptr = Buf_In.buf[4+j];
		    if ((*ptr++ > 250))
		    {
		      Buf_Out.buf[4] = 2;     // Invalid Selection
		      break;
		    }
               }
	   if ((Buf_Out.buf[4]==2)||(Buf_Out.buf[4]==5))
             Buf_Out.buf[1] = 4;  //запуск передачи
            else
	     {
               while(j++ < 8)
                 *ptr++ = 250;    // no Device Variable Code
     // percent range
                         *ptr++ = 0;       //  Device Variable Classification (Device Variable Not Classified)
                         *ptr++ = 57;     //  %
                         Float_Buf_Tmp.f = (P_Out - Actual.Pmin) * K_PerCent;
                         for (i=0;i<4;i++)              //  percent range
                            *ptr++ = Float_Buf_Tmp.c[3-i];
                         *ptr++ = 0xc0;    // Device Variable Status ( Status = "good" and Limit = "not limited")
    // Loop Current
                         *ptr++ = 84;       //  Device Variable Classification (Current)
                         *ptr++ = 39;      //  mA
                         Float_Buf_Tmp.f = ((float)DAC_Out-(float)Code_Down) * K_Loop_Current + Base_Loop_Current;
                         for (i=0;i<4;i++)              //  Loop Current
                            *ptr++ = Float_Buf_Tmp.c[3-i];
                         *ptr++ = 0xc0;    // Device Variable Status ( Status = "good" and Limit = "not limited")

    //  Primary Variable
                          *ptr++ = 65;       //  Device Variable Classification (давление)
                          for (i=0; i< (sizeof(P_Unit)/sizeof(P_unit_record));i++ )
                           {
                             if (P_Unit[i].Code == Actual.P_Unit)
                                break;
                           }
                         *ptr++ = P_Unit[i].Code;     //  Units Code
                         Float_Buf_Tmp.f =  P_Out*P_Unit[i].Kout_P[0];
                         for (i=0;i<4;i++)              //  первичная переменная
                            *ptr++ = Float_Buf_Tmp.c[3-i];
                         *ptr++ = 0xc0;    // Device Variable Status ( Status = "good" and Limit = "not limited")
    //  Secondary Variable
                         *ptr++ = 64;       //  Device Variable Classification (температура)
                         *ptr++ = SV_Unit;     //  Units Code
                         for (ptrURX_Tx=((BYTE*)&Temperature)+3;ptrURX_Tx >=((BYTE*)&Temperature);ptrURX_Tx--)
                          {
                           *ptr++ = *ptrURX_Tx ;
                          }
                         *ptr++ = 0xc0;    // Device Variable Status ( Status = "good" and Limit = "not limited")  spec099  8.4 Device Variable Status

               Buf_Out.buf[1] = 4 + 1  + 8 + 4*7   ;  //запуск передачи
             }
         break;
      case 11:   //   Read Unique Identifier Associated With Tag   // количество байт в команде 6
           if (Buf_In.buf[1]<2+6)
           {
             Buf_Out.buf[1] = 2;  //запуск передачи (2 - не передаются данные - таг не совпал)
           }
            else
            {
             for (i=0; i<6; i++)
              if (Buf_In.buf[4+i] != HART_Cmd_13_Data[i])
                break;
             if (i>5)
              {
               Buf_Out.buf[3] = 11;                              // выполнена команда 11
               ptr = (char*)&HART_Cmd_0_Data[0];
               for (i=6; i<sizeof(HART_Cmd_0_Data)+6; i++)
                 Buf_Out.buf[i] = *ptr++;
           //    Buf_Out.buf[1] = 4 + sizeof(HART_Cmd_0_Data);  //запуск передачи

         //      for (i=0; i<sizeof(HART_Cmd_0_Data); i++)
          //       Buf_Out.buf[6+i] = HART_Cmd_0_Data[i];
               Buf_Out.buf[1] = 4+sizeof(HART_Cmd_0_Data);  //запуск передачи
              }
              else
               Buf_Out.buf[1] = 2;  //запуск передачи (2 - не передаются данные - таг не совпал)
            }
         break;
      case 12:    //  Read Message  // количество байт в команде 0
            Buf_Out.buf[3]= 12;  // выполнена команда 12
            ptr = (char*)&HART_Cmd_12_Data[0];
            for (i=6; i<sizeof(HART_Cmd_12_Data)+6; i++)
               Buf_Out.buf[i] = *ptr++;
        //    Buf_Out.buf[1] = 4 + sizeof(HART_Cmd_12_Data);  //запуск передачи

     //       for (i=0; i<sizeof(HART_Cmd_12_Data); i++)
       //        Buf_Out.buf[6+i] = HART_Cmd_12_Data[i];
            Buf_Out.buf[1]= 4 + sizeof(HART_Cmd_12_Data);  //запуск передачи
          break;

      case 13: //  Read Tag, Descriptor, Date    // количество байт в команде 0
            Buf_Out.buf[3]= 13;  // выполнена команда 13
            ptr = (char*)&HART_Cmd_13_Data[0];
            for (i=6; i<sizeof(HART_Cmd_13_Data)+6; i++)
               Buf_Out.buf[i] = *ptr++;
            Buf_Out.buf[1] = 4 + sizeof(HART_Cmd_13_Data);  //запуск передачи
           break;
      case 14:    // Read Primary Variable Transducer Information     // количество байт в команде 0
            Buf_Out.buf[3] = 14;  // выполнена команда 14
            i=6;
            for (j=9; j < 12;j++,i++ )
               Buf_Out.buf[i] = HART_Cmd_0_Data[j];

            Buf_Out.buf[i++] = Actual.P_Unit;         //  PV Upper and Lower Range Values Units Code    // 4

            for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
             {
               if (P_Unit[j].Code == Actual.P_Unit)
               break;
             }

            Float_Buf_Tmp.f = Pass.Pmax_Lim*P_Unit[j].Kout_P[0];      // Upper Transducer Limit
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 5-8


            Float_Buf_Tmp.f = Pass.Pmin_Lim*P_Unit[j].Kout_P[0];      //    Lower Transducer Limit
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                     // 9-12

            Float_Buf_Tmp.f = (Pass.Pmax_Lim - Pass.Pmin_Lim)*P_Unit[j].Kout_P[0]/Pass.K_Rearranging;  //  Minimum Span  исправлено 2.03.2016
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 13-16

            Buf_Out.buf[1] = i-2;  //запуск передачи
        break;
      case 15:     //Read Device Information    // количество байт в команде 0
            Buf_Out.buf[3] = 15;  // выполнена команда 15
            i=6;
            if (Actual.Math_Mode & FCurrent_Alarm_Enable)      // PV Alarm Selection Code
               {
                if (Actual.Math_Mode & FCurrent_Alarm_High)
                  Buf_Out.buf[i++] = 0;
                 else
                  Buf_Out.buf[i++] = 1;
               }
              else
                Buf_Out.buf[i++] = 251;                                                                 // 1

            if (Actual.Math_Mode & FTransfer_SQRT)     //  PV Transfer Function Code
                 Buf_Out.buf[i++] = 1;
               else
                 Buf_Out.buf[i++] = 0;                                                                  // 2

            Buf_Out.buf[i++] = Actual.P_Unit;         //  PV Upper and Lower Range Values Units Code    // 3

            for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
             {
               if (P_Unit[j].Code == Actual.P_Unit)
               break;
             }
            if (Actual.Math_Mode & FTransfer_Lin_Drop)     // PV Upper Range Value
               Float_Buf_Tmp.f = Actual.Pmin*P_Unit[j].Kout_P[0];
              else
               Float_Buf_Tmp.f = Actual.Pmax*P_Unit[j].Kout_P[0];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 4-7

            if (Actual.Math_Mode & FTransfer_Lin_Drop)     //  PV Lower Range Value
               Float_Buf_Tmp.f = Actual.Pmax*P_Unit[j].Kout_P[0];
              else
               Float_Buf_Tmp.f = Actual.Pmin*P_Unit[j].Kout_P[0];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                     // 8-11

            Float_Buf_Tmp.f = ((float)Actual.Filter_Limit)*(1.0/15.87);  // PV Damping Value
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
            Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 12-15

            Buf_Out.buf[i++] = HART_write_protect_value; //251;      //  Write Protect Code (нет режима запрета записи)      // 16
            Buf_Out.buf[i++] = 250;      //  Reserved  (250)                                     // 17
            Buf_Out.buf[i++] = 0;  // изменено в ревизии 7 : = 1;  //  PV Analog Channel Flags   // 18
            Buf_Out.buf[1] = i-2;  //запуск передачи
        break;

      case 16: //   Read Final Assembly Number    // количество байт в команде =0
            Buf_Out.buf[3] = 16;                                // выполнена команда 16
            Buf_Out.buf[6] = HART_Cmd_16_Data[0];
            Buf_Out.buf[7] = HART_Cmd_16_Data[1];
            Buf_Out.buf[8] = HART_Cmd_16_Data[2];
            Buf_Out.buf[1] = 4 + 3;  //запуск передачи
         break;
      case 17: //    Write Message       // количество байт в команде = 24
           Buf_Out.buf[3] = 17;                                // выполнена команда 17
           if (Buf_In.buf[1]< 2 + sizeof(HART_Cmd_12_Data) )
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
          //    Buf_Out.buf[1] = 4;  //запуск передачи
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
              if ( write_data2flash( (unsigned long int)((char *) &HART_Cmd_12_Data[0]), (char *) &Buf_In.buf[4], 24) )
              {
                Error_Code = Write2Flash_Error;
                Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
              }
             }
            }

           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
               Cnfg_Counter_Increment();

            ptr = (char*)&HART_Cmd_12_Data[0];
            for (i=6; i<sizeof(HART_Cmd_12_Data)+6; i++)
               Buf_Out.buf[i] = *ptr++;

      //         for (i=0; i<sizeof(HART_Cmd_12_Data); i++)
    //             Buf_Out.buf[6+i] = HART_Cmd_12_Data[i];
               Buf_Out.buf[1]= 4 + sizeof(HART_Cmd_12_Data);  //запуск передачи

             }
         break;
      case 18: //    Write Tag, Descriptor, Date   // количество байт в команде 21
           Buf_Out.buf[3] = 18;                                // выполнена команда 18
           if (Buf_In.buf[1] < 2 + sizeof(HART_Cmd_13_Data))
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
              if ( write_data2flash( (unsigned long int)((char *) &HART_Cmd_13_Data[0]), (char *) &Buf_In.buf[4], 21) )
               {
                 Error_Code = Write2Flash_Error;
                 Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
               }
             }
            }
           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
              Cnfg_Counter_Increment();

            ptr = (char*)&HART_Cmd_13_Data[0];
            for (i=6; i<sizeof(HART_Cmd_13_Data)+6; i++)
               Buf_Out.buf[i] = *ptr++;

     //         for (i=0; i<sizeof(HART_Cmd_13_Data); i++)
      //          Buf_Out.buf[6+i] = HART_Cmd_13_Data[i];
              Buf_Out.buf[1]= 4 + sizeof(HART_Cmd_13_Data);  //запуск передачи

             }
         break;
      case 19: //    Write Final Assembly Number    // количество байт в команде 3
           Buf_Out.buf[3] = 19;                                // выполнена команда 19
           if (Buf_In.buf[1]<2+3)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
               if ( write_data2flash( (unsigned long int)((char *) &HART_Cmd_16_Data[0]), (char *) &Buf_In.buf[4], 3) )
               {
                 Error_Code = Write2Flash_Error;
                 Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
               }
             }
            }
           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
              Cnfg_Counter_Increment();
              for (i=0; i<3; i++)
                Buf_Out.buf[6+i] = HART_Cmd_16_Data[i];
              Buf_Out.buf[1]= 4 + 3;  //запуск передачи

             }
         break;
      case 20: //    Read Long Tag   // количество байт в команде 0
              Buf_Out.buf[3] = 20;                                // выполнена команда 20
              for (i=0; i<sizeof(HART_Cmd_20_Data); i++)
                Buf_Out.buf[6+i] = HART_Cmd_20_Data[i];
              Buf_Out.buf[1]= 4 + sizeof(HART_Cmd_20_Data);  //запуск передачи
         break;
      case 21: //    Read Unique Identifier Associated With Long Tag  // количество байт в команде = 32
            if (Buf_In.buf[1]<2+32)
             {
               Buf_Out.buf[1] = 2;  //запуск передачи (2 - не передаются данные - таг не совпал)
             }
             else
             {
              for (i=0; i<sizeof(HART_Cmd_20_Data); i++)
               if (Buf_In.buf[4+i] != HART_Cmd_20_Data[i])
                 break;
              if (i>31)
               {
                Buf_Out.buf[3] = 21;                              // выполнена команда 21
                for (i=0; i<23; i++)
                 Buf_Out.buf[6+i] = HART_Cmd_0_Data[i];
                Buf_Out.buf[1] = 4+sizeof(HART_Cmd_0_Data);  //запуск передачи
               }
              else
               Buf_Out.buf[1] = 2;  //запуск передачи  (2 - не передаются данные - таг не совпал)
             }
         break;
      case 22: //    Write Long Tag    // количество байт в команде 32
           Buf_Out.buf[3] = 22;                                // выполнена команда 22
           if (Buf_In.buf[1]< 2 + sizeof(HART_Cmd_20_Data) )
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
             {
              if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
              else
               {
               if ( write_data2flash( (unsigned long int)((char *) &HART_Cmd_20_Data[0]), (char *) &Buf_In.buf[4], 32) )
                {
                  Error_Code = Write2Flash_Error;
                  Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                }
               }
             }
           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
               Cnfg_Counter_Increment();
               for (i=0; i<sizeof(HART_Cmd_20_Data); i++)
                 Buf_Out.buf[6+i] = HART_Cmd_20_Data[i];
               Buf_Out.buf[1]= 4 + sizeof(HART_Cmd_20_Data);  //запуск передачи
             }
         break;

      case 34:  // Задать константу демпфирования (в секундах, float)  // количество байт в команде 4
        //коды ответа:
        //  3 - параметр слишком велик
        //  4 - параметр слишком мал
        //  5 - получено слишком мало байтов
        //  6 - ошибочная команда или ошибка специфики выполнения этой команды?
        //  7 - включена блокировка записи по харту (видимо)
        //  8 - установлено в ближайшее допустимое значение
        // 16 - доступ запрещен
        // 32 - занято
            Buf_Out.buf[3]= 34;    //  выполнена команда 34
            if (Buf_In.buf[1]<2+4)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
             {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
              {
               for (i=0;i<4;i++)
                Float_Buf_Tmp.c[3-i] =  Buf_In.buf[4+i];
               if (Float_Buf_Tmp.f < 0.0)
                 {
                   Buf_Out.buf[4] = 4; //  HART_Error параметр слишком мал
                 }
                else
                 {
                  PerCent.f = Float_Buf_Tmp.f;
                  Int_Buf_Tmp.w = (int)(Float_Buf_Tmp.f*15.87+.5);
                  if   (Int_Buf_Tmp.w > Filter_Limit_Max)
                    {
                      Buf_Out.buf[4] = 3; //  HART_Error параметр слишком велик
                    }
                   else
                    {
                     if   (Int_Buf_Tmp.w < 1)
                      {
                       Actual.Filter_Limit=0;
                       dampfer_zero();    //  Filter_Limit = 0;
                      }
                      else
                      {
                        Actual.Filter_Limit = Int_Buf_Tmp.w;
                      }
                      Actual_2_User();
                      if (Error_Code == Write2Flash_Error)
                       Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                    }
                  }
               }
              }
            if (Buf_Out.buf[4])
              {
                Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
              }
             else
              {
	       Float_Buf_Tmp.f = Actual.Filter_Limit*(1.0/15.87);
               if (PerCent.f != Float_Buf_Tmp.f) // если уставка не равна 0 - вычисляем
                 Buf_Out.buf[4] = 8; //установлено в ближайшее допустимое значение

               for (i=0;i<4;i++)
                 Buf_Out.buf[6+i] = Float_Buf_Tmp.c[3-i] ;

                Buf_Out.buf[1] = 4 + 4; //запуск передачи
                SPI_Command = 5; // команда - передать статус датчика
              }
        break;
      case 35:  // задать верхний и нижний пределы первичной переменной, соответствующие 4 и 20 ма   // количество байт в команде 9
        // первый байт - единицы, не влияют на единицы прибора, должны быть пересчитаны при установке значений
        // ответ - в тех же единицах; след параметр - float - верхний предел, следующий - нижний
        // коды ответа:
        // 2 - неправильный выбор.. ??????
        // 5 - получено слишком мало байтов
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        // 8 - установлено в ближайшее допустимое значение
        // 9 - нижний предел слишком велик
        //10 - нижний предел слишком мал
        //11 - верхний предел слишком велик
        //12 - верхний предел слишком мал
        //13 - верхний и нижний пределы вне допуска
        //14 - диапазон слишком мал - возможно ухудшение точностных характеристик прибора
        //16 - доступ запрещен
        //18 - неправильный код единиц измерений
        //29 - неправильный диапазон   ??????
        //32 - занят
            Buf_Out.buf[3]= 35;  // выполнена команда 35
            if (Buf_In.buf[1]<2+9)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             if (239 < Buf_In.buf[4])
              {
                Buf_Out.buf[4] = 2;     // неправильный выбор
              }
             else
 //******************** выбор единиц ***************************
            {
             for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
              {
               if (P_Unit[j].Code == Buf_In.buf[4])
                {
                  break;
                }
              }
             if (j>= sizeof(P_Unit)/sizeof(P_unit_record))
               {
                 Buf_Out.buf[4] = 18;   //неправильный код единиц измерений
                 for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
                  {
                    if (P_Unit[j].Code == Actual.P_Unit)
                    break;
                  }
               }
              else
              {
 //******************** верхний предел ***************************
               for (i=0;i<4;i++)
                 Float_Buf_Tmp.c[3-i] = Buf_In.buf[5+i];
              // Float_Buf_Tmp.f *= P_Unit[j].Kout_P[1];
               if (Pass.Pmin_Lim*P_Unit[j].Kout_P[0] > Float_Buf_Tmp.f)
                 Buf_Out.buf[4] = 12 ;     //  верхний предел слишком мал
                else
                 if (Pass.Pmax_Lim*P_Unit[j].Kout_P[0] < Float_Buf_Tmp.f)
                   Buf_Out.buf[4] = 11 ;     //  верхний предел слишком велик
                  else
                   {
                     Pup_Tmp = Float_Buf_Tmp.f*P_Unit[j].Kout_P[1];      // запомнить значение Pup
 //******************** нижний предел ***************************
                     for (i=0;i<4;i++)
                       Float_Buf_Tmp.c[3-i] = Buf_In.buf[9+i];
               //      Float_Buf_Tmp.f *= P_Unit[j].Kout_P[1];
                     if (Pass.Pmin_Lim*P_Unit[j].Kout_P[0] > Float_Buf_Tmp.f)
                       Buf_Out.buf[4] = 10 ;     //  нижний предел слишком мал
                      else
                       if (Pass.Pmax_Lim*P_Unit[j].Kout_P[0] < Float_Buf_Tmp.f)
                         Buf_Out.buf[4] = 9 ;     //  нижний предел слишком велик
                        else
                         {
                           Pup = Pup_Tmp;
                           Pdown = Float_Buf_Tmp.f*P_Unit[j].Kout_P[1];    // Pdown задать
 //********************   span  ***************************
                           if (Pup > Pdown)
                             Float_Buf_Tmp.f = Pup - Pdown;
                            else
                              Float_Buf_Tmp.f = Pdown - Pup;
                           if (Float_Buf_Tmp.f)        //  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            {
                              if (Float_Buf_Tmp.f * Pass.K_Rearranging < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                                Buf_Out.buf[4] = 14 ;   //  диапазон слишком мал - возможно ухудшение точностных характеристик прибора
                              Set_Pup_Pdown();
                              Calculate_Span();
                              Actual_2_User();
                              if (Error_Code == Write2Flash_Error)
                                Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                            }
                           else
                             Buf_Out.buf[4] = 29; // неправильный диапазон
                        }
                   }
              }
            }

           if ((Buf_Out.buf[4])&&(Buf_Out.buf[4]!=14))
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
               Buf_Out.buf[6]  = P_Unit[j].Code;                      // единицы измерения
               Float_Buf_Tmp.f = Pup*P_Unit[j].Kout_P[0];
               for (i=0;i<4;i++)                                   //  верхний предел
                 Buf_Out.buf[7+i] = Float_Buf_Tmp.c[3-i];
               Float_Buf_Tmp.f = Pdown*P_Unit[j].Kout_P[0];
               for (i=0;i<4;i++)                                   //  нижний предел
                 Buf_Out.buf[11+i] = Float_Buf_Tmp.c[3-i];
               Buf_Out.buf[1] = 4 + 1 + 4 + 4 ;              //запуск передачи
               SPI_Command = 5; // команда - передать статус датчика
             }
        break;

      case 36:  // установить верхний  предел первичной переменной равным текущему значению PV  // количество байт в команде 0
         //коды ответа:
        // 6 - Device-specific Command Error
        // 7 - In Write Protect Mode
        // 8 - Set To Nearest Possible Value (Upper Range Value Pushed) - Warning
        // 9 - Applied Process Too High
        //10 - Applied Process Too Low
        //14 - Span Too Small (Device Accuracy May Be Impaired)
        //16 - Access Restricted
        //29 - Invalid Span
        //32 - Busy
          Buf_Out.buf[3]= 36;  // выполнена команда 36
          if (HART_write_protect_value)
            Buf_Out.buf[4] = 7;     // In Write Protect Mode
          else
          {
          if (Pass.Pmin_Lim - (Pass.Pmax_Lim-Pass.Pmin_Lim) * 0.01 > P_Out)
             Buf_Out.buf[4] = 10 ;     //  заданние слишком мало
           else
            if (Pass.Pmax_Lim +(Pass.Pmax_Lim-Pass.Pmin_Lim) * 0.01 < P_Out)
             Buf_Out.buf[4] = 9 ;     //  заданние слишком велико
            else
            {
             if (P_Out > Pdown)
                Float_Buf_Tmp.f = P_Out - Pdown;
               else
                Float_Buf_Tmp.f = Pdown - P_Out;
             if (Float_Buf_Tmp.f * 100.0 < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                 Buf_Out.buf[4] = 29 ;   //Invalid Span
              else
               {
                if (Float_Buf_Tmp.f * Pass.K_Rearranging < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                  Buf_Out.buf[4] = 14 ;   //  диапазон слишком мал - возможно ухудшение точностных характеристик прибора
                Pup = P_Out;
                Set_Pup_Pdown();
                Calculate_Span();
                Actual_2_User();
                if (Error_Code == Write2Flash_Error)
                  Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                SPI_Command = 5; // команда - передать статус датчика
               }
            }
           }
          Buf_Out.buf[1] = 4;              //запуск передачи

          break;

      case 37:  // установить нижний  предел первичной переменной равным текущему значению PV    // количество байт в команде 0
        //коды ответа:
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        // 9 - заданние слишком велико
        //10 - заданние слишком мало
        //14 - установлено в ближайшее допустимое значение
        //16 - доступ запрещен
        //29 - неправильный диапазон   ??????
        //32 - занят
          Buf_Out.buf[3] = 37;  // выполнена команда 37
          if (HART_write_protect_value)
            Buf_Out.buf[4] = 7;     // In Write Protect Mode
          else
         {
          if (Pass.Pmin_Lim /*- (Pass.Pmax_Lim-Pass.Pmin_Lim) * 0.01*/ > P_Out)
             Buf_Out.buf[4] = 10 ;     //  заданние слишком мало
           else
            if (Pass.Pmax_Lim /*+(Pass.Pmax_Lim-Pass.Pmin_Lim) * 0.01 */< P_Out)
             Buf_Out.buf[4] = 9 ;     //  заданние слишком велико
            else
            {
              if (P_Out > Pup)
               { // обратная характеристика
                 Float_Buf_Tmp.f = P_Out - (Actual.Pmax-Actual.Pmin);
                 if (Float_Buf_Tmp.f < Pass.Pmin_Lim)
                  {
                    Float_Buf_Tmp.f = Pass.Pmin_Lim;
                    if ((P_Out - Float_Buf_Tmp.f)*100.0 < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                       Buf_Out.buf[4] = 10; //  заданние слишком мало
                     else
                     {
                       Pdown = P_Out;
                       Pup = Pass.Pmin_Lim;
                       Buf_Out.buf[4] = 14 ;     // нижний предел "вдавлен"
                     }
                  }
                 else
                   {                          // все нормально уложилось
                     Pdown = P_Out;
                     Pup = Float_Buf_Tmp.f;
                   }
               }
              else
                {// прямая характеристика
                  Float_Buf_Tmp.f = P_Out + (Actual.Pmax-Actual.Pmin);
                  if (Float_Buf_Tmp.f > Pass.Pmax_Lim)
                   {
                    Float_Buf_Tmp.f = Pass.Pmax_Lim;
                    if ((Float_Buf_Tmp.f - P_Out)*100.0 < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                      Buf_Out.buf[4] = 9;  //  заданние слишком велико
                     else
                      {
                        Pdown = P_Out;
                        Pup = Pass.Pmax_Lim;
                        Buf_Out.buf[4] = 14 ;     // нижний предел "вдавлен"
                      }
                   }
                  else
                   {                              // все нормально уложилось
                    Pdown = P_Out;
                    Pup = Float_Buf_Tmp.f;
                   }
                }

/*
              if (P_Out > Pup)
               { // обратная характеристика
                 Float_Buf_Tmp.f = P_Out - (Actual.Pmax-Actual.Pmin);
                 if (Float_Buf_Tmp.f < Pass.Pmin_Lim)
                  {
                    Float_Buf_Tmp.f = Pass.Pmin_Lim;
                    Buf_Out.buf[4] = 14 ;     // нижний предел "вдавлен"
                  }
                 if ((P_Out - Float_Buf_Tmp.f)*100.0 < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                    Buf_Out.buf[4] = 29; // неправильный диапазон
                  else
                   {
                     Pdown = P_Out;
                     Pup = Float_Buf_Tmp.f;
                   }
               }
              else
                {// прямая характеристика
                  Float_Buf_Tmp.f = P_Out + (Actual.Pmax-Actual.Pmin);
                  if (Float_Buf_Tmp.f > Pass.Pmax_Lim)
                   {
                     Float_Buf_Tmp.f = Pass.Pmax_Lim;
                     Buf_Out.buf[4] = 14 ;     // нижний предел "вдавлен"
                   }
                  if ((Float_Buf_Tmp.f - P_Out)*100.0 < (Pass.Pmax_Lim - Pass.Pmin_Lim))
                      Buf_Out.buf[4] = 29; // неправильный диапазон
                   else
                    {
                     Pdown = P_Out;
                     Pup = Float_Buf_Tmp.f;
                    }
                }

              // test
             if ((Pdown < 0.3) && (Pdown > 0.0))
               Pup = 250.0;
*/              // ******************************
             if  (Buf_Out.buf[4] != 29)
              {
                Set_Pup_Pdown();
                Calculate_Span();
                Actual_2_User();
                if (Error_Code == Write2Flash_Error)
                  Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                SPI_Command = 5; // команда - передать статус датчика
              }
            }
         }
           Buf_Out.buf[1] = 4;              //запуск передачи

          break;

     case 38:  // сбросить флаг "конфигурация изменена"  // количество байт в команде 2
        //коды ответа:
        // 5 - получено слишком мало байтов
        //  6 - специфическая для прибора ошибка выполнения этой команды
        //  7 - включена блокировка записи по харту (видимо)
        //  9 - счетчик изменения конфигурации не совпал
        // 16 - доступ запрещен
             Buf_Out.buf[3]= 38;    //  выполнена команда 38
        /* */  if (Buf_In.buf[1]==3)
               Buf_Out.buf[4] = 5;  // 1 байт получен - malo dla 7 versii
            else
       /*    if (HART_write_protect_value)
              Buf_Out.buf[4] = 7;     // In Write Protect Mode
            else      */
            {
             if (Buf_In.buf[1]==2)
               {
                 Buf_In.buf[4] =  HART_Cmd_0_Data[14];;
                 Buf_In.buf[5] =  HART_Cmd_0_Data[15];
               }
             if   ((Buf_In.buf[4] != HART_Cmd_0_Data[14]) || (Buf_In.buf[5] != HART_Cmd_0_Data[15]))
               {
                 Buf_Out.buf[4] = 9; //  HART_Error счетчик изменения конфигурации не совпал
               }
              else
               {
                 Int_Buf_Tmp.c[0] = Flash_Device_Status_Byte.Primary_Master;
                 Int_Buf_Tmp.c[1] = Flash_Device_Status_Byte.Secondary_Master;
                 if (Buf_In.buf[2] == 129)
                   {
                     Int_Buf_Tmp.c[0] &= ~FConfiguration_Changed;
                   }
                  else
                   {
                     Int_Buf_Tmp.c[1] &= ~FConfiguration_Changed;
                   }
                 if ( write_data2flash( (unsigned long int)((char *) &Flash_Device_Status_Byte.Primary_Master), (char *) &Int_Buf_Tmp.c[0], 2) )
                  {
                    Error_Code = Write2Flash_Error;
                    Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                  }
                }
             }
            if (Buf_Out.buf[4])
              {
                Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
              }
             else
              {
#ifdef  HART7
                Buf_Out.buf[6] = HART_Cmd_0_Data[14] ; // закомментировано для прохождения теста - отладка
                Buf_Out.buf[7] = HART_Cmd_0_Data[15] ;
                Buf_Out.buf[1] = 4 + 2 ; //запуск передачи
#else
                Buf_Out.buf[1] = 4 ; //запуск передачи
#endif
              }

        break;


      case 40:  // установить ток в петле   // количество байт в команде 4
        //коды ответа:
        // 3 - параметр слишком велик       ----------------
        // 4 - параметр слишком мал         -------------
        // 5 - получено слишком мало байтов    +
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        //11 - режим токовой петли отключен
        //16 - доступ запрещен
        //32 - занят

          Buf_Out.buf[3]= 40;  // выполнена команда 40
          if (Buf_In.buf[1]<2+4)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
            {
             if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Not_Active)
              {
               Buf_Out.buf[4] = 11;     // режим токовой петли отключен = Loop Current Not Active (Device in Multidrop Mode)
              }
              else
               {
                for (i=0;i<4;i++)
                  Float_Buf_Tmp.c[3-i] = Buf_In.buf[4+i];
                if (Float_Buf_Tmp.f == 0.0)
                  {    // выйти из режима фиксированного тока
                    Buf_In.buf[8] = Flash_Device_Status_Byte.Primary_Master;
                    Buf_In.buf[9] = Flash_Device_Status_Byte.Secondary_Master;
                    Buf_In.buf[8] &= ~FLoop_Current_Fixed; // разблокировать токовый выход
                    Buf_In.buf[9] &= ~FLoop_Current_Fixed;
                    if ( write_data2flash( (unsigned long int)((char *) &Flash_Device_Status_Byte.Primary_Master), (char *) &Buf_In.buf[8], 2) )
                     {
                       Error_Code = Write2Flash_Error;
                       Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                     }
                  }
                 else
                  {
                   if (Float_Buf_Tmp.f > 20.0)
                      Buf_Out.buf[4] = 3;     //  параметр слишком велик
                    else
                     if((Float_Buf_Tmp.f < 4.0))
                        Buf_Out.buf[4] = 4;     //  параметр слишком мал
                      else
                       {
                       //  DAC_Count = 255;
                         Int_Buf_Tmp.w = (int)((Float_Buf_Tmp.f - 4.0) * (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) / 16.0 + 0.5) + Actual.DAC_Code_4_mA;

                         Buf_In.buf[8] = Flash_Device_Status_Byte.Primary_Master;   //Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed
                         Buf_In.buf[9] = Flash_Device_Status_Byte.Secondary_Master;
                         Buf_In.buf[8] |= FLoop_Current_Fixed; // заблокировать токовый выход
                         Buf_In.buf[9] |= FLoop_Current_Fixed; // заблокировать токовый выход
                         Buf_In.buf[10] = Int_Buf_Tmp.c[0];
                         Buf_In.buf[11] = Int_Buf_Tmp.c[1];
                         if ( write_data2flash( (unsigned long int)((char *) &Flash_Device_Status_Byte.Primary_Master), (char *) &Buf_In.buf[8], 4) )
                          {
                            Error_Code = Write2Flash_Error;
                            Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                          }
                       }
                  }
               }
             }
            }
           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
              Float_Buf_Tmp.f = ((float)Flash_Device_Status_Byte.DAC_Code_Fixed - (float)Actual.DAC_Code_4_mA) * 16.0 / (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) + 4.0;
              for (i=0;i<4;i++)
                 Buf_Out.buf[6+i] =  Float_Buf_Tmp.c[3-i] ;
              Buf_Out.buf[1] = 4 + 4 ;              //запуск передачи
             }

        break;

      case 41: //self test
         //коды ответа:
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        //16 - доступ запрещен
        //32 - занят
            Buf_Out.buf[3] = 41;              // выполнена команда 41
            Buf_Out.buf[1] = 4 ;              //запуск передачи
          break;

      case 42: // Сброс устройства
         //коды ответа:
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        //16 - доступ запрещен
        //32 - занят
            Buf_Out.buf[3] = 42;              // выполнена команда 42
            Buf_Out.buf[1] = 4 ;              //запуск передачи
            SPI_Command=1;
         break;

      case 43:  // установить  0 PV по давлению на входе   // количество байт в команде 0   (а как же проверка на нулевую последовательность данных?)
         //коды ответа:
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        // 9 - заданние слишком велико
        //10 - заданние слишком мало
        //16 - доступ запрещен
        //32 - занят
            Buf_Out.buf[3] = 43;              // выполнена команда 43
            if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
            else
           {
            zero_shift_set();          // установить ноль
            if  (Error_Code & Set_Zero_Error)
             {
               if (Actual.P_Set_Zero_Shift > P_Out)
                  Buf_Out.buf[4] = 10 ;     //  заданние слишком мало
                else
                  Buf_Out.buf[4] = 9 ;     //  заданние слишком велико
             }
             else
              {
            //   Actual_2_User();                         Это делается в zero_shift_set()    4062016
               if (Error_Code == Write2Flash_Error)
                 Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
            //   SPI_Command = 5; // команда - передать статус датчика   Это делается в zero_shift_set()  4062016
              }
            }
            Buf_Out.buf[1] = 4 ;              //запуск передачи

         break;
      case 44:  // установить  единицы измерения  PV   // количество байт в команде 1     !!!!!!!!!!!!!!!!!!!!!!!!!!переделать
        //коды ответа:
        // 2 - неправильный выбор.. ??????
        // 5 - получено слишком мало байтов
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды???
        // 7 - включена блокировка записи по харту (видимо)
        //16 - доступ запрещен
        //32 - занят
          Buf_Out.buf[3]= 44;  // выполнена команда 44
          if (Buf_In.buf[1]<2+1)
           {
             Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
           }
           else
           {
            if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
            else
            {
             if (Buf_In.buf[4]!=Actual.P_Unit)
             {
              for (i=0; i< (sizeof(P_Unit)/sizeof(P_unit_record));i++ )
               {
                if (P_Unit[i].Code == Buf_In.buf[4])
                 {
                    Actual.P_Unit  = Buf_In.buf[4];
                    Actual_2_User();
                    if (Error_Code == Write2Flash_Error)
                      Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                    break;
                 }
               }
              if (i>= sizeof(P_Unit)/sizeof(P_unit_record))
                Buf_Out.buf[4] = 2;   //неправильный выбор
             }
            }
           }
           if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
               Buf_Out.buf[6] = User.P_Unit;
               Buf_Out.buf[1] = 4 + 1 ;              //запуск передачи
               SPI_Command = 5; // команда - передать статус датчика
             }
          break;
      case 45:  // подстройка 4 ма   // количество байт в команде 4
        //коды ответа:
        // 3 - параметр слишком велик
        // 4 - параметр слишком мал
        // 5 - получено слишком мало байтов
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        // 9 - неверный режим токовой петли или значение тока
        //11 - режим токовой петли отключен
        //16 - доступ запрещен
        //32 - занят
         Buf_Out.buf[3] = 45;  // выполнена команда 45
         if (Buf_In.buf[1]<2+4)
          {
            Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
          }
         else
         {
          if (HART_write_protect_value)
           Buf_Out.buf[4] = 7;     // In Write Protect Mode
          else
          {
          if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Not_Active)
           {
             Buf_Out.buf[4] = 11;     // режим токовой петли отключен = Loop Current Not Active (Device in Multidrop Mode)
             DAC_Code.w = DAC_Out;
           }
          else
           if (DAC_Out != Actual.DAC_Code_4_mA)
            Buf_Out.buf[4] = 9;        // Incorrect Loop Current Mode or Value
           else
           {
             for (i=0;i<4;i++)
               Float_Buf_Tmp.c[3-i] = Buf_In.buf[4+i];
             if (Float_Buf_Tmp.f > 23.5)
                Buf_Out.buf[4] = 3;     //  параметр слишком велик
               else
                if((Float_Buf_Tmp.f < 3.75))
                  Buf_Out.buf[4] = 4;     //  параметр слишком мал
                   else
                    if ((Float_Buf_Tmp.f < 3.75) || (Float_Buf_Tmp.f >5.25) )
                       Buf_Out.buf[4] = 9;     //  неверный режим токовой петли или значение тока
                      else
                    {
                     Actual.DAC_Code_4_mA = Actual.DAC_Code_4_mA - (int)((Float_Buf_Tmp.f - 4.0) * (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) * 0.0625 + 0.5);
                     Code_Down = Actual.DAC_Code_4_mA ;
                     Calculate_Span();
                     Actual_2_User();
                     if (Error_Code == Write2Flash_Error)
                       Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                    }
            }
           }
         }
          if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
              Float_Buf_Tmp.f = ((float)DAC_Code.w - (float)Actual.DAC_Code_4_mA) * 16.0 / (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) + 4.0;
              for (i=0;i<4;i++)
                Buf_Out.buf[6+i] =  Float_Buf_Tmp.c[3-i] ;
              Buf_Out.buf[1] = 4 + 4 ;              //запуск передачи
              SPI_Command = 5; // команда - передать статус датчика
             }
        break;
      case 46:  // подстройка 20 ма   // количество байт в команде 4
        //коды ответа:
        // 3 - параметр слишком велик
        // 4 - параметр слишком мал
        // 5 - получено слишком мало байтов
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?
        // 7 - включена блокировка записи по харту (видимо)
        // 9 - неверный режим токовой петли или значение тока
        //11 - режим токовой петли отключен
        //16 - доступ запрещен
        //32 - занят
         Buf_Out.buf[3] = 46;  // выполнена команда 46
         if (Buf_In.buf[1]<2+4)
          {
            Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
          }
         else
         {
          if (HART_write_protect_value)
           Buf_Out.buf[4] = 7;     // In Write Protect Mode
          else
          {
          if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Not_Active)
           {
             Buf_Out.buf[4] = 11;     // режим токовой петли отключен = Loop Current Not Active (Device in Multidrop Mode)
             DAC_Code.w = DAC_Out;
           }
          else
           if (DAC_Out != Actual.DAC_Code_20_mA)
            Buf_Out.buf[4] = 9;        // Incorrect Loop Current Mode or Value
           else
           {
            for (i=0;i<4;i++)
              Float_Buf_Tmp.c[3-i] = Buf_In.buf[4+i];
            if (Float_Buf_Tmp.f > 23.5)
                Buf_Out.buf[4] = 3;     //  параметр слишком велик
             else
              if((Float_Buf_Tmp.f < 3.75))
                  Buf_Out.buf[4] = 4;     //  параметр слишком мал
                else
                 if ((Float_Buf_Tmp.f < 18.0) || (Float_Buf_Tmp.f >22.0) )
                   Buf_Out.buf[4] = 9;     //  неверный режим токовой петли или значение тока
                  else
                {
                  Actual.DAC_Code_20_mA = Actual.DAC_Code_20_mA - (int)((Float_Buf_Tmp.f-20.0) * (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) * 0.0625 + 0.5);
                  Code_Up = Actual.DAC_Code_20_mA;
                  Calculate_Span();
                  Actual_2_User();
                  if (Error_Code == Write2Flash_Error)
                    Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                }
           }
          }
         }

          if (Buf_Out.buf[4])
             {
               Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
             }
            else
             {
              Float_Buf_Tmp.f = ((float)DAC_Code.w - (float)Actual.DAC_Code_4_mA) * 16.0 / (Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA) + 4.0;
              for (i=0;i<4;i++)
                Buf_Out.buf[6+i] =  Float_Buf_Tmp.c[3-i] ;
              Buf_Out.buf[1] = 4 + 4 ;              //запуск передачи
              SPI_Command = 5; // команда - передать статус датчика
             }
         break;
      case 47:  // установить функцию    // количество байт в команде 1         // обработку ошибок - нереализованных функций сделать
        //коды ответа:
        // 2 - неправильный выбор.. ??????
        // 5 - получено слишком мало байтов
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды???
        // 7 - включена блокировка записи по харту (видимо)
        //16 - доступ запрещен
        //32 - занят
           Buf_Out.buf[3]= 47;  // выполнена команда 47
           if (Buf_In.buf[1]<2+1)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
               Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
              if (Buf_In.buf[4]>1)       // линейная х-ка
                 Buf_Out.buf[4] = 2;        //не поддерживается функция
               else
                {
                 if (Buf_In.buf[4])       // == 1  корнеизвлекающая х-ка
                   {
                     Actual.Math_Mode |= FTransfer_SQRT;
                   }
                  else                   // == 0   линейная х-ка
                   {
                    Actual.Math_Mode &= ~FTransfer_SQRT;
                   }
                 Actual_2_User();
                 if (Error_Code == Write2Flash_Error)
                   Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                }
             }
            }
          if (Buf_Out.buf[4])
            {
              Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
            }
           else
            {
              if  (Actual.Math_Mode & FTransfer_SQRT)
                Buf_Out.buf[6] = 1;
               else
                Buf_Out.buf[6] = 0;
              Buf_Out.buf[1] = 4 + 1 ;              //запуск передачи
              SPI_Command = 5; // команда - передать статус датчика
            }
        break;
      case 48:  // читать дополнительный статус устройства // для family - не необходимо?
        //коды ответа:
        //  5 - получено слишком мало байтов
        //  6 - специфическая для прибора ошибка выполнения этой команды
        //  8 - предупреждение - обновление в процессе
        // 14 - несовпадение статуса в запросе с актуальным
        // 16 - доступ запрещен
       //   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! закомментировано и исправлено для прохождения теста - отладка!!!!!!!!!!!!!!!!!!!
               Buf_Out.buf[3]= 48;    //  выполнена команда 48
               if (Buf_In.buf[1]>=11)
                {
                 for (i=0;i<9;i++)
                  if (Buf_In.buf[4+i] != Additional_Device_Status.Byte_Array[i])
                    break;
                 if (i>8) // совпадение - зачистить бит More Status Available для запрашивающего мастера
                 {
                  if (Buf_In.buf[2] == 129)
                    Device_Status_Byte.Primary_Master &= ~FMore_Status_Available;
                   else
                    Device_Status_Byte.Secondary_Master &= ~FMore_Status_Available;
                 }
                 else
                   Buf_Out.buf[4] = 14; // несовпадение статуса в запросе с актуальным (предупреждение)
                }
               else
                 if (Buf_In.buf[1]>2)
                  {
                    Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
                  }
               // Additional_Device_Status.Extended_Device_Status |= FDevice_Variable_Alert; // пример использования - взводится флаг при сигнале токовой аварии или еще какие проблемы с током:)
               // Device_Status_Byte.Primary_Master |= FMore_Status_Available;               // одновременно устанавливается флаги More Status Available для первого и второго мастера
               // Device_Status_Byte.Secondary_Master |= FMore_Status_Available;
             if ( Buf_Out.buf[4] != 5)
              {
               for (i=0;i<9;i++)
                 Buf_Out.buf[6+i] = Additional_Device_Status.Byte_Array[i];
               Buf_Out.buf[1] = 4 + 9; //запуск передачи
              }
              else
                Buf_Out.buf[1] = 4; //запуск передачи  - ошибка - мало байтов в посылке

        break;
      case 54: //Read Device Variable Information
        //коды ответа:
        //  2 - Invalid Selection
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        // 16 - Access Restricted
        // 32 - Busy
           Buf_Out.buf[3]= 54;  // выполнена команда 54
           if (Buf_In.buf[1]<2+1)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
              Buf_Out.buf[1] = 4; //запуск передачи  - ошибка - мало байтов в посылке
            }
            else
            {
             i=6;
             if (Buf_In.buf[4] > 243 )
                Device_Variable_Code = Buf_In.buf[4] - 244;
              else
                Device_Variable_Code = Buf_In.buf[4] + 2;

             switch (Device_Variable_Code)
              {
               case 0:    // Percent Range

                       Buf_Out.buf[i++] = 244;          // Device Variable Code
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 57;           // Device Variable Limits/Minimum Span Units Code
                       for (j=0;j<4;j++)             // Limits/Minimum Span/Damping Value - not applicable
                       {
                         Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                         Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                       }
                       Buf_Out.buf[i++] = 0;            // Device Variable Classification (Not Yet Implemented) ?
                       Buf_Out.buf[i++] = 250;          // Device Variable Family (Not Used)
                       Buf_Out.buf[i++] = 0x00;       //   Update Time Period
                       Buf_Out.buf[i++] = 0x00;       //   100 ms / 0.03125
                       Buf_Out.buf[i++] = 0x0c;       //
                       Buf_Out.buf[i++] = 0x80;       //

                   break;
               case 1:    // Loop Current
                       Buf_Out.buf[i++] = 245;          // Device Variable Code
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 39;           // Device Variable Limits/Minimum Span Units Code = mA
                       for (j=0;j<4;j++)             // Limits/Minimum Span/Damping Value - not applicable
                       {
                         Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                         Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                       }
                       Buf_Out.buf[i++] = 84;           // Device Variable Classification
                       Buf_Out.buf[i++] = 250;          // Device Variable Family (Not Used)
                       Buf_Out.buf[i++] = 0x00;       //   Update Time Period
                       Buf_Out.buf[i++] = 0x00;       //   100 ms / 0.03125
                       Buf_Out.buf[i++] = 0x0c;       //
                       Buf_Out.buf[i++] = 0x80;       //
                   break;
               case 2:    //  Primary Variable
                       Buf_Out.buf[i++] = 246;          // Device Variable Code
                       for (j=9; j < 12; j++ )                     // Device Variable Transducer Serial Number
                        Buf_Out.buf[i++] = HART_Cmd_0_Data[j];     // Device Variable Transducer Serial Number ?
                       for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
                        {
                         if (P_Unit[j].Code == Actual.P_Unit)
                            break;
                        }
                       Buf_Out.buf[i++] = P_Unit[j].Code;     // Device Variable Limits/Minimum Span Units Code
                       Float_Buf_Tmp.f = Pass.Pmax_Lim*P_Unit[j].Kout_P[0];      // Upper Transducer Limit
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];

                       Float_Buf_Tmp.f = Pass.Pmin_Lim*P_Unit[j].Kout_P[0];      //    Lower Transducer Limit
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];

                       Float_Buf_Tmp.f = Actual.Filter_Limit*(1.0/15.87);         // Device Variable Damping Value
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];

                       Float_Buf_Tmp.f = (Pass.Pmax_Lim - Pass.Pmin_Lim)/Pass.K_Rearranging;  //  Minimum Span
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];

                       Buf_Out.buf[i++] = 65;       //  Device Variable Classification (давление)
                       Buf_Out.buf[i++] = 5;          // Device Variable Family (Pressure)
                       Buf_Out.buf[i++] = 0x00;       //   Update Time Period
                       Buf_Out.buf[i++] = 0x00;       //   100 ms / 0.03125
                       Buf_Out.buf[i++] = 0x0c;       //
                       Buf_Out.buf[i++] = 0x80;       //
                   break;
               case 3:    //  Secondary Variable
                       Buf_Out.buf[i++] = 247;      // Device Variable Code
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 32;           // Device Variable Limits/Minimum Span Units Code = grad C

                       Float_Buf_Tmp.f = Pass.Tmax;      // Upper Transducer Limit
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];

                       Float_Buf_Tmp.f = Pass.Tmin;      //    Lower Transducer Limit
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                       Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];
                                                         // Device Variable Damping Value
                       Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                       Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                       Buf_Out.buf[i++] = 0x00;       //   (NaN)
                       Buf_Out.buf[i++] = 0x00;       //   (NaN)
                                                         // Device Variable Minimum Span
                       Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                       Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                       Buf_Out.buf[i++] = 0x00;       //   (NaN)
                       Buf_Out.buf[i++] = 0x00;       //   (NaN)

                       Buf_Out.buf[i++] = 64;       //  Device Variable Classification (температура)
                       Buf_Out.buf[i++] = 4;          // Device Variable Family (Temperature)
                       Buf_Out.buf[i++] = 0x00;       //   Update Time Period
                       Buf_Out.buf[i++] = 0x01;       //   4 s / 0.03125
                       Buf_Out.buf[i++] = 0xf4;       //
                       Buf_Out.buf[i++] = 0x00;       //
                   break;

               default:
                       Buf_Out.buf[i++] = Buf_In.buf[4];          // Device Variable Code
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 0;            // Device Variable Transducer Serial Number
                       Buf_Out.buf[i++] = 250;           // Device Variable Limits/Minimum Span Units Code  (Not Used)
                       for (j=0;j<4;j++)             // Limits/Minimum Span/Damping Value - not applicable
                       {
                         Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                         Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                         Buf_Out.buf[i++] = 0x00;       //   (NaN)
                       }
                       Buf_Out.buf[i++] = 0;            // Device Variable Classification (Not Yet Implemented) ?
                       Buf_Out.buf[i++] = 250;          // Device Variable Family (Not Used)
                       Buf_Out.buf[i++] = 0x00;       //   Update Time Period
                       Buf_Out.buf[i++] = 0x00;       //   100 ms / 0.03125
                       Buf_Out.buf[i++] = 0x00;       //
                       Buf_Out.buf[i++] = 0x00;       //
                      // Buf_Out.buf[4] = 2;     // Invalid Selection
                   break;
              }
              Buf_Out.buf[1] = i-2;  //запуск передачи
             }
           break;
      case 59:
// Задать количество преамбул в ответе, 1 байт, 5<=, >=20  // количество байт в команде 1
        //коды ответа:
        //  3 - параметр слишком велик
        //  4 - параметр слишком мал
        //  5 - получено слишком мало байтов
        //  6 - ошибочная команда или ошибка специфики выполнения этой команды?
        //  7 - включена блокировка записи по харту
        //  8 - установлено в ближайшее допустимое значение
        // 16 - доступ запрещен
        // 32 - занято

            Buf_Out.buf[3]= 59;    //  выполнена команда 59
            if (Buf_In.buf[1]<2+1)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
             {
              if (HART_write_protect_value)
                Buf_Out.buf[4] = 7;     // In Write Protect Mode
               else
                {
                 if ( Buf_In.buf[4] < 5)
                  {
                    Buf_Out.buf[4] = 4; //  HART_Error параметр слишком мал
                  }
                  else
                   {
                    if   (Buf_In.buf[4] > 20)
                     {
                       Buf_Out.buf[4] = 3; //  HART_Error параметр слишком велик
                     }
                     else
                    {
                     if ( write_data2flash( (unsigned long int)((char *) &(HART_Cmd_0_Data[12])), (char *) &Buf_In.buf[4], 1) )
                      {
                        Error_Code = Write2Flash_Error;
                        Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                      }
                    }
                  }
                }
              }
            if (Buf_Out.buf[4])
              {
                Buf_Out.buf[1] = 4;  //запуск передачи , ошибка
              }
             else
              {
                Buf_Out.buf[6] = HART_Cmd_0_Data[12];
                Buf_Out.buf[1] = 4 + 1; //запуск передачи
                SPI_Command = 60; // команда - передать  HART адрес и константы
              }
        break;


      case 80:  // читать точки подстройки Read Device Variable Trim Points

        //коды ответа:
        //  5 - получено слишком мало байтов
        //  6 - специфическая для прибора ошибка выполнения этой команды
        // 17	Error	Invalid Device Variable Index. The Device Variable does not exist in this Field Device.
        // 19	Error	Device Variable index not allowed for this command.
        // 32	Error	Busy

               Buf_Out.buf[3]= 80;    //  выполнена команда 80
               i=6;
               if (Buf_In.buf[1]<2+1)
                 {
                  Buf_Out.buf[4] = 5;           // Error  Too Few Data Bytes Received
                 }
                else
                {
                 Buf_Out.buf[i++] = Buf_In.buf[4];      // 0
                 //if ( Buf_In.buf[4] == 246)
                 if ((Buf_In.buf[4] == 246)||(Buf_In.buf[4] == 0)) //добавили в Device Variable  0 для согласования с DD 11.12.2017
                  {
                   Buf_Out.buf[i++] = Actual.P_Unit;      //  Trim Points Units Code    // 1

                   for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
                    {
                     if (P_Unit[j].Code == Actual.P_Unit)
                      break;
                    }

                   Float_Buf_Tmp.f = Flash_Trim_Data.Lower_Trim_Point_Value*P_Unit[j].Kout_P[0];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 2-5

                   Float_Buf_Tmp.f = Flash_Trim_Data.Upper_Trim_Point_Value*P_Unit[j].Kout_P[0];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                     // 6-9
                  }
                  else
                   {
                    Buf_Out.buf[4] = 19;     // Error   Device Variable index not allowed for this command
                    switch (Buf_In.buf[4])
                     {
                      case 244:    // Percent Range
                           Buf_Out.buf[i++] = 57;           // Trim Points Units Code = %
                        break;
                      case 245:    // Loop Current
                           Buf_Out.buf[i++] = 39;           // Trim Points Units Code = mA
                        break;
                      case 247:    //  Secondary Variable
                           Buf_Out.buf[i++] = 32;           // Trim Points Units Code = grad C
                         break;
                      default:
                           Buf_Out.buf[4] = 17;         // Error   The Device Variable does not exist in this Field Device.
                           Buf_Out.buf[i++] = 250;      //  Trim Points Units Code   (Not Used)                   // 1
                         break;
                     }
                    for (j=0;j<2;j++)             // Lower/Upper Trim Point Value                            // 2-9
                     {
                      Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                      Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                      Buf_Out.buf[i++] = 0x00;       //   (NaN)
                      Buf_Out.buf[i++] = 0x00;       //   (NaN)
                     }
                   }
                }
              Buf_Out.buf[1] = i-2;  //запуск передачи

        break;
      case 81:  //  Read Device Variable Trim Guidelines
        //коды ответа:
        //  5 - получено слишком мало байтов
        //  6 - специфическая для прибора ошибка выполнения этой команды
        // 17	Error	Invalid Device Variable Index. The Device Variable does not exist in this Field Device.
        // 19	Error	Device Variable index not allowed for this command.
        // 32	Error	Busy

               Buf_Out.buf[3]= 81;    //  выполнена команда 81
               i=6;
               if (Buf_In.buf[1]<2+1)
                 {
                  Buf_Out.buf[4] = 5;           // Error  Too Few Data Bytes Received
                 }
                else
                {
                 Buf_Out.buf[i++] = Buf_In.buf[4];      // 0
                 //if (Buf_In.buf[4] == 246)
                 if ((Buf_In.buf[4] == 246)||(Buf_In.buf[4] == 0)) //добавили в Device Variable  0 для согласования с DD 11.12.2017
                  {
                   Buf_Out.buf[i++] = 3;        //  Lower & Upper // Trim points supported     // 1
                   Buf_Out.buf[i++] = Actual.P_Unit;            //  Trim Points Units Code     // 2

                   for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
                    {
                     if (P_Unit[j].Code == Actual.P_Unit)
                      break;
                    }

                   RAM_Trim_Data.Lower_Trim_Point_Value = Actual.Pmin*P_Unit[j].Kout_P[0];
                   RAM_Trim_Data.Upper_Trim_Point_Value = Actual.Pmax*P_Unit[j].Kout_P[0];
                   RAM_Trim_Data.Trim_Differential = RAM_Trim_Data.Upper_Trim_Point_Value - RAM_Trim_Data.Lower_Trim_Point_Value;

                   Float_Buf_Tmp.f = RAM_Trim_Data.Lower_Trim_Point_Value - Minimum_Lower_Trim_Coefficient * RAM_Trim_Data.Trim_Differential  ;  // Minimum Lower Trim Point Value
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 3-6

                   Float_Buf_Tmp.f = RAM_Trim_Data.Lower_Trim_Point_Value + Maximum_Lower_Trim_Coefficient * RAM_Trim_Data.Trim_Differential ;  // Maximum  Lower Trim Point Value
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                      // 7-10

                   Float_Buf_Tmp.f = RAM_Trim_Data.Upper_Trim_Point_Value - Minimum_Upper_Trim_Coefficient * RAM_Trim_Data.Trim_Differential ;  // Minimum Upper Trim Point Value
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                     // 11-14

                   Float_Buf_Tmp.f =  RAM_Trim_Data.Upper_Trim_Point_Value + Maximum_Upper_Trim_Coefficient * RAM_Trim_Data.Trim_Differential;  // Maximum Upper Trim Point Value
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                     // 15-18

                   Float_Buf_Tmp.f = Minimum_Differential_Trim_Coefficient *RAM_Trim_Data.Trim_Differential;  	//  Minimum Differential
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[3];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[2];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[1];
                   Buf_Out.buf[i++] = Float_Buf_Tmp.c[0];                                                    // 19-22
                  }
                  else
                   {
                    Buf_Out.buf[i++] = 0;        //  NO Trim points supported     // 1
                    Buf_Out.buf[4] = 19;     // Error   Device Variable index not allowed for this command
                    switch (Buf_In.buf[4])
                     {
                      case 244:    // Percent Range
                           Buf_Out.buf[i++] = 57;           // Trim Points Units Code = %
                        break;
                      case 245:    // Loop Current
                           Buf_Out.buf[i++] = 39;           // Trim Points Units Code = mA
                        break;
                      case 247:    //  Secondary Variable
                           Buf_Out.buf[i++] = 32;           // Trim Points Units Code = grad C
                         break;
                      default:
                           Buf_Out.buf[4] = 17;         // Error   The Device Variable does not exist in this Field Device.
                           Buf_Out.buf[i++] = 250;          //  Trim Points Units Code   (Not Used)          // 2
                         break;
                     }
                    for (j=0;j<5;j++)             // Minimum/Maximum Lower/Upper Trim Point Value & Minimum Differential
                     {                                                                                    // 3-22
                      Buf_Out.buf[i++] = 0x7F;       //   (NaN)
                      Buf_Out.buf[i++] = 0xA0;       //   (NaN)
                      Buf_Out.buf[i++] = 0x00;       //   (NaN)
                      Buf_Out.buf[i++] = 0x00;       //   (NaN)
                     }
                   }
                }
              Buf_Out.buf[1] = i-2;  //запуск передачи
        break;
      case 82:    //  Write Device Variable Trim Point
        // коды ответа:
        // 2 - Error	Invalid Selection
        // 3 - Error	Passed Parameter Too Large
        // 4 - Error	Passed Parameter Too Small
        // 5 - получено слишком мало байтов  Too Few Data Bytes Received
        // 6 - ошибочная команда или ошибка специфики выполнения этой команды?  Device Specific Command Error
        // 7 - включена блокировка записи по харту In Write Protect Mode
        // 8 - Undefined
        // 9 - Error	Applied Process Too High
        //10 - Error	Applied Process Too Low
        //11 - Error	Trim Error. Excess Correction Attempted
        //12 - Undefined
        //13 - Error	Computation Error. Trim Values Were Not Changed
        //14 - Warning	Span Too Small
        //16 - Error	Access Restricted     доступ запрещен
        //17 - Error	Invalid Device Variable Index. The Device Variable does not exist in this Field Device.
        //18 - Error	Invalid Units Code         неправильный код единиц измерений
        //19 - Error	Device Variable index not allowed for this command.

        //32 - занят
        Buf_Out.buf[3]= 82;  // выполнена команда 82
        i=6;
        if (Buf_In.buf[1]<2+7)
        {
          Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
        }
        else
        {

          if (( Buf_In.buf[5] != 2)&&( Buf_In.buf[5] != 1))
            Buf_Out.buf[4] = 2;     // Invalid Selection
          else
            //if ( Buf_In.buf[4] != 246)
            if ((Buf_In.buf[4] != 246)&&(Buf_In.buf[4] != 0)) //добавили в Device Variable  0 для согласования с DD 11.12.2017
            {
              Buf_Out.buf[4] = 19;     // Error   Device Variable index not allowed for this command
            }
            else
            {
              for (j=0;j<4;j++)
                Float_Buf_Tmp.c[3-j] = Buf_In.buf[7+j];
              //******************** выбор единиц ***************************
              for (j=0; j< (sizeof(P_Unit)/sizeof(P_unit_record));j++ )
              {
                if (P_Unit[j].Code == Buf_In.buf[6])
                  break;
              }
              if (j>= sizeof(P_Unit)/sizeof(P_unit_record))
                Buf_Out.buf[4] = 18;   //неправильный код единиц измерений  Invalid Units Code
              else
              {
                Buf_Out.buf[i++] = Buf_In.buf[4];      // 0
                Buf_Out.buf[i++] = Buf_In.buf[5];     // Trim Point    // 1
                Buf_Out.buf[i++] = Buf_In.buf[6];      //  Trim Points Units Code   // 2
                if (HART_write_protect_value)
                  Buf_Out.buf[4] = 7;     // In Write Protect Mode
                else
                {
                  Float_Buf_Tmp.f *= P_Unit[j].Kout_P[1];

                  __bic_SR_register(GIE);   //Запретить прерывания
                  Float_Tmp_Buf = P_Out;
                  __bis_SR_register(GIE);   // разрешить прерывания

                  RAM_Trim_Data.Trim_Differential = (Pass.Pmax_Lim - Pass.Pmin_Lim)*Applied_Process_Deviation_Coefficient;
                  if ((Float_Tmp_Buf > Float_Buf_Tmp.f)&&(Float_Tmp_Buf - Float_Buf_Tmp.f > RAM_Trim_Data.Trim_Differential))
                  {
                    Buf_Out.buf[4] = 9;     //  9 - Error	Applied Process Too High
                  }
                  else
                    if ((Float_Tmp_Buf < Float_Buf_Tmp.f)&&(Float_Buf_Tmp.f - Float_Tmp_Buf > RAM_Trim_Data.Trim_Differential))
                    {
                      Buf_Out.buf[4] = 10;    // 10 - Error	Applied Process Too Low
                    }
                    else
                    {
                      RAM_Trim_Data.Lower_Trim_Point_Value = Actual.Pmin;
                      RAM_Trim_Data.Upper_Trim_Point_Value = Actual.Pmax;
                      RAM_Trim_Data.Trim_Differential = RAM_Trim_Data.Upper_Trim_Point_Value - RAM_Trim_Data.Lower_Trim_Point_Value;
                      if ( Buf_In.buf[5] == 1)
                      {                                          // Lower_Trim_Point
                        if ((RAM_Trim_Data.Lower_Trim_Point_Value > Float_Buf_Tmp.f)&&(RAM_Trim_Data.Lower_Trim_Point_Value - Float_Buf_Tmp.f > RAM_Trim_Data.Trim_Differential*Minimum_Lower_Trim_Coefficient))
                        {
                          Buf_Out.buf[4] = 4 ;     //  4 - Error	Passed Parameter Too Small  нижний предел слишком мал
                        }
                        else
                          if ((RAM_Trim_Data.Lower_Trim_Point_Value < Float_Buf_Tmp.f)&&(Float_Buf_Tmp.f - RAM_Trim_Data.Lower_Trim_Point_Value > RAM_Trim_Data.Trim_Differential*Maximum_Lower_Trim_Coefficient))
                          {
                            Buf_Out.buf[4] = 3 ;     //  3 - Error	Passed Parameter Too Large   нижний предел слишком велик
                          }
                          else
                          {
                            RAM_Trim_Data.Lower_Trim_Point_Value = Float_Buf_Tmp.f;
                            __bic_SR_register(GIE);   //Запретить прерывания
                            RAM_Trim_Data.PpEt_for_Lower_Trim_Point_Value = PpEt;
                            __bis_SR_register(GIE);   // разрешить прерывания
                            write_data2flash( (unsigned long int)((char *) &Flash_Trim_Data.Lower_Trim_Point_Value), (char *) &RAM_Trim_Data.Lower_Trim_Point_Value,(int) 8) ;
                          }
                      }
                      else
                      {                                         // Upper_Trim_Point
                        if ((RAM_Trim_Data.Upper_Trim_Point_Value > Float_Buf_Tmp.f)&&(RAM_Trim_Data.Upper_Trim_Point_Value - Float_Buf_Tmp.f > RAM_Trim_Data.Trim_Differential*Minimum_Upper_Trim_Coefficient))
                        {
                          Buf_Out.buf[4] = 4 ;     //  4 - Error	Passed Parameter Too Small  верхний предел слишком мал
                        }
                        else
                          if ((RAM_Trim_Data.Upper_Trim_Point_Value < Float_Buf_Tmp.f)&&(Float_Buf_Tmp.f - RAM_Trim_Data.Upper_Trim_Point_Value > RAM_Trim_Data.Trim_Differential*Maximum_Upper_Trim_Coefficient))
                          {
                            Buf_Out.buf[4] = 3 ;     //  3 - Error	Passed Parameter Too Large   верхний предел слишком велик
                          }
                          else
                          {
                            RAM_Trim_Data.Upper_Trim_Point_Value = Float_Buf_Tmp.f;
                            __bic_SR_register(GIE);   //Запретить прерывания
                            RAM_Trim_Data.PpEt_for_Upper_Trim_Point_Value = PpEt;
                            __bis_SR_register(GIE);   // разрешить прерывания
                            write_data2flash( (unsigned long int)((char *) &Flash_Trim_Data.Upper_Trim_Point_Value), (char *) &RAM_Trim_Data.Upper_Trim_Point_Value,(int) 8) ;
                          }
                      }
                      if (Flash_Trim_Data.Upper_Trim_Point_Value - Flash_Trim_Data.Lower_Trim_Point_Value < RAM_Trim_Data.Trim_Differential*Minimum_Differential_Trim_Coefficient )
                        Buf_Out.buf[4] = 14;                       //14 - Warning	Span Too Small
                    }
                }
                if (1==Buf_In.buf[5])
                  Float_Buf_Tmp.f = Flash_Trim_Data.Lower_Trim_Point_Value;
                else
                  Float_Buf_Tmp.f = Flash_Trim_Data.Upper_Trim_Point_Value;
                for (j=0;j<4;j++)
                  Buf_Out.buf[i++] = Float_Buf_Tmp.c[3-j];
              }
            }
        }

        Buf_Out.buf[1] = i-2;    //запуск передачи
        if ((!(Buf_Out.buf[4]))|| (Buf_Out.buf[4] == 14))// здесь пересчитать коэффициенты..
        {
          RAM_Trim_Data.Trim_Differential = Flash_Trim_Data.Upper_Trim_Point_Value * Flash_Trim_Data.PpEt_for_Lower_Trim_Point_Value - Flash_Trim_Data.Lower_Trim_Point_Value * Flash_Trim_Data.PpEt_for_Upper_Trim_Point_Value;
          Actual.Zero_Shift = (RAM_Trim_Data.Trim_Differential)/(Flash_Trim_Data.Upper_Trim_Point_Value - Flash_Trim_Data.Lower_Trim_Point_Value);
          // -- modify 11.09.2018 start
          //--old-- 
		  //Actual.Span = Flash_Trim_Data.Upper_Trim_Point_Value/(Flash_Trim_Data.PpEt_for_Upper_Trim_Point_Value - Actual.Zero_Shift);
          //--new--
          if(Flash_Trim_Data.Upper_Trim_Point_Value!=0)
            Actual.Span = Flash_Trim_Data.Upper_Trim_Point_Value/(Flash_Trim_Data.PpEt_for_Upper_Trim_Point_Value - Actual.Zero_Shift);
          else
            Actual.Span = Flash_Trim_Data.Lower_Trim_Point_Value/(Flash_Trim_Data.PpEt_for_Lower_Trim_Point_Value - Actual.Zero_Shift);
          //-- modify 11.09.2018 end

          Actual_2_User();
        }

        break;
   case 83: //  Reset Device Variable Trim
        //коды ответа:
        //  2	Invalid Selection ?????
        //  5 - получено слишком мало байтов
        //              вроде как нет такой здесь   6 - специфическая для прибора ошибка выполнения этой команды
        //  7	Error	In Write Protect Mode
        // 16	Error	Access Restricted ?????
        // 17	Error	Invalid Device Variable Index. The Device Variable does not exist in this Field Device.
        // 19	Error	Device Variable index not allowed for this command.
        // 32	Error	Busy (A DR Could Not Be Started)
        // 33	Error	DR Initiated
        // 34	Error	DR Running
        // 35	Error	DR Dead
        // 36	Error	DR Conflict


               Buf_Out.buf[3]= 83;    //  выполнена команда 83
               i=6;
               if (Buf_In.buf[1]<2+1)
                 {
                  Buf_Out.buf[4] = 5;           // Error  Too Few Data Bytes Received
                 }
                else
                 if (HART_write_protect_value)
                   Buf_Out.buf[4] = 7;     // In Write Protect Mode
                  else
                   //if ((Buf_In.buf[4] < 244)||(Buf_In.buf[4] > 247))
                   if ((( Buf_In.buf[4] < 244)||(Buf_In.buf[4] > 247)) && (Buf_In.buf[4] != 0))//добавили исключение для Device Variable  0 для согласования с DD 11.12.2017
                     Buf_Out.buf[4] = 17;         // Error   The Device Variable does not exist in this Field Device.
                   else
                     //if ( Buf_In.buf[4] != 246)
                      if ((Buf_In.buf[4] != 246)&&(Buf_In.buf[4] != 0)) //добавили в Device Variable  0 для согласования с DD 11.12.2017
                       Buf_Out.buf[4] = 19;     // Error   Device Variable index not allowed for this command
               Buf_Out.buf[i++] = Buf_In.buf[4];  // Device Variable trim to reset      // 0
               Buf_Out.buf[1] = i-2;  //запуск передачи
               if (!(Buf_Out.buf[4]))
                {                                                  // reset
                 Actual.Zero_Shift = 0;  //пользовательская коррекция нуля
                 Actual.Span = 1;        //User.Span;
                 Actual_2_User();
                                                                   // reset  Trim Points  to Plant Trim Points
                 RAM_Trim_Data.Lower_Trim_Point_Value = Pass.Pmin_Lim;
                 RAM_Trim_Data.PpEt_for_Lower_Trim_Point_Value = Pass.Pmin_Lim;
                 RAM_Trim_Data.Upper_Trim_Point_Value = Pass.Pmax_Lim;
                 RAM_Trim_Data.PpEt_for_Upper_Trim_Point_Value = Pass.Pmax_Lim;
                 write_data2flash( (unsigned long int)((char *) &Flash_Trim_Data), (char *) &RAM_Trim_Data,(int) 16) ;
                }

        break;


      case 103: //Write Burst Period
        //коды ответа:
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        //  7 - In Write Protect Mode
        //  8 - warning - Update Times Adjusted
        //  9 - Invalid Burst Message
        // 16 - Access Restricted
        // 32 - Busy (A DR Could Not Be Started
        // 33 - DR Initiated
        // 34 - DR Running
        // 35 - DR Dead
        // 36 - DR Conflict
           Buf_Out.buf[3]= 103;  // выполнена команда 103
           i=6;
           if (Buf_In.buf[1] < 2 + 9)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
              Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
             if (Buf_In.buf[4] > 2)   // Burst Message №
              {
               Buf_Out.buf[4] = 9;     // Invalid Burst Message
              }
             else
             {
               ptr = (char*)&Time_Tmp_Buf1+3;           // Update Period
               for (j=5 ; j<9; j++)
                 *ptr-- = Buf_In.buf[j];

               ptr = (char*)&Time_Tmp_Buf2+3;           // Maximum Update Period
               for (; j<13; j++)
                 *ptr-- = Buf_In.buf[j];

               if (Time_Tmp_Buf2 < Time_Tmp_Buf1)    // MAX < Normal ?
                {
                  Buf_Out.buf[4] = 6;     // Device-Specific Command Error
                }
               else
                {
		  /*
                  Float_Buf_Tmp.f = Time_Tmp_Buf1*(1.15*1/7.8125/8.0/8.0);
                  Time_Tmp_Buf1 = (unsigned long int) (Float_Buf_Tmp.f+.5);
                  Float_Tmp_Buf = Time_Tmp_Buf2*(1.15*1/7.8125/8.0/8.0);
                  Time_Tmp_Buf2 = (unsigned long int) (Float_Tmp_Buf+.5);
		  */
                  if ( write_data2flash( (unsigned long int)((char *) &Burst_Message[Buf_In.buf[4]].Update_Time), (char *) &Time_Tmp_Buf1, 8) )
                  {
                    Error_Code = Write2Flash_Error;
                    Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                  }
                  else
                  {
                     Burst_Message_Number = Buf_In.buf[4];


                     Buf_Out.buf[i++] = Burst_Message_Number;
		     /*
                     ptr = (char*) &Burst_Message[Buf_In.buf[4]].Update_Time;
                     Time_Tmp_Buf1 = (unsigned long int) (*((unsigned long int*)ptr) * 7.8125*8.0*8.0/1.15);
                     ptr += 4;
                     Time_Tmp_Buf2 = (unsigned long int) (*((unsigned long int*)ptr) * 7.8125*8.0*8.0/1.15);
                     ptr = (char*) &Time_Tmp_Buf1+3;
                     for (;i<11;i++)
                       Buf_Out.buf[i] = *ptr--;
                     ptr = (char*) &Time_Tmp_Buf2+3;
                     for (;i<15;i++)
                       Buf_Out.buf[i] = *ptr--;
                     ptr = (char*) &Buf_Out.buf[14];
                     for (j=12 ; j>4; j--)
                       if (*ptr-- != Buf_In.buf[j])
                        {
                          Buf_Out.buf[4] = 8;   // warning - Update Times Adjusted
                          break;
                        }
		     */
		     for (j=5;i<15;i++,j++)
                       Buf_Out.buf[i] = Buf_In.buf[j];

                     SPI_Command = 63;
                  }
                }
             }
            }
            }

            Buf_Out.buf[1] = i-2;  //запуск передачи
       //     SPI_Command = 63;
         break;

      case 104: //Write Burst Trigger
        //коды ответа:
        //  2 - Invalid Selection
        //  3 - Passed Parameter Too Large
        //  4 - Passed Parameter Too Small
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        //  7 - In Write Protect Mode
        //  9 - Invalid Burst Message
        // 11 - Invalid Device Variable Classification
        // 12 - Invalid Units Code
        // 13 - Invalid Burst Trigger Mode Selection Code
        // 16 - Access Restricted
        // 32 - Busy (A DR Could Not Be Started)
        // 33 - DR Initiated
        // 34 - DR Running
        // 35 - DR Dead
        // 36 - DR Conflict
           Buf_Out.buf[3]= 104;  // выполнена команда 104
           i=6;
           if (Buf_In.buf[1] < 2 + 8)
            {
              Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
            }
            else
            {
             if (HART_write_protect_value)
              Buf_Out.buf[4] = 7;     // In Write Protect Mode
             else
             {
             if (Buf_In.buf[4] > 2)   // Burst Message №
              {
               Buf_Out.buf[4] = 9;     // Invalid Burst Message
              }
             else
             {
              ptrMessage = (char*)&Burst_Message[Buf_In.buf[4]].Burst_Mode_Control_Code;
              Burst_Message_Command_Number = *(ptrMessage + (&Burst_Message[0].Command_Number - &Burst_Message[0].Burst_Mode_Control_Code));
              if ((Buf_In.buf[5] > 4) || (((Buf_In.buf[5] != 4))&&(Burst_Message_Command_Number==48)))  // Burst Trigger Mode Selection Code
               {
                Buf_Out.buf[4] = 13;   // Invalid Burst Trigger Mode Selection Code
               }
              else
               {
                if (Buf_In.buf[5] > 0)
                {
                 switch(Burst_Message_Command_Number)
                 {
                  case 1:
                       Device_Variable_Code = 246 - 244;
                     break;
                  case 2:
                       Device_Variable_Code = 244 - 244;
                     break;
                  case 3:
                       Device_Variable_Code = 246 - 244;
                     break;
                  case 9:
                     //  Device_Variable_Code = *(ptrMessage + (&Burst_Message[0].Slot [0] - &Burst_Message[0].Burst_Mode_Control_Code)) - 244;

                           Device_Variable_Code = *(ptrMessage + (&Burst_Message[0].Slot [0] - &Burst_Message[0].Burst_Mode_Control_Code));
			   if (Device_Variable_Code >243)
                             Device_Variable_Code -= 244;
			   else
			     Device_Variable_Code += 2;

                     break;
                  default:
                     break;
                 }
                if ((Burst_Message_Command_Number != 48)&& (Device_Variable_Code < 4))
                {
                 if (Device_Variable[Device_Variable_Code].Device_Variable_Classification != Buf_In.buf[6])
                   Buf_Out.buf[4] = 11;   // Invalid Device Variable Classification
                 else
                  {
                   if (Device_Variable_Code != 2)
                     Buf_In.buf[24] = Device_Variable[Device_Variable_Code].Device_Variable_Units;
                    else
                     Buf_In.buf[24] = Actual.P_Unit;
                   if  (Buf_In.buf[7] != Buf_In.buf[24])                 //  проверить соответствие единиц
                      Buf_Out.buf[4] =  12;  // Invalid Units Code
                    else
                    {
                     if ((Buf_In.buf[5] == 2)||(Buf_In.buf[5] == 3))     //  в зависимости от типа - проверить порог
                     {
                      ptr = (char*)&Float_Tmp_Buf + 3;
                      for (j=8 ; j<12 ; j++)
                       {
                         *ptr-- = Buf_In.buf[j]  ;
                       }
                      switch(Device_Variable_Code)
                       {
                         case 0:  //  Percent Range
                            if (100.0 < Float_Tmp_Buf)
                               Buf_Out.buf[4] =  3;    //  Passed Parameter Too Large
                             else
                              if (0.0 > Float_Tmp_Buf)
                                 Buf_Out.buf[4] = 4;    // Passed Parameter Too Small
                                else
                                 {  // пересчитать параметр
                                   Float_Tmp_Buf *= (Actual.Pmax - Actual.Pmin)*0.01;
                                 }
                           break;
                         case 1:  //  Loop Current
                            if (20.0 < Float_Tmp_Buf)
                               Buf_Out.buf[4] =  3;    //  Passed Parameter Too Large
                             else
                              if (4.0 > Float_Tmp_Buf)
                                 Buf_Out.buf[4] = 4;    // Passed Parameter Too Small
                                else
                                 {  // пересчитать параметр
                                   Float_Tmp_Buf = (Actual.Pmax - Actual.Pmin)*(Float_Tmp_Buf - 4.0)*0.0625+Actual.Pmin;
                                 }
                           break;
                         case 2:  //  PV - давление
                            for (k=0; k< (sizeof(P_Unit)/sizeof(P_unit_record));k++ )
                             {
                               if (P_Unit[k].Code == Actual.P_Unit)
                                 break;
                             }
                            Float_Tmp_Buf *= P_Unit[k].Kout_P[1];
                            if (Pass.Pmax_Lim < Float_Tmp_Buf)
                               Buf_Out.buf[4] =  3;    //  Passed Parameter Too Large
                             else
                             {
                              if (Pass.Pmin_Lim > Float_Tmp_Buf)
                                Buf_Out.buf[4] = 4;    // Passed Parameter Too Small
                           /*    else
                               { // пересчитать и записать Trigger Level
                                 ptr = (char*)&Float_Tmp_Buf;
                                 for (j=8 ; j<12 ; j++)
                                  {
                                   Buf_In.buf[j] = *ptr++;
                                  }
                               }*/
                             }
                           break;
                         case 3:   //  SV - температура
                            if (Pass.Tmax < Float_Tmp_Buf)
                               Buf_Out.buf[4] =  3;    //  Passed Parameter Too Large
                             else
                              if (Pass.Tmin > Float_Tmp_Buf)
                                Buf_Out.buf[4] = 4;    // Passed Parameter Too Small
                           break;
                        default:
                           break;
                      }
                     }
                    }
                  }
                }
               }
                 if (!(Buf_Out.buf[4]))
                    {                                      //нет ошибок
                      ptr = (char*)&Float_Tmp_Buf;
                      for (j=8 ; j<12 ; j++)              // пересчитанную переменную скопировать в буфер приема
                       {
                         Buf_In.buf[j] = *ptr++;
                       }

                     ptr = ptrMessage + (&Burst_Message[0].Trigger_Mode - &Burst_Message[0].Burst_Mode_Control_Code);
                     if ( write_data2flash( (unsigned long int)(ptr), (char *) &Buf_In.buf[5], 7) )
                       {
                         Error_Code = Write2Flash_Error;
                         Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                       }
                      else
                       {
                         Burst_Message_Number = Buf_In.buf[4];
                         Buf_Out.buf[i++] = Burst_Message_Number;
                         for (;i<14;i++)         // передать текущие значения,
                           Buf_Out.buf[i] = *ptr++;
                         // пытаюсь соврать - возвращаю не из памяти, а те, что прислали, иначе надо будет все пересчитывать как при проверке на макс и мин выше
                         //не, уже пытаюсь честно все сделать
                         if ((Device_Variable_Code == 2) && ((Buf_In.buf[5] == 2)||(Buf_In.buf[5] == 3)))// пересчитать параметр
                         {
                           Float_Tmp_Buf *=  P_Unit[k].Kout_P[0];
                           ptr = (char*)&Float_Tmp_Buf + 3;
                           for (j=10 ; j<14 ; j++)
                              Buf_Out.buf[j] = *ptr-- ;
                           SPI_Command = 64;
                         }
                       }
                    }
               }
              }
             }
            }

         Buf_Out.buf[1] = i-2;  //запуск передачи
        //  SPI_Command = 64;
       break;
      case 105: //Read Burst Mode Configuration
        //коды ответа:
        //  6 - Device-Specific Command Error
        //  9 - Invalid Burst Message
        // 32 - Busy

           Buf_Out.buf[3]= 105;  // выполнена команда 105
           i=6;
           if (Buf_In.buf[1] == 2)
           {
             Buf_In.buf[4] = 0;     // HART 6 Message 0
             Buf_Out.buf[7] = Burst_Message[0].Command_Number;
           }
           else
             Buf_Out.buf[7] = 31;
           if (Buf_In.buf[4] > 2)   // Burst Message №
           {
             Buf_Out.buf[4] = 9;     // Invalid Burst Message
           }
           else
           {
             ptr = (char*)&Burst_Message[Buf_In.buf[4]].Burst_Mode_Control_Code;
             Buf_Out.buf[i++] = *ptr++;
             i++;
             for (;i<16;i++)                      // slots
               Buf_Out.buf[i] = *ptr++;
             Buf_Out.buf[i++] = Buf_In.buf[4];   // Burst Message
             Buf_Out.buf[i++] = 3;               // Maximum number of Burst Messages
             Buf_Out.buf[i++] = 0;               // Extended
             Buf_Out.buf[i++] = *ptr++;          // Command Number
             //Update Times
             Time_Tmp_Buf1 = (unsigned long int) (*((unsigned long int*)ptr));
             ptr += 4;
             Time_Tmp_Buf2 = (unsigned long int) (*((unsigned long int*)ptr));
             ptr += 4;
             ptrURX_Tx = (BYTE*) &Time_Tmp_Buf1+3;
             for (;i<24;i++)
               Buf_Out.buf[i] = *ptrURX_Tx--;
             ptrURX_Tx = (BYTE*) &Time_Tmp_Buf2+3;
             for (;i<28;i++)
               Buf_Out.buf[i] = *ptrURX_Tx--;

             ptr++;
             for (;i<31;i++)                    // Trigger
               Buf_Out.buf[i] = *ptr++;
             ptr +=3;
             for (;i<35;i++)                    // Trigger Value
               Buf_Out.buf[i] = *ptr--;
           }
           Buf_Out.buf[1] = i-2;  //запуск передачи
           break;
   case 107: //Write Burst Device Variables
        //коды ответа:
        //  2 - Invalid Selection
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        //  7 - In Write Protect Mode
        //  8 - warning - Burst Condition Conflict
        //  9 - Invalid Burst Message
        Old_HART = 0;
           Buf_Out.buf[3]= 107;  // выполнена команда 107
           i=6;
           if (HART_write_protect_value)
            Buf_Out.buf[4] = 7;     // In Write Protect Mode
           else
           {
           if (Buf_In.buf[1] < 2 + 9)
            {
             if ((Buf_In.buf[1] > 2 + 4) || (Buf_In.buf[1] < 2 + 1) )
               Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
             else
              {                          // HART 6 совместимость
		Old_HART++;
                j = 2 + Buf_In.buf[1];
                while (j < 4 + 8)
                 Buf_In.buf[j++] = 250;
                Buf_In.buf[j] = 0;        // Burst Message № =0
              }
            }    // OK
           if  (!(Buf_Out.buf[4]))
            {
             if (Buf_In.buf[12] > 2)   // Burst Message №
              {
               Buf_Out.buf[4] = 9;     // Invalid Burst Message
              }
             else
             {
               if (Buf_In.buf[4]==250)  // первый элемент
                 Buf_Out.buf[4] =  2;    // Invalid Selection
               else
               for (j=4 ; j<12; j++)
                {
                  if (!(((Buf_In.buf[j]>243) && (Buf_In.buf[j] < 251)) /* ||  (Buf_In.buf[j]==250)*/ ||  (Buf_In.buf[j] == 0) ||  (Buf_In.buf[j] == 1)))
 //                 if (!(((Buf_In.buf[j]>243) && (Buf_In.buf[j]<251/*248*/))/* ||  (Buf_In.buf[j]==250)*/ ||  (Buf_In.buf[j]<4) ))//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                   {
                     Buf_Out.buf[4] =  2;    // Invalid Selection
                     break;
                   }
                }
                if  (!(Buf_Out.buf[4]))  // нормальная команда - ошибок нет
                 {
                   j=8;
                   Burst_Message_Number = Buf_In.buf[12];
                   ptrMessage = (char*)&Burst_Message[Burst_Message_Number].Burst_Mode_Control_Code;


                   Device_Variable_Code = Buf_In.buf[4];  // триггер настраивается на переменную из слота 0
                   if (Device_Variable_Code >243)
                     Device_Variable_Code -= 244;
		    else
		     Device_Variable_Code += 2;
                   j=8;
                   if (*(ptrMessage + (&Burst_Message[0].Trigger_Mode - &Burst_Message[0].Burst_Mode_Control_Code)))  // если активен триггер
                    if (Device_Variable[Device_Variable_Code].Device_Variable_Classification != *(ptrMessage + (&Burst_Message[0].Device_Variable_Classification - &Burst_Message[0].Burst_Mode_Control_Code)) )
                      {
                 /*       If the trigger mode is non-zero in Command 104 and the trigger source's Device Variable Classification
                        does not match the new Slot 0 Device Variable the new values will be accepted and Response Code "Burst Condition Conflict" will be returned.
                        The field device must correct the classification, unit codes, reset to Trigger Mode 0 and publish continuously at the Update Period until it receives another Command 104.
                 */
			if (!(Old_HART))
                         Buf_Out.buf[4] =  8 ; //   warning - Burst Condition Conflict
                        ptr = ptrMessage + (&Burst_Message[0].Command_Number - &Burst_Message[0].Burst_Mode_Control_Code);

                        for (j=12 ; j<25; j++)                                  // корректировать burst_message_data
                         {
                           Buf_In.buf[j] = *ptr++;  //  переложили в буфер приема все, кроме   Trigger_Value
                         }
                //        j-=4;                       // j = 25
                    //    Bus_Control_Flags |= FHART_Trigger_Reconfig;
                        Buf_In.buf[22] = 0;                //   Trigger_Mode = 0
                        Buf_In.buf[23] = Device_Variable[Device_Variable_Code].Device_Variable_Classification;    // новая переменная
                        if (Device_Variable_Code != 2)                                                          // новый код единиц поставить
                         Buf_In.buf[24] = Device_Variable[Device_Variable_Code].Device_Variable_Units;
                        else
                         Buf_In.buf[24] = Actual.P_Unit;
			j=21;
                      }
                   ptr = ptrMessage + (&Burst_Message[0].Slot [0] - &Burst_Message[0].Burst_Mode_Control_Code);
                   if ( write_data2flash( (unsigned long int)(ptr), (char *) &Buf_In.buf[4],  (int)j )) //запись всего, кроме Burst_Mode_Control_Code и Trigger_Value
                    {
                      Error_Code = Write2Flash_Error;
                      Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                    }
    //                else
                }
                      for (;i<14;i++)
                       Buf_Out.buf[i] = *ptr++;
                      Buf_Out.buf[i++] = Burst_Message_Number;

             }
            }
           }
	 /*
            for (;i<14;i++)
             Buf_Out.buf[i] = *ptr++;
            Buf_Out.buf[i++] = Burst_Message_Number;
         */

            Buf_Out.buf[1] = i-2;  //запуск передачи

	   if (i ==  15)
             SPI_Command = 67;
	    else
	      SPI_Command = 0;
         break;
   case 108: //Write Burst Mode Command Number
        //коды ответа:
        //  2 - Invalid Selection
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        //  7 - In Write Protect Mode
        //  8 - warning - Burst Condition Conflict
        //  9 - Invalid Burst Message
         Old_HART = 0;
         Buf_Out.buf[3]= 108;  // выполнена команда 108
         i=6;
         if (HART_write_protect_value)
           Buf_Out.buf[4] = 7;     // In Write Protect Mode
         else
         {
           if (Buf_In.buf[1] == 2 )
           {
             Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
           }
           else
           {
             if (Buf_In.buf[1] == 2 + 1)  // совместимость с HART 6
             {
               Old_HART++;
               Buf_In.buf[5] = Buf_In.buf[4];
               Buf_In.buf[4] = 0;
               Buf_In.buf[6] = 0;
             }
             if (!((((Buf_In.buf[5]>0) && (Buf_In.buf[5]<4)) || (Buf_In.buf[5]==9) || (Buf_In.buf[5]==48)) && (Buf_In.buf[4]==0)))
             {
               Buf_Out.buf[4] =  2;    // Invalid Selection
             }
             else
             {
               if (Buf_In.buf[6] > 2)   // Burst Message №
               {
                 Buf_Out.buf[4] = 9;     // Invalid Burst Message
               }
               else
               {
                 ptrMessage = (char*)&Burst_Message[Buf_In.buf[6]].Burst_Mode_Control_Code;
                 switch(Buf_In.buf[5])
                 {
                 case 1:
                   Device_Variable_Code = 246-244;
                   break;
                 case 2:
                   Device_Variable_Code = 244-244;
                   break;
                 case 3:
                   Device_Variable_Code = 246-244;
                   break;
                 case 9:
                   Device_Variable_Code = *(ptrMessage + (&Burst_Message[0].Slot [0] - &Burst_Message[0].Burst_Mode_Control_Code));
                   if (Device_Variable_Code >243)
                     Device_Variable_Code -= 244;
                   else
                     Device_Variable_Code += 2;
                   break;
                 case 48:
                   if ((*(ptrMessage + (&Burst_Message[0].Trigger_Mode - &Burst_Message[0].Burst_Mode_Control_Code)) != 4) && (*(ptrMessage + (&Burst_Message[0].Trigger_Mode - &Burst_Message[0].Burst_Mode_Control_Code)) != 0)) // Burst Trigger Mode Selection Code
                   {
                     if (!(Old_HART))
                       Buf_Out.buf[4] = 8;   // warning - Burst Condition Conflict
                   }
                   break;
                 default:
                   break;
                 }
                 j=1;
                 Burst_Message_Number = Buf_In.buf[6];
                 if (*(ptrMessage + (&Burst_Message[0].Trigger_Mode - &Burst_Message[0].Burst_Mode_Control_Code)))
                   if ((Device_Variable[Device_Variable_Code].Device_Variable_Classification != *(ptrMessage + (&Burst_Message[0].Device_Variable_Classification - &Burst_Message[0].Burst_Mode_Control_Code)) )|| (Buf_Out.buf[4] == 8))
                   {
                     if (!(Old_HART))
                       Buf_Out.buf[4] =  8 ; //   warning - Burst Condition Conflict
                     ptr = ptrMessage + ((unsigned char const*)&Burst_Message[0].Update_Time - &Burst_Message[0].Burst_Mode_Control_Code);

                     for (j=6 ; j<22; j++)
                     {
                       Buf_In.buf[j] = *ptr++;
                     }
                     j=17;
                     Buf_In.buf[15] = 0;
                     if (Buf_In.buf[5]!=48)
                     {
                       Buf_In.buf[16] = Device_Variable[Device_Variable_Code].Device_Variable_Classification;           // корректировать уставки триггера
                       if (Device_Variable_Code != 2)
                         Buf_In.buf[17] = Device_Variable[Device_Variable_Code].Device_Variable_Units;
                       else
                         Buf_In.buf[17] = Actual.P_Unit;
                     }
                   }
                 ptr = ptrMessage + (&Burst_Message[0].Command_Number - &Burst_Message[0].Burst_Mode_Control_Code);

                 if ( write_data2flash( (unsigned long int)(ptr), (char *) &Buf_In.buf[5],(int)j) )
                 {
                   Error_Code = Write2Flash_Error;
                   Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
                 }
                 else
                 {
                   if (Buf_In.buf[1] == 2 + 1)   // HART 6 совместимость
                     Buf_Out.buf[i++] = Burst_Message[0].Command_Number;
                   else
                   {
                     Buf_Out.buf[i++] = 0;
                     Buf_Out.buf[i++] = *ptr;
                     Buf_Out.buf[i++] = Burst_Message_Number;
                   }
                 }
               }
             }
           }
         }
         Buf_Out.buf[1] = i-2;  //запуск передачи
         SPI_Command = 68;
         break;
   case 109: //Burst Mode Control
        //коды ответа:
        //  2 - Invalid Selection
        //  5 - Too Few Data Bytes Received
        //  6 - Device-Specific Command Error
        //  7 - In Write Protect Mode
        //  8 - warning - Update Period Increased
        //  9 - Insufficient bandwidth
        // 16 - Access Restricted
        // 32 - Busy (A DR Could Not Be Started
        // 33 - DR Initiated
        // 34 - DR Running
        // 35 - DR Dead
        // 36 - DR Conflict
           Buf_Out.buf[3]= 109;  // выполнена команда 109
          i=6;
        if (HART_write_protect_value)
          Buf_Out.buf[4] = 7;     // In Write Protect Mode
         else
        {
        if (Buf_In.buf[1] == 2)
         {
           Buf_Out.buf[4] = 5;     // Too Few Data Bytes Received
         }
         else
         {
          if (Buf_In.buf[1] == 3)
           Buf_In.buf[5] = 0;    //Burst Message № 0 (HART 6)
          if ((Buf_In.buf[5] > 2) || (Buf_In.buf[4] > 1))   // Burst Message № >2 || Burst Mode Control Code >1
           {
             Buf_Out.buf[4] = 2;  // Invalid Selection
           }
           else
            {  // все ОК
             if ( write_data2flash( (unsigned long int)((char *) &Burst_Message[Buf_In.buf[5]].Burst_Mode_Control_Code), (char *) &Buf_In.buf[4] , 1) )
               {
                 Error_Code = Write2Flash_Error;
                 Buf_Out.buf[4] = 6;  //  Device-Specific Command Error
               }
              else
               { /*
                j = (1 << Buf_In.buf[5]);
                if (Buf_In.buf[4])
                  Burst_Control_Flags |= j;
                 else
                  Burst_Control_Flags &= ~j;
                 */

                 Burst_Control_Flags = (Buf_In.buf[4]<<4) + Buf_In.buf[5];

               }
              Buf_Out.buf[i++] = Burst_Message[Buf_In.buf[5]].Burst_Mode_Control_Code;
            //  if (Buf_In.buf[1] > 3)
                Buf_Out.buf[i++] = Buf_In.buf[5];
              SPI_Command = 69;
             }
          }
        }
          Buf_Out.buf[1] = i-2;  //запуск передачи

         break;
     default:
         Buf_Out.buf[3]= Buf_In.buf[3];   // код команды
         Buf_Out.buf[4] = 64;            //команда не поддерживается
         Buf_Out.buf[1] = 4;              //запуск передачи
       break;
   }
  if (Buf_In.buf[2] == 129)
      {
        Burst_Master_Address =0x80;
        Buf_Out.buf[5] = Device_Status_Byte.Primary_Master | (Flash_Device_Status_Byte.Primary_Master & ~FLoop_Current_Not_Active);
        Device_Status_Byte.Primary_Master &= ~FCold_Start;
      }
     else
      {
        Burst_Master_Address =0;
        Buf_Out.buf[5] = Device_Status_Byte.Secondary_Master | (Flash_Device_Status_Byte.Secondary_Master & ~FLoop_Current_Not_Active);
        Device_Status_Byte.Secondary_Master &= ~FCold_Start;
      }
#ifdef  Debugging
  HART_Out_Message_Counter++;
#endif
  Bus_Control_Flags &= ~FHART_CMD_Active;


