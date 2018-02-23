/*
  ili9327.cpp add
*/

UTFT_ILI9327::UTFT_ILI9327()
             :UTFT(ILI9327,239,319,16)
{

};

void UTFT_ILI9327::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/ili93287/initlcd.h"
    }
#include "tft_drivers/common/head2.h"
}
