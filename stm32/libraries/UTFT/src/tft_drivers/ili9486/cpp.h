/*
  ili9486.cpp add
*/

UTFT_ILI9486::UTFT_ILI9486()
             :UTFT(ILI9486,319,479,16)
{
	
};


void UTFT_ILI9486::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/ili9486/initlcd.h"
    }

#include "tft_drivers/common/head2.h"
}
