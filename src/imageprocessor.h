#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdlib.h>
#include <QSize>
#include <QRect>

class ImageProcessor
{
public:
    /**
     * @brief cropImage crop the image (stored in byte array)
     * @param original original image
     * @param size size of original image
     * @param rect target rectangle (must be included in original)
     * @param target preallocated memory for image storing
     * @return 0 if image was successfully cropped, 1 iff dimensions of rectangle are wrong.
     */
    static int cropImage(char * original, QSize size, QRect rect, char * target);

    /**
     * @brief isFrozen checks if frozen mark is on
     * @param original original image stored in byte array
     * @param size size of original image
     * @return true iff frozen mark is enabled
     */
    static bool isFrozen(char * original, QSize size, QRect freezeMark);
};

#endif // ANALYZER_H
