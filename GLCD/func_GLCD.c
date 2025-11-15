#include "GLCD.h"
#include "LPC17xx.h"

static uint16_t abs_diff(uint16_t x, uint16_t y) {
  return (x > y) ? x - y : y - x;
}

void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0,
          numadd = 0, numpixels = 0, curpixel = 0;

  deltax = abs_diff(x2, x1); /* The difference between the x's */
  deltay = abs_diff(y2, y1); /* The difference between the y's */
  x = x1;                    /* Start x off at the first pixel */
  y = y1;                    /* Start y off at the first pixel */

  if (x2 >= x1) /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  } else /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1) /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  } else /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay) /* There is at least one x-value for every y-value */
  {
    xinc1 = 0; /* Don't change the x when numerator >= denominator */
    yinc2 = 0; /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax; /* There are more x-values than y-values */
  } else                /* There is at least one y-value for every x-value */
  {
    xinc2 = 0; /* Don't change the x for every iteration */
    yinc1 = 0; /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay; /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++) {
    LCD_SetPoint(x, y, color); /* Draw the current pixel */
    num += numadd;             /* Increase the numerator by the top of the fraction */
    if (num >= den)            /* Check if numerator >= denominator */
    {
      num -= den; /* Calculate the new numerator value */
      x += xinc1; /* Change the x as appropriate */
      y += yinc1; /* Change the y as appropriate */
    }
    x += xinc2; /* Change the x as appropriate */
    y += yinc2; /* Change the y as appropriate */
  }
}

void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color) {
  int16_t D;     /* Decision Variable */
  uint16_t CurX; /* Current X Value */
  uint16_t CurY; /* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  while (CurX <= CurY) {
    LCD_SetPoint(Xpos + CurX, Ypos + CurY, color);
    LCD_SetPoint(Xpos + CurX, Ypos - CurY, color);
    LCD_SetPoint(Xpos - CurX, Ypos + CurY, color);
    LCD_SetPoint(Xpos - CurX, Ypos - CurY, color);
    LCD_SetPoint(Xpos + CurY, Ypos + CurX, color);
    LCD_SetPoint(Xpos + CurY, Ypos - CurX, color);
    LCD_SetPoint(Xpos - CurY, Ypos + CurX, color);
    LCD_SetPoint(Xpos - CurY, Ypos - CurX, color);
    if (D < 0) {
      D += (CurX << 2) + 6;
    } else {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color) {
  int16_t D;
  uint16_t CurX;
  uint16_t CurY;
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY) {
    LCD_DrawUniLine(Xpos + CurX, Ypos + CurY, Xpos + CurX, Ypos - CurY, color);
    LCD_DrawUniLine(Xpos - CurX, Ypos + CurY, Xpos - CurX, Ypos - CurY, color);
    LCD_DrawUniLine(Xpos + CurY, Ypos + CurX, Xpos + CurY, Ypos - CurX, color);
    LCD_DrawUniLine(Xpos - CurY, Ypos + CurX, Xpos - CurY, Ypos - CurX, color);

    if (D < 0) {
      D += (CurX << 2) + 6;
    } else {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t color) {
  int x, y;
  x = 0;
  y = 0;
  while (x < Width) {
    while (y < Height) {
      LCD_SetPoint(Xpos + x, Ypos + y, color);
      y++;
    }
    y = 0;
    x++;
  }
}

void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a, uint16_t color) {
  int x, y;
  x = 0;
  y = 0;
  while (x < a) {
    while (y < a) {
      LCD_SetPoint(Xpos + x, Ypos + y, color);
      y++;
    }
    y = 0;
    x++;
  }
}