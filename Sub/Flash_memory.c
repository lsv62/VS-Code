
/*!
 * \brief write_data2flash - запись информации во флеш память
 *
 *  \par  записывает "data_count" байт информации, начинающихся в точке,
 *        указываемой параметром "data" в область, начинающуюся с "addr"
 *
 *
 *  Stack:     не использует \par
 *  Return:    возвращает код завершения операции:
 *             0 - успешное завершение
 *             1 - попытка записи в несуществующую память
 *             2 - попытка записи блока нулевой длины
 *             3 - сверка записи не прошла \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/
#include  "msp430x54xA.h"

//#include  <types_define.h>
#include  <sub_main_data.h>

//char DAt_Test[1500];

unsigned char Flash_Buf[512];

char write_data2flash(unsigned long int addr, char* data, int data_count)
{
 unsigned long int Segment_Start, Next_Segment, start_mem;
 unsigned int Actual_Segment_Size, i, Nseg, Blok_Count;
 unsigned char  flag, Chk_Byte_Flash, Chk_Byte_Data ;
 char* ptr_tmp;
  if (data_count)
  {
              // определение типа памяти для записи
 if ((addr >= Inf_Memory_Down_Limit) && (addr <= Inf_Memory_Up_Limit))
  {           // запись в информац. память
    Actual_Segment_Size = Inf_Segment_Size;  // размер информационного сегмента
    start_mem = Inf_Memory_Down_Limit;     // начальный адрес информационной памяти
  }
 else
    if ((addr >= Main_Memory_Down_Limit) &&  (addr <= Main_Memory_Up_Limit))
      {       // запись в основную память
        Actual_Segment_Size = Main_Segment_Size; // размер основного сегмента
        start_mem = Main_Memory_Down_Limit;   // начальный адрес основной памяти
      }
      else
       return 1;    // error 1 - попытка записи в несуществующую память
  // вычисление адреса сегмента для записи
  Nseg = (unsigned int)((addr - start_mem) / Actual_Segment_Size);  // номер сегмента для начала записи
  Next_Segment = ((unsigned long int)Nseg*Actual_Segment_Size + start_mem);  // стартовый адрес сегмента начала записи
   // запись данных
do
 {
   Segment_Start = Next_Segment ;          // начальный адрес рабочего сегмента
   Next_Segment += Actual_Segment_Size;    // начальный адрес следующего сегмента
   if  ((addr + data_count) > Next_Segment)    //Запись в одном сегменте?
     Blok_Count = (unsigned int)( Next_Segment - addr); // нет, пишем посегментно
    else   Blok_Count = data_count;            // запись в текущий сегмент
   data_count -= Blok_Count;                 //посчитать недописанные данные
   flag=1;                                   // разрешена запись без стирания
   ptr_tmp=data;                            // указатель на данные
     //проверка - нужно ли стирание сегмента?
   for (i=0;i<Blok_Count;i++)
   {
     Chk_Byte_Flash = __data20_read_char (addr+i);      // прочитать флеш
     Chk_Byte_Data = *ptr_tmp++;                //прочитать записываемые данные
     if ( Chk_Byte_Data !=  (Chk_Byte_Flash & Chk_Byte_Data)) // возможна дозапись?
     {           // нет, писать придется весь сегмент
        // копирование флеши в буфер
       for (i=0;i<Actual_Segment_Size;i++)
         Flash_Buf[i] = __data20_read_char (i + Segment_Start);

       // модификация информации в буфере
       for (i=addr-Segment_Start;i<addr - Segment_Start + Blok_Count;i++)
         Flash_Buf[i] = *data++;
          // подготовка к зеписи
      Blok_Count=Actual_Segment_Size; // писать придется весь сегмент
      addr=Segment_Start;             // писать с начала сегмента
       //стирание
      __disable_interrupt();
      FCTL3 = FWKEY;                            // Clear Lock bit
      FCTL1 = FWKEY+ERASE;                      // Set Erase bit
      __data20_write_char( Segment_Start, 0);   // Dummy write to erase Flash segment

      // запись блока данных из буфера
      FCTL1 = FWKEY+WRT;               // разрешить запись
        for (i=0; i<Blok_Count; i++ )
       __data20_write_char( addr+i, Flash_Buf[i] );  // записать буфер
       __enable_interrupt();
        flag=0;                      // запись произведена
        // проверка записи
        for (i=0; i<Blok_Count; i++ )
        {
         Chk_Byte_Flash = __data20_read_char (addr++); // прочитать флеш
         if (Chk_Byte_Flash != Flash_Buf[i])           // сравнить с буфером
          { //несовпало? - прекратить операцию
            FCTL1 = FWKEY;                         // Clear WRT bit
            FCTL3 = FWKEY + LOCK;                  // Set LOCK bit
            return 3 ; // error  3 - сверка записи не прошла
          }
        }
       break; // прекратить проверку - запись уже произведена
     }
   }
   if  (flag)  // Данные не стирались - возможна дозапись?
   { // да
     __disable_interrupt();
    FCTL1 = FWKEY+WRT;                // разрешить запись
    FCTL3 = FWKEY;                    // снять блокировку
    for (i=0; i<Blok_Count; i++ )
    {
      Chk_Byte_Flash = __data20_read_char (addr+i); // прочитать флеш
      if (Chk_Byte_Flash !=*(data+i))       // есть отличие от уже записанного?
        __data20_write_char( addr+i, *(data+i) ); // записать данные
    }
    __enable_interrupt();
     // проверка записи
    for (i=0; i<Blok_Count; i++ )
       {
         Chk_Byte_Flash = __data20_read_char (addr++); // прочитать флеш
         if (Chk_Byte_Flash != *data++)           // сравнить с буфером
         {
           FCTL1 = FWKEY;                         // Clear WRT bit
           FCTL3 = FWKEY + LOCK;                  // Set LOCK bit
           return 3 ; // error  3 - сверка записи не прошла
         }
        }
   }
 }
while (data_count);  // условие выхода - достигнут конец блока данных

  FCTL1 = FWKEY;                         // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                  // Set LOCK bit

  return 0;  // запись произведена без ошибок
  }
  else
    return 2 ;// error 2 - попытка записи блока нулевой длины
}

