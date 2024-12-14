/**********************************************************
 * Проверка валидности диапазонов                   
 * 
 * 
 *********************************************************/

if (P_Out > Pmax_Lim_Journal) // превышен предел?
{
  Jurnal_Record.Parametr = P_Out;
  Pmax_Lim_Journal = Jurnal_Record.Parametr + Delta_P_Lim_Journal ;    // изменить предел
  Jurnal_Record.Param_Code=1;    //код параметра
  Jurnal_Record.Units_Code = Pass.P_Unit;   // код единиц давления
 
  write_record_2_journal(); //Записать в журнал
}
if (P_Out < Pmin_Lim_Journal)
{
  Jurnal_Record.Parametr = P_Out;
  Pmin_Lim_Journal = Jurnal_Record.Parametr - Delta_P_Lim_Journal ;   // изменить предел
  Jurnal_Record.Param_Code=1;      //код параметра
  Jurnal_Record.Units_Code = Pass.P_Unit; // код единиц давления
  
  write_record_2_journal(); //Записать в журнал
}

if (Temperature > Tmax_Lim_Journal)
{
  Jurnal_Record.Parametr  = Temperature;
  Tmax_Lim_Journal = Jurnal_Record.Parametr +  Delta_T_Lim_Journal ;   // изменить предел
  Jurnal_Record.Param_Code=2;      //код параметра
  Jurnal_Record.Units_Code = 32;   // код единиц температуры
  
  write_record_2_journal(); //Записать в журнал
}

if (Temperature < Tmin_Lim_Journal)
{
  Jurnal_Record.Parametr  = Temperature;
  Tmin_Lim_Journal = Jurnal_Record.Parametr - Delta_T_Lim_Journal ;    // изменить предел
  Jurnal_Record.Param_Code=2;      //код параметра
  Jurnal_Record.Units_Code = 32;   // код единиц температуры
  
  write_record_2_journal(); //Записать в журнал
}
// здесь еще про напряжение подумать:)  скорее всего - прямо в команде SPI его писать

