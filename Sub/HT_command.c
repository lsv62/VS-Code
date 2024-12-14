

//file "HT_command.c"

/*!
 * \brief HT_command - обработка технологических команд
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/


// int_char CodeBSL;

// File "HT_command.c"

    addrin  = ReceiveBuffer[0];
    addrout = ReceiveBuffer[1];
    cmd     = ReceiveBuffer[2];
    count   = ReceiveBuffer[3]-1;
  //  datalen = ReceiveBuffer[4];

    ReceiveBuffer[0] = addrout;
    ReceiveBuffer[1] = AddrHTp; //ADC_Flash.AddrHTp;
    ReceiveBuffer[2] = HTp_Error;
  //  ReceiveBuffer[3] = 0;
    HTp_Error = 0;
    flagHTechR &= ~RHTpReady;
   // Indik_Status_Index = 10;
  //  HProtocol_Count=100;
 /*   if (!(ADC_HT_Mode_Count))
    {
      ADC_reset();          // сброс АЦП
      ADC_fast_start();
    }*/
 //   ADC_HT_Mode_Count = 6000;   // перейти в технологич. режим и обновить счетчик паузы НТ режима

     if(addrin == AddrHTpAll)   // Если принят пaкет с данными  широковещательный
      {
        switch(cmd & 0x7F){
         case   12:          //  групповое измерение
               ADC_HT_Mode_Count = 6000;   // перейти в технологич. режим и обновить счетчик паузы НТ режима

                Group_ADC_Pause_Count = 0;
                ADC_Buf_Cnt = ReceiveBuffer[5];    // счетчик байтов
                Reg_ADC[0][0]=ReceiveBuffer[6];  //параметр управления канала0
                Reg_ADC[1][0]=ReceiveBuffer[7];  //параметр управления канала1
                Reg_ADC[2][0]=ReceiveBuffer[8];  //параметр управления канала2
                HT_Debugiing_Column_Row= ReceiveBuffer[9];  //координаты подмены из паспорта
                if(HT_Debugiing_Column_Row!=0xff)
                    HT_Debugiing_Column_Row = (HT_Debugiing_Column_Row & 0x0f)*Pass.T_Nmax + ((HT_Debugiing_Column_Row & 0xf0)>>4);

                ADC_Status &= ~ADC_Buf_Ready;
                ADC_Status |= ADC_Buf_Start;
            break;

           case   17:          // задать код ЦАП
                  DAC_Count =ReceiveBuffer[5];  //счетчик паузы
                  DAC_Code.c[0]=ReceiveBuffer[6];
                  DAC_Code.c[1]=ReceiveBuffer[7];
              //    Device_Status_Byte.Primary_Master |= FLoop_Current_Fixed;
              //    Device_Status_Byte.Secondary_Master |= FLoop_Current_Fixed;
             break;


         default:
            break;
        }
      }
     else
   // Если принят пакет с данными  непосредственно датчику
   // или  принят пакет с данными c настроечным адресом
     if((addrin == AddrHTp/*ADC_Flash.AddrHTp*/) || (addrin == AddrHTpDef))
      { switch(cmd & 0x7F){
          case 0:    //Сбросить датчик (уйти на Reset)
            SPI_Command=1;
            /*
            Start_Ptr =(unsigned int *) 0xfffe;
            ((void(*)())*Start_Ptr)();
            */
            break;
          case 1:   //Рассчитать или получить массив CRC
            if (0==ReceiveBuffer[5])  // запрос к модулю АЦП ?
             {                        // да
               count=8;
               for (i=0;i<4;i++)
                ReceiveBuffer[8+i] = Pass_ID.Tech_Number[i]; // передать номер
               for (CRC_test_Record_pointer=0;CRC_test_Record_pointer<sizeof(Flash_CRC_Record)/sizeof(flash_crc_record); CRC_test_Record_pointer++)
                {
                  ptr = (char*) &Flash_CRC_Record[CRC_test_Record_pointer];   //установить указатель
                  for (i=0;i<8;i++,count++)
                  {
                    ReceiveBuffer[12+(CRC_test_Record_pointer<<3)+i] = *ptr;  // передать массив записей CRC
                    ptr++;
                  }
                }
               if (1 == ReceiveBuffer[6])
                {    // посчитать CRC
                  for (CRC_test_Record_pointer=0;CRC_test_Record_pointer<sizeof(Flash_CRC_Record)/sizeof(flash_crc_record); CRC_test_Record_pointer++)
                  {
                    CRC_test_Start.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[0];
                    CRC_test_Start.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[1];
                    CRC_test_Start.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[2];

                    CRC_test_Finish.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[0];
                    CRC_test_Finish.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[1];
                    CRC_test_Finish.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[2];

                    CRCINIRES = 0xffff;
                    for (CRC_test_pointer=CRC_test_Start.dw; CRC_test_pointer <= CRC_test_Finish.dw; CRC_test_pointer++)
                      {
                         CRCDIRB_L = __data20_read_char (CRC_test_pointer);
                      }
                    ReceiveBuffer[18+(CRC_test_Record_pointer<<3)] = CRCINIRES;   // подменить данные массива рассчитанными
                    ReceiveBuffer[19+(CRC_test_Record_pointer<<3)] = CRCINIRES>>8;;
                  }
                }

                ReceiveBuffer[7] = ReceiveBuffer[6];
                ReceiveBuffer[6] = ReceiveBuffer[5];
                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = count;
		  InitBufUSART_Tx();
             }
             else
              SPI_Command = 3; //запрос к ведомым - передать по SPI
            break;
          case 2:  //Записать блок CRC
             if (0==ReceiveBuffer[5])
             {
               count--;
               ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &Flash_CRC_Record[0]), (char *) &ReceiveBuffer[6], (int) count) ;
                ReceiveBuffer[6] = ReceiveBuffer[5];
                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 4;
		  InitBufUSART_Tx();
             }
             else
              SPI_Command= 2; //запрос к ведомым - передать по SPI
            break;
          case 3:  //Обнулить ID ЦАПа в АЦП
              Float_Buf_Tmp.c[0]=0xff;
              Float_Buf_Tmp.c[1]=0xff;
              Float_Buf_Tmp.c[2]=0xff;
              Float_Buf_Tmp.c[3]=0xff;
              ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Attached_DAC_ID[0]), (char *) &Float_Buf_Tmp.c[0], 4) ;
              ReceiveBuffer[5] = cmd;   //ответ
              ReceiveBuffer[3] = 3;
              InitBufUSART_Tx();
            break;

          case 4:     // поменять НТ адрес
               AddrHTp = ReceiveBuffer[5];
               count=1;
         //       __bic_SR_register(GIE);   //Запретить прерывания
               i = write_data2flash( (unsigned long int)((char *) &ADC_Flash.AddrHTp), (char *) &AddrHTp,(int) count);
         //       __bis_SR_register(GIE);                 //  разрешить прерывания
               ReceiveBuffer[5] = cmd;   // возвращаем код команды
	       ReceiveBuffer[3] = 2;
	       InitBufUSART_Tx();
            break;
         case 5:     // записать данные датчика (HART)
               switch (ReceiveBuffer[5])
                  {
                    case 0:                 // HART комманда 0
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_0_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_0_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
                    case 1:                 //  HART адрес
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &Addr_H_Long[0]), (char *) &ReceiveBuffer[6], 6);
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
                    case 2:                 // сбросить счетчики HARTа
                         ReceiveBuffer[7] =0;
                         SPI_Command= 63;
                      break;
                    case 12:                 // HART комманда 12
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_12_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_12_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
                    case 13:                 // HART комманда 13
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_13_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_13_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
            /*        case 14:                 // HART комманда 14
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_14_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_14_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;   */
                    case 16:                 // HART комманда 16
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_16_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_16_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
                    case 20:                 // HART комманда 20
                         ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &HART_Cmd_20_Data[0]), (char *) &ReceiveBuffer[6], sizeof(HART_Cmd_20_Data));
                         if (ReceiveBuffer[7])
                            Error_Code = Write2Flash_Error;
                      break;
                    default:   break;
                  }
               ReceiveBuffer[6] =  ReceiveBuffer[5];   // код операции
               ReceiveBuffer[5] = cmd;   // возвращаем код команды
	       ReceiveBuffer[3] = 4;
	       InitBufUSART_Tx();
            break;
        case 6:     // прочитать данные датчика (HART)
            { switch (ReceiveBuffer[5])
                  {
                    case 0:                 // HART комманда 0
                      ptr = (char*) &HART_Cmd_0_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_0_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;
                    case 1:                 //  HART адрес
                      ptr = (char*) &Addr_H_Long[0];   //установить указатель
                      for (i=7;i<7+6;i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }

                      break;
                    case 2:                 //прочитать счетчики HARTа
                         SPI_Command= 62;
                        // i=7;
                      break;

                  case 12:                 // HART комманда 12
                      ptr = (char*) &HART_Cmd_12_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_12_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;
                    case 13:                 // HART комманда 13
                      ptr = (char*) &HART_Cmd_13_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_13_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;
          /*          case 14:                 // HART комманда 14
                      ptr = (char*) &HART_Cmd_14_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_14_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;   */
                    case 16:                 // HART комманда 16
                      ptr = (char*) &HART_Cmd_16_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_16_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;
                    case 20:                 // HART комманда 20
                      ptr = (char*) &HART_Cmd_20_Data[0];   //установить указатель
                      for (i=7;i<7 + sizeof(HART_Cmd_20_Data);i++)
                       {
                        ReceiveBuffer[i] = *ptr;
                        ptr++;
                       }
                      break;
                    default:
                        i=7;
                      break;
                  }
               if (ReceiveBuffer[5] != 2)
                {
                 ReceiveBuffer[6] = ReceiveBuffer[5]; // код операции
                 ReceiveBuffer[5] = cmd;   // возвращаем код команды
	         ReceiveBuffer[3] = i-4;
	         InitBufUSART_Tx();
                }
             }
            break;

        case 7:     // записать / прочитать регистр управления
               // запись в ОЗУ? или флеш?

               if(!(ReceiveBuffer[5]))
                {   // прочитать
                  ReceiveBuffer[6] = Actual.Math_Mode;
                  ReceiveBuffer[7] = Mode_Setting_Reg;
                }
                else
                {
                  Actual.Math_Mode = ReceiveBuffer[6];
                  if (write_data2flash( (unsigned long int)((char *) &Mode_Setting_Reg), (char *) &ReceiveBuffer[7],1))
                    Error_Code = Write2Flash_Error;
                  ReceiveBuffer[6] = Actual.Math_Mode;
                  ReceiveBuffer[7] = Mode_Setting_Reg;
                  Actual_2_User();
                  #include "Iout_regim_set.c"

                Calculate_Span();
                Calculate_S0();

                SPI_Command = 184;
                }

            //   ReceiveBuffer[6] = ReceiveBuffer[5];   // возвращаем управляющий байт
               ReceiveBuffer[5] = cmd;   // возвращаем код команды
	       ReceiveBuffer[3] = 4;
	       InitBufUSART_Tx();
            break;
         case 8:
          /* // запереть/открыть BSL
               CodeBSL.c[0] = ReceiveBuffer[5]; //55 для запрета
               CodeBSL.c[1] = ReceiveBuffer[6]; //AA для запрета

               • 0x0000: The flash memory is not erased if an incorrect BSL password has been received by the target.
               • 0xAA55:
               адрес 0xFF7E для MSP430F5438

               count=2;
         //       __bic_SR_register(GIE);   //Запретить прерывания
             i = write_data2flash( (unsigned long int)((char *) (0xFF7E)), (char *) &CodeBSL,(int) count);
           //       __bis_SR_register(GIE);                 //  разрешить прерывания


         //      unsigned int Int_Vect_Addr;

        //       for (Int_Vect_Addr = 0xff00; Int_Vect_Addr < 0xffE2; Int_Vect_Addr+=2) //от FF00 до FFDD - не заперлось:(
        //           ReceiveBuffer[3] = write_data2flash( (unsigned long int)((char *) (Int_Vect_Addr)), (char *) &CodeBSL,(int) count);

               ReceiveBuffer[5] = cmd;   // возвращаем код команды
	       ReceiveBuffer[3] = 2;
	       InitBufUSART_Tx();*/
            break;



          case 9:     // записать настройку канала
                 if (ReceiveBuffer[5] == 0x63) // 99 = код АЦП соответствует?
                  {                                   //  lf
                    count=11;
                  //   __bic_SR_register(GIE);   //Запретить прерывания
                  // при коэффициенте > 2 включается буферный усилитель
                    if ((ReceiveBuffer[6]&0x07) > 1)
                         ReceiveBuffer[7] |= BUF;
                   //   else
                    //     ReceiveBuffer[7] &= ~BUF;
                    if ((ReceiveBuffer[9]&0x07) > 1 )
                         ReceiveBuffer[10] |= BUF;
                    //  else
                    //     ReceiveBuffer[10] &= ~BUF;
                    if ((ReceiveBuffer[12]&0x07) > 1)
                         ReceiveBuffer[13] |= BUF;
                    //  else
                    //     ReceiveBuffer[13] &= ~BUF;
                    if (ReceiveBuffer[8] == F62)   // если частота 62Гц, есть ограничение на допустимое значение
                        ReceiveBuffer[15] = ((ReceiveBuffer[15]>>2)<<2) + 3; // количества точек измерения давления
                    i = write_data2flash( (unsigned long int)((char *) &),(char *) &ReceiveBuffer[6],(int)( count)) ;
                  //   __bis_SR_register(GIE);                 //  разрешить прерывания
                     i= ReceiveBuffer[7] & 0x03;        // номер канала
                     Reg_ADC[i][0] = 1;
                     Reg_ADC[i][1] = ReceiveBuffer[6];   // ConR1
                     Reg_ADC[i][2] = ReceiveBuffer[7];   // ConR0
                     Reg_ADC[i][3] = ReceiveBuffer[8];   // MR1
                      i= ReceiveBuffer[10] & 0x03;        // номер канала
                     Reg_ADC[i][0] = 1;
                     Reg_ADC[i][1] = ReceiveBuffer[9];   // ConR1
                     Reg_ADC[i][2] = ReceiveBuffer[10];   // ConR0
                     Reg_ADC[i][3] = ReceiveBuffer[11];   // MR1
                      i= ReceiveBuffer[13] & 0x03;        // номер канала
                     Reg_ADC[i][0] = 1;
                     Reg_ADC[i][1] = ReceiveBuffer[12];   // ConR1
                     Reg_ADC[i][2] = ReceiveBuffer[13];   // ConR0
                     Reg_ADC[i][3] = ReceiveBuffer[14];   // MR1
                     Number_P_Points = ReceiveBuffer[15] + 2;  //кол-во измерений P + 2 на калибровку
                     Number_T_Points = ReceiveBuffer[16] + 2;  //кол-во измерений Т + 2 на калибровку

                  }
                  else
                     ReceiveBuffer[2] |= Invalid_Data;  // 0x02? - недопустимый параметр команды
                 ReceiveBuffer[5] = cmd;
	         ReceiveBuffer[3] = 2;
		 InitBufUSART_Tx();

            break;
          case   10:          // прочитать параметры АЦП
                 ReceiveBuffer[5] = cmd;   // код команды
                 ReceiveBuffer[6] = 0x63; // 99
                              // нулевой канал АЦП
                 ReceiveBuffer[7] = Reg_ADC[0][1];   // ConR1
                 ReceiveBuffer[8] = Reg_ADC[0][2];   // ConR0
                 ReceiveBuffer[9] = Reg_ADC[0][3];   // MR1
                              // первый канал АЦП
                 ReceiveBuffer[10] = Reg_ADC[1][1];   // ConR1
                 ReceiveBuffer[11] = Reg_ADC[1][2];   // ConR0
                 ReceiveBuffer[12] = Reg_ADC[1][3];   // MR1
                              // второй канал АЦП
                 ReceiveBuffer[13] = Reg_ADC[2][1];   // ConR1
                 ReceiveBuffer[14] = Reg_ADC[2][2];   // ConR0
                 ReceiveBuffer[15] = Reg_ADC[2][3];   // MR1

                 ReceiveBuffer[16] = Number_P_Points - 2;  //кол-во измерений P
                 ReceiveBuffer[17] = Number_T_Points - 2;  //кол-во измерений T

		 ReceiveBuffer[3] = 14;
		 InitBufUSART_Tx();
            break;
          case   11:          // прочитать данные канала АЦП
                ADC_HT_Mode_Count = 6000;   // перейти в технологич. режим и обновить счетчик паузы НТ режима
                i= ReceiveBuffer[5] & 0x03;        // номер канала
                ReceiveBuffer[6] = i;   // номер канала - вернуть
                ReceiveBuffer[7] = Data_ADC[i][0];   // младший байт
                ReceiveBuffer[8] = Data_ADC[i][1];   // данные АЦП
                ReceiveBuffer[9] = Data_ADC[i][2];   // старший байт*

                 ReceiveBuffer[5] = cmd;      // код команды
		 ReceiveBuffer[3] = 6;
		 InitBufUSART_Tx();
            break;
          case   12:          //  групповое измерение
                ADC_HT_Mode_Count = 6000;   // перейти в технологич. режим и обновить счетчик паузы НТ режима
                Group_ADC_Pause_Count = 0;
                ADC_Buf_Cnt = ReceiveBuffer[5];    // счетчик байтов
                Reg_ADC[0][0]=ReceiveBuffer[6];  //параметр управления канала0
                Reg_ADC[1][0]=ReceiveBuffer[7];  //параметр управления канала1
                Reg_ADC[2][0]=ReceiveBuffer[8];  //параметр управления канала2
                HT_Debugiing_Column_Row= ReceiveBuffer[9];  //координаты подмены из паспорта
                if(HT_Debugiing_Column_Row!=0xff)
                    HT_Debugiing_Column_Row = (HT_Debugiing_Column_Row & 0x0f)*Pass.T_Nmax + ((HT_Debugiing_Column_Row & 0xf0)>>4);
                ADC_Status &= ~ADC_Buf_Ready;
                ADC_Status |= ADC_Buf_Start;

            break;
          case   13:          //передать результат группового измерения
                ReceiveBuffer[6] = ReceiveBuffer[5];        // номер канала
                if  ( ReceiveBuffer[5] == 3 ) // передавать все три канала?
                 {
                  for (ReceiveBuffer[5]=0,i=0; ReceiveBuffer[5]<3; ReceiveBuffer[5]++)
                    for (k=0; k < ADC_Buf_Cnt; k++)
                      for (j=0; j<3; j++,i++)
                        ReceiveBuffer[7+i] = Data_Buf_ADC[ReceiveBuffer[5]][j][k];
                 }
                else
                 {     // запрос одного канала
                  for (k=0,i=0; k < ADC_Buf_Cnt; k++)
                    for (j=0; j<3; j++,i++)
                      ReceiveBuffer[7+i] = Data_Buf_ADC[ReceiveBuffer[5]][j][k];
                 }
                ReceiveBuffer[3] = 3+i;          // счетчик
                ReceiveBuffer[5] = cmd;           // код команды
                InitBufUSART_Tx();
            break;
          case   14:          //передать результат группового измерения по индексу
                    ReceiveBuffer[6] = ReceiveBuffer[5];       // индекс
                    ReceiveBuffer[7] = Group_ADC_Pause_Count>>3;  //счетчик паузы
                    for (i=0,k=0; i<3; i++)                  // выборка
                     for (j=0; j<3; j++,k++)                 // измерений
                       ReceiveBuffer[8+k] = Data_Buf_ADC[i][j][ReceiveBuffer[5]];
                    ReceiveBuffer[5] = cmd;           // код команды
		    ReceiveBuffer[3] = 4+k;           // счетчик
		    InitBufUSART_Tx();
            break;
          case   15:          // cчитать код ЦАП, давление и коды давления и температуры
                    Float_Buf_Tmp.f = P_Out;

                    ReceiveBuffer[5] = cmd;
                    ReceiveBuffer[6] =   DAC_Out;
                    ReceiveBuffer[7] =   >>8;
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[10] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[11] = Float_Buf_Tmp.c[3];
                    ReceiveBuffer[12] =  P_int.c[0];
                    ReceiveBuffer[13] =  P_int.c[1];
                    ReceiveBuffer[14] =  P_int.c[2];
                    ReceiveBuffer[15] = T_int.c[0];
                    ReceiveBuffer[16] = T_int.c[1];
                    ReceiveBuffer[17] = T_int.c[2];
                     ReceiveBuffer[18] = DiffTemperatureCodePre.c[0];
                    ReceiveBuffer[19] = DiffTemperatureCodePre.c[1];
                    ReceiveBuffer[20] = DiffTemperatureCodePre.c[2];
                     ReceiveBuffer[21] = DiffTemperatureCodeActual.c[0];
                    ReceiveBuffer[22] = DiffTemperatureCodeActual.c[1];
                    ReceiveBuffer[23] = DiffTemperatureCodeActual.c[2];

