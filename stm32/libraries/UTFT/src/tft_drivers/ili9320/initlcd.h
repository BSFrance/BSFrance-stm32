//for  ILI9320   add by huaweiwx@sina.com 2017.1.12

  case ILI9320:
//	LCD_Write_COM_DATA(0xe5, 0x8000);   
	LCD_Write_COM_DATA(0x00, 0x0001); //start osc
	LCD_Write_COM_DATA(0x01, 0x0100); // set Driver Output Control: SS and SM bit   
	LCD_Write_COM_DATA(0x02, 0x0700); // LCD Driver Waveform Contral.  
	LCD_Write_COM_DATA(0x03, 0x1030); // set GRAM write direction and BGR=1.  
//	LCD_Write_COM_DATA(0x03, 0x1018); // set GRAM write direction and BGR=1.  
	
	LCD_Write_COM_DATA(0x04, 0x0000); // Resize register  
	LCD_Write_COM_DATA(0x08, 0x0202); // set the back porch and front porch  
	LCD_Write_COM_DATA(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]  
	LCD_Write_COM_DATA(0x0A, 0x0000); // FMARK function  
	LCD_Write_COM_DATA(0x0C, (1<<0)); // RGB interface setting  
	LCD_Write_COM_DATA(0x0D, 0x0000); // Frame marker Position  
	LCD_Write_COM_DATA(0x0F, 0x0000); // RGB interface polarity 

//	delay(50); // Delay 50ms  
//	LCD_Write_COM_DATA(0x07, 0x0101);  //Display Contral.
//	delay(50); // Delay 50ms  

	//*************Power On sequence ****************//  
	LCD_Write_COM_DATA(0x10, 0x0000);        
	LCD_Write_COM_DATA(0x11, 0x0007);        
	LCD_Write_COM_DATA(0x12, 0x0000);        
	LCD_Write_COM_DATA(0x13, 0x0000);        
	delay(50); // Delay 50ms  

	LCD_Write_COM_DATA(0x10, (1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4)); //Power Control 1.(0x16b0)  
	LCD_Write_COM_DATA(0x11, 0x0007); // Power Control 2.(0x0001) 
	delay(10); // Delay 50ms  	
	LCD_Write_COM_DATA(0x12, (1<<8)|(1<<4)|(0<<0));	//Power Control 3.(0x0138)  
	delay(10); // Delay 50ms  		
	LCD_Write_COM_DATA(0x13, 0x0b00); // VDV[4:0] for VCOM amplitude  
	LCD_Write_COM_DATA(0x29, 0x0000); // 04  VCM[5:0] for VCOMH  
	delay(10); // Delay 50ms  		
	LCD_Write_COM_DATA(0x2B, (1<<14)|(1<<4)); // Set Frame Rate  

	//-----Gamma control-----------------------        
	LCD_Write_COM_DATA(0x30, 0x0000);        
	LCD_Write_COM_DATA(0x31, 0x0505);        
	LCD_Write_COM_DATA(0x32, 0x0004);        
	LCD_Write_COM_DATA(0x35, 0x0006);        
	LCD_Write_COM_DATA(0x36, 0x0707);        
	LCD_Write_COM_DATA(0x37, 0x0105);        
	LCD_Write_COM_DATA(0x38, 0x0002);        
	LCD_Write_COM_DATA(0x39, 0x0707);        
	LCD_Write_COM_DATA(0x3C, 0x0704);        
	LCD_Write_COM_DATA(0x3D, 0x0807);        	
	//------------------ Set GRAM area ---------------//  
	LCD_Write_COM_DATA(0x50, 0x0000); //Set X Start.  
	LCD_Write_COM_DATA(0x51,    239); //Set X End 
	LCD_Write_COM_DATA(0x52, 0x0000); // Vertical GRAM Start Address  
	LCD_Write_COM_DATA(0x53,    319); // Vertical GRAM end Address 
	
	LCD_Write_COM_DATA(0x60, 0x2700); // Gate Scan Line  
	LCD_Write_COM_DATA(0x61, 0x0001); // NDL,VLE, REV   
	LCD_Write_COM_DATA(0x6A, 0x0000); // set scrolling line
	
	//-------------- Partial Display Control ---------//  
	LCD_Write_COM_DATA(0x80, 0x0000);  
	LCD_Write_COM_DATA(0x81, 0x0000);  
	LCD_Write_COM_DATA(0x82, 0x0000);  
	LCD_Write_COM_DATA(0x83, 0x0000);  
	LCD_Write_COM_DATA(0x84, 0x0000);  
	LCD_Write_COM_DATA(0x85, 0x0000);
	
	//-------------- Panel Control -------------------//  
	LCD_Write_COM_DATA(0x90, (0<<7)|(16<<0));  
	LCD_Write_COM_DATA(0x92, 0x0000);  
	LCD_Write_COM_DATA(0x93, 0x0001);  
	LCD_Write_COM_DATA(0x95, 0x0110);  
	LCD_Write_COM_DATA(0x97, (0<<8));  
	LCD_Write_COM_DATA(0x98, 0x0000);	//Frame Cycle Contral.
	
	LCD_Write_COM_DATA(0x07, 0x0173); // 262K color and display ON        
break;
