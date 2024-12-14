//file "crc_test.c"

/*! 
 * \brief crc_test - контроль целостности данных в заданных зонах
 *
 *  \par эта подпрограмма обрабатывает текущее слово с помощью аппаратуры CRC
 *   и если достигнут конец блока - сверяет сумму с контрольным значением
 *   и переходит к следующему блоку
 *  
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

if (++CRC_test_pointer > CRC_test_Finish.dw )  // конец зоны контроля?
 {                      
   if (CRCINIRES != Flash_CRC_Record[CRC_test_Record_pointer].CRC_Value)
     Error_Code=CRC_Error_Base+CRC_test_Record_pointer;   // CRC не совпала
   if (++CRC_test_Record_pointer >= sizeof(Flash_CRC_Record)/sizeof(flash_crc_record)) // следующая зона CRC контроля
      CRC_test_Record_pointer=0;
   CRC_test_Start.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[0];
   CRC_test_Start.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[1];
   CRC_test_Start.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[2];
   CRC_test_pointer=CRC_test_Start.dw;                       // подготовка к работе в след. зоне
   CRC_test_Finish.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[0];
   CRC_test_Finish.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[1];
   CRC_test_Finish.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[2];
   CRCINIRES = 0xffff;                           // инициализация аппаратуры CRC
 }

CRCDIRB_L = __data20_read_char (CRC_test_pointer);  // подсчет CRC

