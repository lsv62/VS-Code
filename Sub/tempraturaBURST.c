 // file tempraturaBURST.c

    
 //   таймер температуры - 4 сек.   может быть сделать исключающее или с флагом через 2сек?
/* */ 
  if (!(nTemperature))
   {
      ptrTemperature_Polinom = &Polinom.A_T[0];
      TemperatureActual =  *ptrTemperature_Polinom++;
   }
  else
     TemperatureActual = TemperatureActual * T_float.f + *ptrTemperature_Polinom++;
  //   TemperatureActual = 20.5*1.01+0.05;
   
     if (++nTemperature >= Polinom.NAt[7])
      {
        nTemperature = 0;
        Temperature = TemperatureActual;
   //     TemperatureActual=0;
      
        if ((Temperature > Pass.Tmax) ||(Temperature < Pass.Tmin )) // или надо текущие?
         {// одновременно устанавливаются флаги Non_Primary_Variable_Out_of_Limits для первого и второго мастера
           Device_Status_Byte.Primary_Master |= FNon_Primary_Variable_Out_of_Limits;        
           Device_Status_Byte.Secondary_Master |= FNon_Primary_Variable_Out_of_Limits;  
         }
        else
         { // одновременно сбрасываются флаги Non_Primary_Variable_Out_of_Limits для первого и второго мастера
           Device_Status_Byte.Primary_Master &= ~FNon_Primary_Variable_Out_of_Limits;       
           Device_Status_Byte.Secondary_Master &= ~FNon_Primary_Variable_Out_of_Limits;       
         }
        Device_Status_Reg &= ~FTemperature_Calculate;
        Device_Status_Reg |= FNew_Temperature; 
      }


 