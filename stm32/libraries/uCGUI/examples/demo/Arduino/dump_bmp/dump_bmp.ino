/*
  dump_bmp_onece.ino  a uCGUI sample skatch  by huaweiwx@sina.com 2017.8.5
  use BSP_TFTLCD & uCGUI391 libraries
  tested OK: ili9320/9325/ili9341/ili9486/spfd5420 lcd fsmc drivers
*/

#include <uCGUI.h>
uCGUI  GUI;

#define BK_COLOR GUI_BLUE
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  GUI.Init();

  int XSize = LCD_GetXSize();
  int YSize = LCD_GetYSize();

  GUI.SetBkColor(BK_COLOR);
  GUI.SetFont(&GUI_Font8x16);
  GUI.Clear();

  GUI.SetDrawMode(0);
  GUI.SetColor(~BK_COLOR);
  GUI.FillCircle (XSize / 2, YSize / 2, 50);

  GUI.SetColor(GUI_RED);
  GUI.DrawLine(0, 0, XSize, YSize); //line
  GUI.DrawLine(0, YSize, XSize, 0);

  GUI.SetColor(GUI_WHITE);
  GUI.DispStringAt("HELLO", XSize / 2 - 8 * 3, 30); //text

  GUI.SetColor(~BK_COLOR);
  GUI.SetDrawMode(GUI_DM_XOR);
  //dump_bmp_onece();   //bmp1 只有第一次调用才dump,后续的dump_bmp()不会转储图像;
  for (;;) {
    GUI.FillCircle (XSize / 2, YSize / 2, 60);
    dump_bmp_onece();  //bmp2 只有第一次调用才,因此需删除前一个 dump_bmp()才能在此转储一幅bmp图形,
    digitalToggle(LED_BUILTIN);
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalToggle(LED_BUILTIN);
  delay(500);
}
void SendU8(uint8_t Data) {
  Serial.write((uint8_t)Data);
}

void SendU16(uint16_t Data) {
  Serial.write((uint8_t)(Data & 0xff));
  Serial.write((uint8_t)(Data >> 8));
}

void SendU32(uint32_t Data) {
  SendU16((uint16_t)Data);
  SendU16((uint16_t)(Data >> 16));
}


#define DIB_HEADER_MARKER ((U16) ('M' << 8) | 'B') /* DIB Header Marker - used in writing DIBs to files */
#define SIZEOF_BITMAPFILEHEADER 14
#define SIZEOF_BITMAPINFOHEADER 40

static uint8_t dumped = 0; //串口转储标志,只做一次,保证串口输出图像的正确性;
void dump_bmp_onece(void)
{
  if (dumped) return; //如已转储过,则直接返回;
  
  dumped = 1;
  int x, y, i, BPP, BytesPerLine, NumColors, Padding;
  uint32_t  BitmapOffset, FileSize;
  int xSize = LCD_GetXSize();
  int ySize = LCD_GetYSize();
  BPP          = LCD_GetBitsPerPixel();
  BytesPerLine = ((BPP > 8) ? (2 * xSize + 2) : (xSize + 3)) & ~3;
  NumColors    = (BPP > 8) ? 0 : (1 << BPP);
  Padding      = BytesPerLine - ((BPP > 8) ? (2 * xSize) : xSize);
  /* Write BITMAPFILEHEADER */
  BitmapOffset   = SIZEOF_BITMAPFILEHEADER
                   + SIZEOF_BITMAPINFOHEADER
                   + NumColors * 4;
  FileSize       = BitmapOffset
                   + ySize * BytesPerLine;
  SendU16(DIB_HEADER_MARKER);       /* bfType */
  SendU32(FileSize);                /* bfSize */
  SendU32(0);                       /* bfReserved1, bfReserved2 */
  SendU32(BitmapOffset);            /* bfOffBits */
  /* Write BITMAPINFOHEADER */
  SendU32(SIZEOF_BITMAPINFOHEADER); /* biSize */
  SendU32(xSize);                   /* biWidth */
  SendU32(ySize);                   /* biHeighth */
  SendU16(1);                       /* biPlanes */
  SendU16((BPP  <= 8) ? 8 : 16);    /* biBitCount */
  SendU32(0);                       /* biCompression */
  SendU32(0);                       /* biSizeImage */
  SendU32(0);                       /* biXPelsPerMeter */
  SendU32(0);                       /* biYPelsPerMeter */
  SendU32(NumColors);               /* biClrUsed */
  SendU32(0);                       /* biClrImportant */
  /* Write palette */
  for (i = 0; i < NumColors; i++) {
    uint32_t Color;
    Color = GUI_Index2Color(i);
    Color = ((Color >> 16) & 255) | (Color & 0xff00) | ((Color & 0xff) << 16);
    SendU32(Color);
  }
  /* Write pixels */
  for (y = ySize - 1; y >= 0; y--) {
    for (x = 0; x < xSize; x++) {
      if (BPP <= 8) {
        SendU8(LCD_GetPixelIndex(x, y));
      } else {
        uint16_t Color16;
        uint32_t Color = LCD_GetPixelColor(x, y);
        int b = ((Color >> 16) * 31 + 127) / 255;
        int g = (((Color >> 8) & 255) * 31 + 127) / 255;
        int r = ((Color & 255) * 31 + 127) / 255;
        Color16 = (r << 10) | (g << 5) | b;       /* 16 bpp Bitmaps in windows are 555: rrrrrgggggbbbbb */
        SendU16( Color16);
      }
    }
    /* Align pointer to next U32 */
    for (i = 0; i < Padding; i++) {
      SendU8(0);
    }
  }
}
