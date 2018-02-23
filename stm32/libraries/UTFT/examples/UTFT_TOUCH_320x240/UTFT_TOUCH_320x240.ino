// UTFT_Demo_320x240
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution
// of 320x240 pixels.
//
// This program requires the UTFT library.
// for  RedBull ev_board(STM32F103Zet6) with ili9320_TFT  by huawei<huaweiwx@sina.com> 2016.12.20
//

#include <UTFT.h>
#include <BSP_Touch.h>


//UTFT_ILI9320 myGLCD; //320x240 ili9320 chip
UTFTCLASS myGLCD;     //board default BSP_TFT_LCD config
TOUCH myTP;

#define LED LED_BUILTIN

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  // Setup the LCD
  myGLCD.Init();
  myGLCD.setFont(SmallFont);
  myTP.Init(1);//缺省0 竖屏 /1 横屏
  attachInterrupt(TOUCH_NIRQ, intcallback, GPIO_MODE_IT_RISING) ;

  myGLCD.clrScr();

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 226, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.printStr("* STM32 Color TFT Display Library *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255, 255, 0);
  myGLCD.printStr("<huaweiwx@sina.com>", CENTER, 227);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 319, 225);
  myGLCD.setFont(BigFont);
}

void  tpxy(void)
{
  // Clear the screen and draw the frame
  uint16_t x, y;
  myTP.readXY2(&x, &y);

  myGLCD.setColor(0, 255, 255);
  myGLCD.printStr("tp x:", LEFT, 20);
  myGLCD.printStr("    ", CENTER, 20);
  myGLCD.printNumI(x, CENTER, 20);
  myGLCD.printStr("tp y:", LEFT, 40);
  myGLCD.printStr("    ", CENTER, 40);
  myGLCD.printNumI(y, CENTER, 40);
  if (myTP.scan(0)) {
    Serial << "x=" << myTP.pdev->x[0] << "\n";
    Serial << "y=" << myTP.pdev->y[0] << "\n";
  }
  digitalToggle(LED);
  delay (1000);
}

static uint8_t test = 0;
void loop()
{
  Adjust();
}

void intcallback(void) {
  test++;
}
uint16_t POINT_COLOR;

void TP_Adj_Info_Show(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t fac)
{
  Serial << "x1:" << _HEX(x0) << "  ";  Serial << "y1:" << _HEX(y0) << "\n";
  Serial << "x2:" << _HEX(x1) << "  ";  Serial << "y1:" << _HEX(y1) << "\n";
  Serial << "x3:" << _HEX(x2) << "  ";  Serial << "y1:" << _HEX(y2) << "\n";
  Serial << "x4:" << _HEX(x3) << "  ";  Serial << "y1:" << _HEX(y3) << "\n";
  Serial << "fac is:" << _HEX(fac) << "\n";
}

void show_Adj(void)
{
  Serial << "Xsize:" <<myGLCD.getDisplayXSize() <<"\n";
  Serial << "xoff:" << myTP.pdev->xoff << "\n";
  Serial << "xfac:" << 1000*myTP.pdev->xfac << "\n";
  Serial << "Ysize:" <<myGLCD.getDisplayYSize() <<"\n";
  Serial << "yoff:" << myTP.pdev->yoff << "\n";
  Serial << "yfac:" << 1000*myTP.pdev->yfac << "\n";

  while (1) {
    tpxy();
  }
}

void TP_Drow_Touch_Point(uint16_t x, uint16_t y, uint16_t color)
{
  uint16_t tmpc = myGLCD.getColor();
  myGLCD.setColor(color);
  myGLCD.drawLine(x - 12, y, x + 13, y); //横线
  myGLCD.drawLine(x, y - 12, x, y + 13); //竖线
  myGLCD.drawPixel(x + 1, y + 1);
  myGLCD.drawPixel(x - 1, y + 1);
  myGLCD.drawPixel(x + 1, y - 1);
  myGLCD.drawPixel(x - 1, y - 1);
  myGLCD.drawCircle(x, y, 6); //画中心圈
  myGLCD.setColor(tmpc);
}

