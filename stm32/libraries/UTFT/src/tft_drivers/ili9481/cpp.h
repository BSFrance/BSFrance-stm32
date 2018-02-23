/*
  ili9481.cpp add
*/

UTFT_ILI9481::UTFT_ILI9481()
             :UTFT(ILI9481,319,479,16)
{
	
};


void UTFT_ILI9481::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/ili9481/initlcd.h"
    }

#include "tft_drivers/common/head2.h"
}
