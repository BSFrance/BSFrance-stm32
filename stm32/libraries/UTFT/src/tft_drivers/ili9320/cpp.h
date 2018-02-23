/*
  ili9320.cpp add
*/

UTFT_ILI9320::UTFT_ILI9320()
             :UTFT(ILI9320,239,319,16)
{

};

void UTFT_ILI9320::Init(byte orientation){
#include "tft_drivers/common/head1.h"
	switch(display_model)
	{

#include "tft_drivers/ili9320/initlcd.h"
    }
#include "tft_drivers/common/head2.h"
}
