/*******************************************************************************
 * 
 * @file  sub_HTp.c
 *
 * Программа фильтрации данных технологического протокола по прерыванию от USART
 * 
 * Copyright 2010 Titov-MV, Inc.
 *
*******************************************************************************/
  rxdat0 = rxdat1; 
  rxdat1 = tch;
  /* Оброаботка принимаемойй информации с выделением пакета HTp*/
  // установить флаг наличия данных
  if((flagHTechR & RHTpReady)==0)
  if(flagHTechR & RPocketStart) 
   { flagHTechR |= RByteReady;  // Предустановить бит получения байта 
     // Продолжение приема блока
     if((rxdat0 == 0x55) && (tch == 0xAA))
      { // Установить флаг готовности пакета в случае его достоверности
         flagHTechR &= ~RByteReady;
	 if((HTp_CRC1==0))
	  { if(HTp_CRC2 ==0) 
	     {  flagHTechR = RHTpReady;  CountPoket.w ++;		}
	    else
	     { HTp_Error |= ErrorCRC2;   flagHTechR = FindeRPocket;	}
	  }
          else 
	  { HTp_Error |= ErrorCRC2;      flagHTechR = FindeRPocket;	}
 	  rxdat0 = 0x00;  rxdat1 = 0x00;
      }
    else
     if((rxdat0 == 0x55) && (tch == 0x55))
      { // Первые два байта 0xFF и 0x55 приняты, в буфер помещен только 0x55
//        ReceiveBuffer[bufferPtr++] = tch;
	rxdat0 = 0x00; rxdat1 = 0x00;
      }
     else
     if ((rxdat0 != 0x55) && (tch == 0x55))
        flagHTechR &= ~RByteReady; // Прринят первый 0x55, не вносить в буфер
     else
    if((rxdat0 == 0x55) && (tch != 0x55))
      { // Выйти из приема пакета с ошибкой
         flagHTechR = FindeRPocket;
	 HTp_Error  |= Error0x55;
	 rxdat0 = 0x00; rxdat1 = 0x00;
      }
    // Прошли все проверки и есть данные для помещения в буфер 
    if(flagHTechR & RByteReady) 
     {  if(bufferPtr < 5)
          HTp_CRC1 = crc_table[HTp_CRC1^tch];    // HTp_CRC1 += tch;
        else
	 { 
           HTp_CRC2 = crc_table[HTp_CRC2^tch];   //HTp_CRC2 += tch; 
           if((bufferPtr > (ReceiveBuffer[3]+6)))
	    { HTp_Error |= ErrorCRC2;
	      flagHTechR = FindeRPocket;
	    }
	 }
        if(bufferPtr == 0)
        if((tch != AddrHTp) && (tch != AddrHTpDef) && (tch != AddrHTpAll))
	   flagHTechR = FindeRPocket; 
        ReceiveBuffer[bufferPtr++] = tch;
     }
   }
  else
   { if((rxdat0 == 0xFF) && (rxdat1 == 0x55))  // Поиск начала передачи пакета
      { // Установлен признак - начало пакета два байта 0xFF и 0x55 приняты
        flagHTechR = RPocketStart;
	bufferPtr  = 0x00; // Установить указатель на начало буфера приемника
	// Инициализировать все переменные
	HTp_CRC1 = 0; HTp_CRC2 = 0; rxdat0 = 0x00; rxdat1 = 0x00;
      }
   }
