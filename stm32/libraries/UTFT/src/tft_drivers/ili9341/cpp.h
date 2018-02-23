/*
  ili9341.cpp add
*/

UTFT_ILI9341::UTFT_ILI9341()
             :UTFT(ILI9341_16,239,319,16)
{
	
};


void UTFT_ILI9341::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/ili9341/initlcd.h"
    }

#include "tft_drivers/common/head2.h"
}
