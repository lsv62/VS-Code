// file HART_meander_test.c 

const unsigned int Meander_Pulse_Front_Value[]=
{
  /*
0x2000, 0x28cd, 0x319a, 0x3a67, 0x4334, 0x4c01, 0x54ce, 0x5d9b, 0x6668, 0x6f35,
0x7802, 0x80cf, 0x899c, 0x9269, 0x9b36, 0xa403, 0xacd0, 0xb59d, 0xbe6a, 0xc737  
 */
  

0x2000, /*0x28cd, 0x319a, 0x3a67,0x4334,*/ 0x4c01,  /*0x54ce, 0x5d9b, 0x6668, 0x6f35,*/
0x7802, /*0x80cf, 0x899c, 0x9269, 0x9b36,*/ 0xa403, /*0xacd0, 0xb59d, 0xbe6a, 0xc737 */ 
 

/*  
0x2000, 0x2466, 0x28cd, 0x2D33, 0x319a, 0x3600, 0x3a67, 0x3ECD, 0x4334, 0x479A,
0x4c01, 0x5067, 0x54ce, 0x5934, 0x5d9b, 0x6201, 0x6668, 0x6ACE, 0x6f35, 0x739B,
0x7802, 0x7C68, 0x80cf, 0x8535, 0x899c, 0x8E02, 0x9269, 0x96CF, 0x9b36, 0x9F9C, 
0xa403, 0xA869, 0xacd0, 0xB136, 0xb59d, 0xBA03, 0xbe6a, 0xC2D0, 0xc737  
*/
};

switch (HART_Test_Ctrl_Register & FTest_Meander_Mask) 
  {
    case 0: // front
 
        if (!(Meander_Pulse_Counter))
          DAC_Out=Code_Down;
         else
          {
           if (Meander_Pulse_Counter  >=4)//20
            {
              Meander_Pulse_Counter = 0;
              HART_Test_Ctrl_Register++;
              DAC_Out=Code_Up;
            }
            else
             DAC_Out=Meander_Pulse_Front_Value[Meander_Pulse_Counter];
           }
  /*        
              HART_Test_Ctrl_Register++;
              DAC_Out=Code_Up;
    */  
         break;
    case 1: // top
          if (Meander_Pulse_Counter  >=10)
           {
             Meander_Pulse_Counter = 0;
             HART_Test_Ctrl_Register++; 
           }
           DAC_Out=Code_Up;
         break;  
    case 2: // droop
      /**/
        if (!(Meander_Pulse_Counter))
          DAC_Out=Code_Up;
         else
          {
           if (Meander_Pulse_Counter  >=4) //10
            {
              Meander_Pulse_Counter = 0;
              HART_Test_Ctrl_Register++;
              DAC_Out=Code_Down;
            }
            else
             DAC_Out=Meander_Pulse_Front_Value[4-Meander_Pulse_Counter];
           }
      /*
              HART_Test_Ctrl_Register++;
              DAC_Out=Code_Down;
	 */
         break;         
    case 3: // bottom
          if (Meander_Pulse_Counter >=10)
            {
              Meander_Pulse_Counter = 0;
              HART_Test_Ctrl_Register &= ~FTest_Meander_Mask; 
              Hart_Meander_Test_Counter--;
            }
           DAC_Out=Code_Down;
         break;         
  }
  