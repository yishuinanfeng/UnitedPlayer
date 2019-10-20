//
// Created by yanxi on 2019/10/19.
//

#include "IResample.h"

void IResample::Update(XData data) {
    XData xData = this->Resample(data);
    if (xData.size >0){
        this->notify(xData);
    }
}
