#ifndef FILENAMEGENERATOR_H
#define FILENAMEGENERATOR_H
#include <QString>

/**
 * @brief The SessionNameGenerator class
 *
 * Grabbed data are split into sessions according to clicking on button run/stop.
 * This class generates for each session new name. Name is only suggested (not
 * necessarily unique).
 */
class SessionNameGenerator
{
public:
    /**
     * @brief generate generates "unique" directory name in specified directory
     *
     * Generatator suggest new subdirectory name. Name consists from date and time.
     * If such directory exists (multiple records in same second) milliseconds are
     * added. If this is not sufficient previous directory will be overwritten.
     *
     * @param dirPath target directory, where new subdirectory should be created
     * @return name of a new directory (directory is not created yet)
     */
    static QString generateSessionDirectory(QString dirPath);

    /**
     * @brief generateRawFileName generates new filename which is ordered by fileNo
     * @param fileNo order of the raw file in the session
     * @return ordered filename for raw data
     */
    static QString generateRawFileName(int fileNo);
};

#endif // FILENAMEGENERATOR_H