//触摸屏校准代码
//得到四个校准参数
void Adjust(void)
{
  uint16_t pos_temp[4][2];//坐标缓存值
  uint8_t  cnt = 0;
  uint16_t d1, d2;
  uint32_t  tem1, tem2;
  double fac;
  uint16_t outtime = 0;
  float factmp;
  cnt = 0;

  TP_Drow_Touch_Point(20, 20, WHITE ); //画点1
  myTP.pdev->sta = 0; //消除触发信号
  myTP.pdev->xfac = 0; //xfac用来标记是否校准过,所以校准之前必须清掉!以免错误
  while (1) //如果连续10秒钟没有按下,则自动退出
  {
    while (myTP.scan(1) == 0); //扫描物理坐标 没有按，等待；
    while (myTP.scan(1)) {     //等待按键松开
      delay(100);
    }
    outtime = 0;
    myTP.pdev->sta &= ~(1 << 6); //标记按键已经被处理过了.

    pos_temp[cnt][0] = myTP.pdev->x[0];
    pos_temp[cnt][1] = myTP.pdev->y[0];
    cnt++;
    switch (cnt)
    {
      case 1:
        TP_Drow_Touch_Point(20, 20, BLACK);				//清除点1
        TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, 20, WHITE);	//画点2
        break;
      case 2:
        TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, 20, BLACK);	//清除点2
        TP_Drow_Touch_Point(20, myGLCD.getDisplayYSize() - 20, WHITE);	//画点3
        break;
      case 3:
        TP_Drow_Touch_Point(20, myGLCD.getDisplayYSize() - 20, BLACK);			//清除点3
        TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, myGLCD.getDisplayYSize() - 20, WHITE);	//画点4
        break;
      case 4:	 //全部四个点已经得到
        //对边相等
        tem1 = abs(pos_temp[0][0] - pos_temp[1][0]); //x1-x2
        tem2 = abs(pos_temp[0][1] - pos_temp[1][1]); //y1-y2
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = sqrt(tem1 + tem2); //得到1,2的距离

        tem1 = abs(pos_temp[2][0] - pos_temp[3][0]); //x3-x4
        tem2 = abs(pos_temp[2][1] - pos_temp[3][1]); //y3-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = sqrt(tem1 + tem2); //得到3,4的距离
        fac = (float)d1 / d2;
        if (fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, myGLCD.getDisplayYSize() - 20, BLACK);	//清除点4
          TP_Drow_Touch_Point(20, 20, WHITE);								//画点1
          TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1], pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
          continue;
        }
        tem1 = abs(pos_temp[0][0] - pos_temp[2][0]); //x1-x3
        tem2 = abs(pos_temp[0][1] - pos_temp[2][1]); //y1-y3
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = sqrt(tem1 + tem2); //得到1,3的距离

        tem1 = abs(pos_temp[1][0] - pos_temp[3][0]); //x2-x4
        tem2 = abs(pos_temp[1][1] - pos_temp[3][1]); //y2-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = sqrt(tem1 + tem2); //得到2,4的距离
        fac = (float)d1 / d2;
        if (fac < 0.95 || fac > 1.05) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, myGLCD.getDisplayYSize() - 20, BLACK);	//清除点4
          TP_Drow_Touch_Point(20, 20, WHITE);								//画点1
          TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1], pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
          continue;
        }//正确了

        //对角线相等
        tem1 = abs(pos_temp[1][0] - pos_temp[2][0]); //x1-x3
        tem2 = abs(pos_temp[1][1] - pos_temp[2][1]); //y1-y3
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = sqrt(tem1 + tem2); //得到1,4的距离

        tem1 = abs(pos_temp[0][0] - pos_temp[3][0]); //x2-x4
        tem2 = abs(pos_temp[0][1] - pos_temp[3][1]); //y2-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = sqrt(tem1 + tem2); //得到2,3的距离
        fac = (float)d1 / d2;
        if (fac < 0.95 || fac > 1.05) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, myGLCD.getDisplayYSize() - 20, BLACK);	//清除点4
          TP_Drow_Touch_Point(20, 20, WHITE);								//画点1
          TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1], pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
          continue;
        }//正确了
        //计算结果
        factmp = (float)((myGLCD.getDisplayXSize() - 20.0) / (pos_temp[1][0] - pos_temp[0][0])); //得到xfac
        myTP.pdev->xfac = factmp;
        factmp = factmp * pos_temp[0][0] - 10;
        myTP.pdev->xoff = factmp;                //得到xoff

        factmp = (float)((myGLCD.getDisplayYSize() - 20.0) / (pos_temp[2][1] - pos_temp[0][1])); //得到yfac
        myTP.pdev->yfac = factmp;
        factmp = factmp * pos_temp[0][1] - 10;
        myTP.pdev->yoff = factmp;  //得到yoff
        
        show_Adj();
        if (abs(myTP.pdev->xfac) > 2 || abs(myTP.pdev->yfac) > 2) //触屏和预设的相反了.
        {
          cnt = 0;
          myGLCD.clrScr();
          TP_Drow_Touch_Point(myGLCD.getDisplayXSize() - 20, myGLCD.getDisplayYSize() - 20, WHITE);	//清除点4
          TP_Drow_Touch_Point(20, 20, WHITE);								//画点1
          myGLCD.printStr("TP Need readjust!", 40, 26);
          myTP.pdev->touchtype = !myTP.pdev->touchtype; //修改触屏类型.
          if (myTP.pdev->touchtype) //X,Y方向与屏幕相反
          {
            myTP.pdev->CMD_RDX = 0X90;
            myTP.pdev->CMD_RDY = 0XD0;
          } else				   //X,Y方向与屏幕相同
          {
            myTP.pdev->CMD_RDX = 0XD0;
            myTP.pdev->CMD_RDY = 0X90;
          }
          continue;
        }
        //					POINT_COLOR=BLUE;
        myGLCD.clrScr();
        myGLCD.printStr("Touch Screen Adjust OK!", 35, 110);
        show_Adj();
        delay(1000);
        // 					LCD_Clear(WHITE);//清屏
        return;//校正完成
    }
    delay(2000);
    digitalToggle(LED);
  }
}
