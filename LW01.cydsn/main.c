#include "project.h"
#include <stdlib.h>

int main(void)
{
    CyGlobalIntEnable; 
    int lastState = 0;
    for(;;)
    {
      int currentState = Button_Read();
      if(currentState == 1 && lastState == 0)
        { 
           LED_Red_Write(rand() % 2);
           LED_Green_Write(rand() % 2);
           LED_Blue_Write(rand() % 2);
          
        CyDelay(200);
        }

        lastState = currentState;
    }
}
