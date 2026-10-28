#include "tim.h"

void SetDirection(int16_t Angle)
{
	if(Angle<=-100){Angle=-100;}
	if(Angle>=100){Angle=100;}
__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,Angle*4+1500);

}
