#include "imageprocessor.h"

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

    free(freeze);
    return false;
}
