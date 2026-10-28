#include "procedure.h"

void Start() {
    static int progress_num = 0;

    switch(progress_num) {
        case 0:  
            // 第一步：前进200单位
            if(Straight(200, 10)) {
                progress_num++;
            }
            break;
        case 1:
            // 第二步：转弯180度
            if(SelfTurn(180)) {
                progress_num++;
            }
            break;
        case 2:
            // 第三步：前进200单位，回到起点
            if(Straight(200, 10)) {
                progress_num++;
            }
            break;
        default:
            // 完成所有步骤后，停止
            break;
    }

}
