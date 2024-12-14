/**********************************************************
 * демпфирование давления                     
 * 
 * 
 *********************************************************/

if (Actual.Filter_Limit)      // демпфер включен?
  {                    // да, 
    if (FilterCount > Actual.Filter_Limit)  // уменьшили уставку демпфера..
     {
       FilterSum = 0;
       FilterCount = 0;                                    // выполнить изменение ..
       FilterPtr =0;
     }
    
    Filter_P[FilterPtr]= PpT;   // текущее значение - в ячейку буфера
    FilterSum += Filter_P[FilterPtr];  // нарастить сумму
    if (++FilterPtr > (Actual.Filter_Limit))    // конец буфера?
           FilterPtr = 0;              // перейти на начало
  /*  if ((FilterCount > Actual.Filter_Limit) && (FilterPtr == (Actual.Filter_Limit-1))) // уменьшили уставку демпфера..
    {
      FilterSum /= FilterCount;
      FilterCount = Actual.Filter_Limit;                                    // выполнить изменение ..
      FilterSum *= FilterCount;
    }*/
    if ((FilterCount < Actual.Filter_Limit) && (FilterPtr > FilterCount)) // буфер не заполнен?
          FilterCount++;                    // да, увеличить счетчик 
      else                                   // нет, уже заполнен -
          FilterSum=FilterSum-Filter_P[FilterPtr];  // отнять наиболее старое значение
    PpE =  FilterSum / FilterCount  ;               // высчитать среднее
  }  
