#include "imageprocessor.h"
#include <QDebug>

int ImageProcessor::cropImage(char* original, QSize size, QRect rect, char * target)
{
    if (rect.x() < 0
            || rect.y() < 0
            || rect.width() + rect.x() > size.width()
            || rect.height() + rect.y() > size.height())
        return 1;

    for(int y = 0; y < rect.height(); y++) {
        memcpy(target + y * rect.width(), original + (rect.y() + y) * size.width() + rect.x(), rect.width());
    }

    return 0;
}

bool ImageProcessor::isFrozen(char * original, QSize size, QRect freezeMark)
{
    char* freeze = (char*)malloc(freezeMark.width() * freezeMark.height() * sizeof(uchar));
    if (ImageProcessor::cropImage(original, size, freezeMark, freeze) != 0) {
        free(freeze);
        return false;
    }

    // here do some computation
    int sum = 0;
    // Gimli's old code
    // for(int i = 0; i < freezeMark.width() * freezeMark.height(); i++) {
    //    sum += freeze[i];
    //}
    char *b, *t;
    for(int i = 1; i < freezeMark.height(); i++) {
        t = freeze+i*freezeMark.width();
        b = freeze+(i-1)*freezeMark.width();
        for(int j = 0; j < freezeMark.width(); j++) {
            sum += abs((int)(*t++) - (int)(*b++))>0;
        }
    }

    free(freeze);
    //return (sum < -10000);
    return (sum > freezeMark.width()-1);
}
