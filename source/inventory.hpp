#pragma once

void inventoryUpdate();
void inventoryDraw(float subFrame, float depth);

#ifdef _3DS
void inventoryTouch();
#endif