// ReceiveBuffer[11] =DAC_Count;
		    ReceiveBuffer[3] = 8+6+6;
		    InitBufUSART_Tx();
              break;


          case   16:          // cчитать код ЦАП
                    Float_Buf_Tmp.f = P_Out;

                    ReceiveBuffer[5] = cmd;
                    ReceiveBuffer[6] =  DAC_Out;
                    ReceiveBuffer[7] =  DAC_Out>>8;
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[10] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[11] = Float_Buf_Tmp.c[3];
//ReceiveBuffer[11] =DAC_Count;
		    ReceiveBuffer[3] = 8 ;
		    InitBufUSART_Tx();

            break;
          case   17:          // задать код ЦАП
                  DAC_Count =ReceiveBuffer[5];  //счетчик паузы
                  DAC_Code.c[0]=ReceiveBuffer[6];
                  DAC_Code.c[1]=ReceiveBuffer[7];
            //      Device_Status_Byte.Primary_Master |= FLoop_Current_Fixed;
           //       Device_Status_Byte.Secondary_Master |= FLoop_Current_Fixed;
                  ReceiveBuffer[5] = cmd;
		  ReceiveBuffer[3] = 2;
		  InitBufUSART_Tx();
             break;

           case   18:          // Записать данные ЦАПа включая № блока
                   SPI_Command = 186;
             break;

           case   19:          //Прочитать данные ЦАПа включая № блока
                   SPI_Command = 187;
             break;

           case   20:          // Записать коэффициенты термокомпенсации ЦАПа
                  SPI_Command = 180;
            /*
                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &DAC_Polinom), (char *) &ReceiveBuffer[5],(int) count) ;
                  ReceiveBuffer[5] = cmd;
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             */
              break;
            case   21:          // Прочитать коэффициенты термокомпенсации ЦАПа
                  SPI_Command = 181;
                  /*
                    count = 4+4+4*4*4+4*4*4+1;              //счетчик байтов
                   ptr = (char*) &DAC_Polinom;   //установить указатель
                   for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[5] = cmd;         // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
                  */
               break;

            case   22:          // Прочитать запись журнала

                   ptr = (char*) &Journal[ReceiveBuffer[5]];   //установить указатель
                   for (i=0;i<16;i++)
                   {
                    ReceiveBuffer[7+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[6] = ReceiveBuffer[5]; // номер записи журнала
                  ReceiveBuffer[5] = cmd;         // ответ
		  ReceiveBuffer[3] = 16+2+1;
		  InitBufUSART_Tx();
             break;

           case   23:          // стереть журнал
                     //стирание
                  __disable_interrupt();
                  FCTL3 = FWKEY;                            // Clear Lock bit
                  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
                  __data20_write_char(((unsigned long int) &Journal), 0);   // Dummy write to erase Flash segment
                  __enable_interrupt();
                  FCTL1 = FWKEY;                         // Clear WRT bit
                  FCTL3 = FWKEY + LOCK;                  // Set LOCK bit
                  ReceiveBuffer[5] = cmd;         // ответ
		  ReceiveBuffer[3] = 2;

		  InitBufUSART_Tx();
             break;

           case   25:          // Прочитать  заводскую таблицу
                   count = 33+28;    //счетчик байтов

                   ptr = (char*) &Pass;  //указатель - на паспорт
                   for (i=0;i<count;i++)
                    {
                       ReceiveBuffer[6+i] =*ptr;
                       ptr++;
                    }

                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = 35+28;

		  InitBufUSART_Tx();
              break;

           case   26:          // Записать заводскую таблицу    !!! ее еще создать надо

            //      ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Pass), (char *) &ReceiveBuffer[5], (int) count) ;
                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
              break;

/* */
           case   27:          // Записать коэффициенты полинома заводской
                  count = 3+(3*6)*4+4*4+1;
                //   __bic_SR_register(GIE);   //Запретить прерывания
                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Polinom_Z), (char *) &ReceiveBuffer[5], (int) count) ;
                //   __bis_SR_register(GIE);                 //  разрешить прерывания
                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
              break;

           case   28:          // Прочитать коэффициенты полинома заводской
                   count = 3+(3*6)*4+4*4+1;    //счетчик байтов
                   ptr = (char*) &Polinom_Z;  //указатель - на заводской полтном
                   for (i=0;i<count;i++)
                    {
                       ReceiveBuffer[6+i] =*ptr;
                       ptr++;
                    }
                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
              break;

           case   29:                      // Записать флеш АЦП
                  count = 3*3+1+1+1+4+2*2+2*4+8;   //счетчик байтов = sizeof(ADC_Flash)
                  if ((ReceiveBuffer[5]&0x07) > 1)
                         ReceiveBuffer[6] |= BUF;
                   //   else
                   //      ReceiveBuffer[6] &= ~BUF;
                  if ((ReceiveBuffer[8]&0x07) > 1 )
                         ReceiveBuffer[9] |= BUF;
                  //    else
                   //      ReceiveBuffer[9] &= ~BUF;
                  if ((ReceiveBuffer[11]&0x07) > 1)
                         ReceiveBuffer[12] |= BUF;
                  //    else
                  //       ReceiveBuffer[12] &= ~BUF;
                  if (ReceiveBuffer[7] == F62)   // если частота 62Гц, есть ограничение на допустимое значение
                        ReceiveBuffer[14] = ((ReceiveBuffer[14]>>2)<<2) + 3; // количества точек измерения давления
                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &ADC_Flash), (char *) &ReceiveBuffer[5], (int) count) ;
                  if ((ReceiveBuffer[41] == 0xff)&&(ReceiveBuffer[42] == 0xff)&& (!(ReceiveBuffer[6])) && (ADC_Error_Reg != 0xffff))
                    ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &ADC_Error_Reg), (char *) &ReceiveBuffer[41], (int) 2) ;
                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;

           case   30:                        // Прочитать флеш АЦП
                  count =  3*3+1+1+1+4+2*2+2*4+8;         //счетчик байтов = sizeof(ADC_Flash)
                  ptr = (char*) &ADC_Flash;   //установить указатель
                  for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  Int_Buf_Tmp.w = ADC_Error_Reg;

                  ReceiveBuffer[6+i++] = Int_Buf_Tmp.c[0];
                  ReceiveBuffer[6+i++] = Int_Buf_Tmp.c[1];
                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = count+2+2;
		  InitBufUSART_Tx();
              break;


           case   31:                      // Прочитать - записать номер блока

                if (!(ReceiveBuffer[5] & 0x7f)) // Работа с АЦП ?
                 {
                  if  (ReceiveBuffer[5]) // Запись?
                   {
                     ReceiveBuffer[7] = write_data2flash( (unsigned long int)((char *) &Pass_ID), (char *) &ReceiveBuffer[6], 4) ;
                     if (!(ReceiveBuffer[7]))
                      switch (ReceiveBuffer[10]) // работа с дополнительными командами
                      {
                        case 1:
                            Float_Buf_Tmp.c[0] = 0x00;
                            Float_Buf_Tmp.c[1] = 0x00;
                            Float_Buf_Tmp.c[2] = 0xff;
                            Float_Buf_Tmp.c[3] = 0xff;
                            ReceiveBuffer[7] = write_data2flash((unsigned int)((char *) &Flash_Device_Status_Byte.Primary_Master),((char *) &Float_Buf_Tmp.c[0]), 4) ;
                          break;
                        default:
                             break;
                      }
                      ReceiveBuffer[3] = 4;
                   }
                  else                          // чтение
                   {
                     ReceiveBuffer[13] = ReceiveBuffer[6];
                     ptr = (char*) &Pass_ID;   //установить указатель
                     for (i=0;i<4;i++)
                      {
                        ReceiveBuffer[7+i] = *ptr;
                        ptr++;
                      }
                    ReceiveBuffer[11] = MathVersion;
                    ReceiveBuffer[12] = MathVersion>>8;
                    count = 0;
                 //   if (ReceiveBuffer[13]) // есть продолжение?
                    switch (ReceiveBuffer[13]) // работа с дополнительными командами
                      {
                        case 1:
                            ReceiveBuffer[14] = Flash_Device_Status_Byte.Primary_Master;
                            ReceiveBuffer[15] = Flash_Device_Status_Byte.Secondary_Master;
                            ReceiveBuffer[16] = Flash_Device_Status_Byte.DAC_Code_Fixed;
                            ReceiveBuffer[17] = Flash_Device_Status_Byte.DAC_Code_Fixed>>8;
                            count = 4;
                          break;
                        default:
                             break;
                      }
		    ReceiveBuffer[3] = 3+4+2+1+count;
                   }
                  ReceiveBuffer[6] = ReceiveBuffer[5];
                  ReceiveBuffer[5] = cmd;   //ответ

		  InitBufUSART_Tx();
                 }
                 else
                  SPI_Command = 9; //запрос к ведомым - передать по SPI

             break;




           case   32:                      // Записать ID датчика
                  count = 4+12+10+1+1+1+8;
                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Pass_ID), (char *) &ReceiveBuffer[5], (int) count) ;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;

           case   33:                        // Прочитать ID датчика
                   count = 4+12+10+1+1+1+8;          //счетчик байтов
                   ptr = (char*) &Pass_ID;   //установить указатель
                   for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
              break;


          /*

           case   34:          // Записать  константы режима модуля ЦАП
                  SPI_Command = 186;


                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 2;
		  InitBufUSART_Tx();

             break;

           case   35:          // Получить константы режима модуля ЦАП
                  SPI_Command = 187;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 6;
		  InitBufUSART_Tx();

             break;
               */
           case   36:      //  Установить текущий  Pmin
                  Actual.Pmin = P_Out;
              /*   if (Actual.Math_Mode & FTransfer_Lin_Drop)
                   {       // обратная х-ка
                     Pup = Actual.Pmin;
                   }
                  else
                    {       // прямая или корнеизвлекающая характеристика
                      Pdown = Actual.Pmin;
                    }   */
                 #include "Pup_Pdown_select.c"
                 Calculate_Span();
                 Calculate_S0();
                 Float_Buf_Tmp.f  = Actual.Pmin;
                 ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                 ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                 ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                 ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
                 ReceiveBuffer[5] = cmd;         // ответ
	         ReceiveBuffer[3] = 6;
                 SPI_Command = 5; // команда - передать статус датчика
	         InitBufUSART_Tx();
               break;

           case   37:      //   Установить текущий  Pmax
                    Actual.Pmax = P_Out;
                  /*   if (Actual.Math_Mode & FTransfer_Lin_Drop)
                      {       // обратная х-ка
                        Pdown = Actual.Pmax;
                      }
                       else
                       {       // прямая или корнеизвлекающая характеристика
                         Pup = Actual.Pmax;
                       }*/
                    #include "Pup_Pdown_select.c"
                    Calculate_Span();
                    Calculate_S0() ;

                    Float_Buf_Tmp.f  = Actual.Pmax;
                    ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
                    ReceiveBuffer[5] = cmd;         // ответ
	            ReceiveBuffer[3] = 6;
                    SPI_Command = 5; // команда - передать статус датчика
	            InitBufUSART_Tx();
              break;

           case   38:                      // Записать набор из 4 профайлов
                  count = 4*(4+4+2);

                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Profile), (char *) &ReceiveBuffer[5], (int) count) ;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;
           case   39:                        // Прочитать набор из 4 профайлов
                   count = 4*(4+4+2);          //счетчик байтов
                   ptr = (char*) &Profile;   //установить указатель
                   for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
              break;


           case   40:                        // Записать заголовок паспорта датчика
                  count = 4*1+4*4+(4*2)*4+1 ;    //счетчик байтов  //62
                //   __bic_SR_register(GIE);   //Запретить прерывания
                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Pass), (char *) &ReceiveBuffer[5],(int) count) ;
                //   __bis_SR_register(GIE);                 //  разрешить прерывания
                  ReceiveBuffer[5] = cmd;    //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
                  Plant_2_User();
                  RAM_Trim_Data.Lower_Trim_Point_Value = Pass.Pmin_Lim;
                  RAM_Trim_Data.PpEt_for_Lower_Trim_Point_Value = Pass.Pmin_Lim;
                  RAM_Trim_Data.Upper_Trim_Point_Value = Pass.Pmax_Lim;
                  RAM_Trim_Data.PpEt_for_Upper_Trim_Point_Value = Pass.Pmax_Lim;
                  write_data2flash( (unsigned long int)((char *) &Flash_Trim_Data), (char *) &RAM_Trim_Data,(int) 16) ;

              break;
           case   41:                       // Прочитать заголовок паспорта датчика
                   count = 4*1+4*4+(4*2)*4+1;    //счетчик байтов  //62

                   ptr = (char*) &Pass;     //указатель - на паспорт
                   for (i=6;i<count+6;i++)
                    {
                       ReceiveBuffer[i] =*ptr;
                       ptr++;
                    }
                  ReceiveBuffer[i] = VersHTProt;  // версия протокола
                  ReceiveBuffer[5] = cmd;    // ответ
		  ReceiveBuffer[3] = count+2+1;

		  InitBufUSART_Tx();
             break;
           case   42:                        // Записать осевые подписи-массивы
                  if (count > 10*4+16*4+1)
                    count = 10*4+16*4+1;
              //     __bic_SR_register(GIE);   //Запретить прерывания
                 ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Axial), (char *) &ReceiveBuffer[5],(int) count) ;
             //       __bis_SR_register(GIE);  //  разрешить прерывания
                  ReceiveBuffer[5] = cmd;    //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;
           case   43:                       // Прочитать осевые подписи-массивы
                   count = 10*4+16*4+1;     //счетчик байтов
                   ptr = (char*) &Axial;    //установить указатель
                   for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[5] = cmd;   // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
              break;
           case   44:                       // Записать коэффициенты полинома
                 if (count > 8+(7*7)*4+4*4+1)
                    count = 8+(7*7)*4+4*4+1 ; // sizeof(Polinom);

                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &Polinom), (char *) &ReceiveBuffer[5],(int) count) ;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;
           case   45:                       // Прочитать коэффициенты полинома
                   count = 8+(7*7)*4+4*4+1;              //счетчик байтов
                   ptr = (char*) &Polinom;  //установить указатель
                   for (i=0;i<count;i++)
                   {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                   }
                  ReceiveBuffer[5] = cmd;   // ответ
		  ReceiveBuffer[3] = count+2;
		  InitBufUSART_Tx();
              break;

           case   46:                            // Запись таблицы прогона

               //   __bic_SR_register(GIE);            //Запретить прерывания
                if (ReceiveBuffer[5] + ReceiveBuffer[6] <= 160)
                  {
                    count = 16 * ReceiveBuffer[6];
                     ReceiveBuffer[8] = write_data2flash((unsigned long int)((char *) &Point[ReceiveBuffer[5]]), (char *) &ReceiveBuffer[7],(int) count) ;
               //   __bis_SR_register(GIE);    //  разрешить прерывания
                  }
                  else ReceiveBuffer[8] = 250;
                  ReceiveBuffer[7] = ReceiveBuffer[6];
                  ReceiveBuffer[6] = ReceiveBuffer[5];
                  ReceiveBuffer[5] = cmd;     // ответ
		  ReceiveBuffer[3] = 5;
                  InitBufUSART_Tx();
             break;
           case   47:                       // Чтение таблицы прогона
                ptr = (char*)&Point[ReceiveBuffer[5]];  //установить указатель
                count = ReceiveBuffer[6]*16;            //счетчик точек
                for (i=0; i<count; i++)
                 {
                    ReceiveBuffer[8+i] = *ptr;
                    ptr++;
                 }
                ReceiveBuffer[7] = ReceiveBuffer[6];
                ReceiveBuffer[6] = ReceiveBuffer[5];
                ReceiveBuffer[5] = cmd;     // ответ
		ReceiveBuffer[3] = count+4;
		InitBufUSART_Tx();
              break;

            case   48:                       // Прочитать актуальные настройки датчика
                count = 1+1+4*2+2*4+2+2+3*4;            //счетчик точек
                ptr = (char*)&Actual;  //установить указатель

                for (i=0; i<count; i++)
                 {
                    ReceiveBuffer[6+i] = *ptr;
                    ptr++;
                 }
                ReceiveBuffer[5] = cmd;     // ответ
		ReceiveBuffer[3] =  count+2;
		InitBufUSART_Tx();
              break;

            case   49:                       // Записать пользовательские настройки датчика

                  count = 1+1+4*2+2*4+2+2+3*4;

                  ReceiveBuffer[6] = write_data2flash( (unsigned long int)((char *) &User), (char *) &ReceiveBuffer[5],(int) count) ;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 3;
		  InitBufUSART_Tx();
             break;

           case   50:                       // Установить заводской ноль "0" датчика
                switch (Actual.Math_Type & 0x03)
                  {
                    case 0:                 // без коррекции
                        Actual.Zero_Shift0 = PpT-Pass.Pmin_Lim;
                        Float_Buf_Tmp.f = Actual.Zero_Shift0;
                      break;
                    case 1:                 // polinom
                        Actual.Zero_Shift1 = PpT-Pass.Pmin_Lim;
                        Float_Buf_Tmp.f = Actual.Zero_Shift1;
                      break;
                    case 2:                 //  cubic_spl
                        Actual.Zero_Shift2 = PpT-Pass.Pmin_Lim;
                        Float_Buf_Tmp.f = Actual.Zero_Shift2;
                      break;
                    case 3:                 //  zavod
                        Actual.Zero_Shift3 = PpT-Pass.Pmin_Lim;
                        Float_Buf_Tmp.f = Actual.Zero_Shift3;
                      break;
                    default:   break;
                  }
                    ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
                    ReceiveBuffer[5] = cmd; // ответ
	            ReceiveBuffer[3] = 6;
	            InitBufUSART_Tx();
               break;

           case   51:      //    Установить заводской диапазон "S" датчика
                 Float_Buf_Tmp.f = PpE;
                switch (Actual.Math_Type & 0x03) //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                  {
                    case 0:            //  без коррекции
                       Actual.Span0 = (Pass.Pmax_Lim - Pass.Pmin_Lim  )/(PpT  - Pass.Pmin_Lim /*  - Actual.Zero_Shift0*/); // Пересчет коэффициента
                       Float_Buf_Tmp.f = Actual.Span0;
                      break;
                    case 1:            //  polinom
                       Actual.Span1 = (Pass.Pmax_Lim  -Pass.Pmin_Lim  )/(PpT  - Pass.Pmin_Lim /*  - Actual.Zero_Shift1*/); // Пересчет коэффициента
                       Float_Buf_Tmp.f = Actual.Span1;
                      break;
                    case 2:            //  cubic_spl
                       Actual.Span2 = (Pass.Pmax_Lim  -Pass.Pmin_Lim  )/(PpT  - Pass.Pmin_Lim /* - Actual.Zero_Shift2*/); // Пересчет коэффициента
                       Float_Buf_Tmp.f = Actual.Span2;
                      break;
                    case 3:            //  zavod
                       Actual.Span3 = (Pass.Pmax_Lim  -Pass.Pmin_Lim  )/(PpT  - Pass.Pmin_Lim /* - Actual.Zero_Shift3*/); // Пересчет коэффициента давления
                       Float_Buf_Tmp.f = Actual.Span3;
                      break;
                    default:   break;
                  }
                    ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
                    ReceiveBuffer[5] = cmd;         // ответ
	            ReceiveBuffer[3] = 6;
	            InitBufUSART_Tx();
               break;

           case   52:      //  Установить текущий  ноль "0" датчика
                    zero_shift_set() ;
       //             Actual.Zero_Shift = PpEt-Actual.P_Set_Zero_Shift;
  /*
                    Float_Buf_Tmp.f  = Actual.Pmax;
                    ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
                 */
                    ReceiveBuffer[5] = cmd;         // ответ
	            ReceiveBuffer[3] = 2;
	            InitBufUSART_Tx();
               break;
           case   53:      // Установить текущий  диапазон "S" датчика
              /*   Float_Buf_Tmp.f = PpE;
                 Actual.Span = (Actual.Pmax - Actual.Pmin)/(Float_Buf_Tmp.f - Actual.Pmin - Actual.Zero_Shift);
                    Float_Buf_Tmp.f = Actual.Pmax; //???  Actual.Span ?
                    ReceiveBuffer[6] = Float_Buf_Tmp.c[0];
                    ReceiveBuffer[7] = Float_Buf_Tmp.c[1];
                    ReceiveBuffer[8] = Float_Buf_Tmp.c[2];
                    ReceiveBuffer[9] = Float_Buf_Tmp.c[3];
             */
                    ReceiveBuffer[5] = cmd;           // ответ
	            ReceiveBuffer[3] = 2;
	            InitBufUSART_Tx();
               break;
      /*
            case   54:      //  Установить текущий  ток датчика 4 мА
                 ptr = (char *) &Actual.Code_4_mA;    // установить указатель
                 *ptr = ReceiveBuffer[5];
                  ptr++;
                 *ptr = ReceiveBuffer[6];
                 Code_Down = Actual.Code_4_mA ;
                 Calculate_Span();
                 ReceiveBuffer[5] = cmd;              // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
               break;
            case   55:      // Установить текущий  ток датчика 20 мА
                 ptr = (char*) &Actual.Code_20_mA;    //установить указатель
                 *ptr = ReceiveBuffer[5];
                 ptr++;
                 *ptr = ReceiveBuffer[6];
                 Code_Up = Actual.Code_20_mA;
                 Calculate_Span();
                 ReceiveBuffer[5] = cmd;              // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
               break;

       case   56:      //  Установить текущий  ток датчика 0 мА
                ptr = (char*) &Actual.Code_0_mA;      //установить указатель
                *ptr = ReceiveBuffer[5];
                 ptr++;
                *ptr = ReceiveBuffer[6];
                 Code_Down = Actual.Code_0_mA;
                 Calculate_Span();
                 ReceiveBuffer[5] = cmd;              // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
              break;

       case   57:      //  Установить текущий  ток датчика 5 мА
                  ptr = (char*) &Actual.Code_5_mA;   //установить указатель
                 *ptr = ReceiveBuffer[5];
                  ptr++;
                 *ptr = ReceiveBuffer[6];
                 Code_Up = Actual.Code_5_mA;
                 Calculate_Span();
                 ReceiveBuffer[5] = cmd;             // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
              break;
*/
        case   58:          // Получить данные АЦП модуля ЦАП
                  SPI_Command = 189;

             break;

        case   59:          // прочитать режим математики

                  ReceiveBuffer[6] = Actual.Math_Mode ;
                  ReceiveBuffer[7] = Actual.Math_Type ;

                  tmp_Buf.w = Actual.Filter_Limit;
                  ReceiveBuffer[8] = tmp_Buf.c[0] ;
                  ReceiveBuffer[9] = tmp_Buf.c[1] ;

                  ReceiveBuffer[5] = cmd;   //ответ
		  ReceiveBuffer[3] = 6;
		  InitBufUSART_Tx();
             break;

        case   60:      // Установить режим математики   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                 Actual.Math_Mode = ReceiveBuffer[5];
                 Actual.Math_Type = ReceiveBuffer[6]; // режим математики


                  tmp_Buf.c[0] = ReceiveBuffer[7];
                  tmp_Buf.c[1] = ReceiveBuffer[8];
                  if (tmp_Buf.w < Filter_Limit_Max)
                   {
                    if (!(tmp_Buf.w))
                     {
                       dampfer_zero();
                       Actual.Filter_Limit=0;
                     }
                     else
                       Actual.Filter_Limit = tmp_Buf.w;
                   }
                   else
                    Actual.Filter_Limit = Filter_Limit_Max;
                  i=((Actual.Math_Mode & FProfile_Mask)>>4);         // номер профайла
                  Actual.Pmin = Profile[i].Pmin;
                  Actual.Pmax = Profile[i].Pmax;
               /*  if  (Actual.Math_Mode & FTransfer_Lin_Drop )  // обратная х-ка
                   {
                     Pup = Actual.Pmin;
                     Pdown = Actual.Pmax;
                   }
                  else
                   {                     // прямая или корнеизвлекающая х-ки
                     Pup = Actual.Pmax;
                     Pdown = Actual.Pmin;
                   }*/
                 #include "Pup_Pdown_select.c"
               /*  if (Actual.Math_Mode & FSw_5mA_Off)
                  {                       // режим 4..20 ма
                    Code_Up = Actual.DAC_Code_20_mA;
                    Code_Down = Actual.DAC_Code_4_mA;
                  }
                  else
                   {                      // режим 0..5 ма
                     Code_Up = Actual.DAC_Code_5_mA;
                     Code_Down = Actual.DAC_Code_0_mA;
                   }*/
                 #include "Iout_regim_set.c"
                 Calculate_Span();
                 Calculate_S0() ;
                 SPI_Command = 188;
                 ReceiveBuffer[5] = cmd;         // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
               break;

          case   61:     //Временно подменить код температуры и давления
                 PT_Count =  ReceiveBuffer[5];  // счетчик паузы приема из АЦП
                 P_float.c[0] =  ReceiveBuffer[6];
                 P_float.c[1] =  ReceiveBuffer[7];
                 P_float.c[2] =  ReceiveBuffer[8];
                 P_float.c[3] =  ReceiveBuffer[9];
                 T_float.c[0] =  ReceiveBuffer[10];
                 T_float.c[1] =  ReceiveBuffer[11];
                 T_float.c[2] =  ReceiveBuffer[12];
                 T_float.c[3] =  ReceiveBuffer[13];
                 ReceiveBuffer[5] = cmd;         // ответ
	         ReceiveBuffer[3] = 2;
	         InitBufUSART_Tx();
              break;
          case   62:    // Оценить качество передачи по SPI
                 ReceiveBuffer[3] = 2; //возвращается - если команда на тестирование или при
                                       // опросе, если не завершено тестирование
                 if (ReceiveBuffer[5] | ReceiveBuffer[6]) //не равно нулю - задать счетчик оценки
                  {
                    SPI_Send_Count.c[0] = ReceiveBuffer[5] ;
                    SPI_Send_Count.c[1] = ReceiveBuffer[6] ;
                    SPI_Send_Count.w += 2;
                     for (i=0; i<8; i++)
                         SPI_Test_Buf[i].w=0 ;
                    Bus_Control_Flags |= SPI_Test;
                  }
                  else
                   if ((Bus_Control_Flags & SPI_Test) && (!(SPI_Send_Count.w)))
                    {                        // режим завершен - можно отдавать данные
                      Bus_Control_Flags &= ~SPI_Test;
                      for (i=0; i<8; i++)
                      {
                        ReceiveBuffer[6+2*i] = SPI_Test_Buf[i].c[0] ;
                        ReceiveBuffer[6+2*i+1] = SPI_Test_Buf[i].c[1] ;
                      }
                      ReceiveBuffer[3] =18;
                    }
                 ReceiveBuffer[5] = cmd;         // ответ
	         InitBufUSART_Tx();
              break;
          case   63:   //Оценить качество передачи по HT
                 if (!( ReceiveBuffer[5] | ReceiveBuffer[6])) // запрос значения счетчика по НТ
                    CountPoket.w=0;  // сброс счетчика
                  ReceiveBuffer[9] =  CountPoket.c[1]; // передать внутрений
                  ReceiveBuffer[8] =  CountPoket.c[0];  // счетчик
                  ReceiveBuffer[7] =  ReceiveBuffer[6]; // вернуть внешний
                  ReceiveBuffer[6] =  ReceiveBuffer[5]; // счетчик
                  ReceiveBuffer[5] = cmd;           // повторить команду
                  ReceiveBuffer[3] = 6;             // счетчик байтов
                  InitBufUSART_Tx();                // передать
              break;
       default:
            break;
      }
      flagHTechR &= ~RHTpReady;
      }
     else flagHTechR = FindeRPocket;
