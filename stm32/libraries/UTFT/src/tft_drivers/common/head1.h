	orient=orientation;
	_hw_special_init();
	
#ifdef FSMC_TFT
	_set_direction_registers(display_transfer_mode);
#else
	
	pinMode(__p1,OUTPUT);
	pinMode(__p2,OUTPUT);
	pinMode(__p3,OUTPUT);
	if (__p4 != NOTINUSE)
		pinMode(__p4,OUTPUT);
	if ((display_transfer_mode==LATCHED_16) or ((display_transfer_mode==1) and (display_serial_mode==SERIAL_5PIN)))
		pinMode(__p5,OUTPUT);
	if (display_transfer_mode!=1)
	  _set_direction_registers(display_transfer_mode);

	sbi(P_RST, B_RST);
	delay(5); 
	cbi(P_RST, B_RST);
	delay(15);
	sbi(P_RST, B_RST);
	delay(15);

	cbi(P_CS, B_CS);
#endif

  delay(50); // delay 50 ms 
  LCD_Write_COM_DATA(0x00,0x0001);
  delay(50); // delay 50 ms 
  mode_id =  LCD_ReadData();   
  if((mode_id < 0XFF)||(mode_id == 0XFFFF)||(mode_id ==0X9300)){ //teset 9341 6804 5310 5510
    LCD_Write_COM(0xD3);      //test ili9341
	mode_id = LCD_ReadData(); //dummy read 
	mode_id = LCD_ReadData(); //read 0x00
	mode_id = LCD_ReadData();   //0x93
	mode_id <<=8;  
	mode_id |=LCD_ReadData();  //0x41
	if (mode_id !=0x9341){
      LCD_Write_COM(0xBF);      //test ili9341
	  mode_id = LCD_ReadData(); //dummy read 
	  mode_id = LCD_ReadData(); //read 0x01
	  mode_id = LCD_ReadData();   //0xD0
	  mode_id = LCD_ReadData();   //0x68
	  mode_id <<=8;  
	  mode_id |=LCD_ReadData();  //0x41
	  if (mode_id !=0X6804){
        LCD_Write_COM(0xD4);      //NT35310
	    mode_id = LCD_ReadData(); //dummy read 
	    mode_id = LCD_ReadData(); //read 0x01
		mode_id = LCD_ReadData();   //0x53
		mode_id <<=8;  
		mode_id |=LCD_ReadData();  //0x10
	    if (mode_id !=0X5310){    //NT35510
		  LCD_Write_COM(0xDA00); 
	      mode_id = LCD_ReadData(); //dummy read 
		  LCD_Write_COM(0xDB00); 
	      mode_id = LCD_ReadData(); //read 0x80
		  mode_id <<=8;  
		  LCD_Write_COM(0xDC00); 
		  mode_id |=LCD_ReadData();  //0x00
		  if(mode_id ==0x8000){
		    mode_id = 0x5510; // is NT35510
	      } else{
		    mode_id =0x00;
		  }
	    }
	  }
	}
  }
