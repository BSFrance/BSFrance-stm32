/*
  ili9486.cpp add
*/

UTFT_SPFD5420::UTFT_SPFD5420()
             :UTFT(SPFD5420,239,399,16)
{
	
};


void UTFT_SPFD5420::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/SPFD5420/initlcd.h"
    }

#include "tft_drivers/common/head2.h"
}
