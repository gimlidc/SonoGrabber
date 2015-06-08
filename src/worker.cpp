#include <QDebug>
#include "worker.h"
#include "sessionparams.h"
#include "igtlinkclient.h"
#include "sessionnamegenerator.h"

Worker::Worker(SessionParams * conn)
{
    session = conn;
}

Worker::~Worker() {}

int Worker::saveTransform(const igtl::TransformMessage::Pointer &transMsg)
{
   igtl::Matrix4x4 matrix;

   transMsg->GetMatrix(matrix);
   tstr   << matrix[0][0] << ' ' << matrix[0][1] << ' ' << matrix[0][2] << ' ' << matrix[0][3] << ' '
        << matrix[1][0] << ' ' << matrix[1][1] << ' ' << matrix[1][2] << ' ' << matrix[1][3] << ' '
        << matrix[2][0] << ' ' << matrix[2][1] << ' ' << matrix[2][2] << ' ' << matrix[2][3] << ' '
        << matrix[3][0] << ' ' << matrix[3][1] << ' ' << matrix[3][2] << ' ' << matrix[3][3] << '\n';
   return 1;
}

int Worker::saveImage(const igtl::ImageMessage::Pointer &imgMsg)
{
    int r;
    if ((session->getChunkSize() == 0 && imageCounter == 0)
        || (session->getChunkSize() != 0 && imageCounter % session->getChunkSize() == 0)) {// open new raw image file
        rawFile.close();
        headerFile.flush();
        rawFile.setFileName(session->getRawFileName(fileCounter));
        if (!(r = rawFile.open(QIODevice::WriteOnly))) {
            qWarning() << "Cannot open raw file.";
            _writeFlag = false;
            return r;
        }
        fileCounter++;
    }

    //r = rawFile.write(qCompress((const uchar *) imgMsg->GetScalarPointer(),imgMsg->GetImageSize(),9));
    r = rawFile.write((char *) imgMsg->GetScalarPointer(),imgMsg->GetImageSize());
    if (r == -1) {
        qWarning() << "Cannot write to raw image file";
        _writeFlag = false;
        return r;
    }
    imageCounter++;
    return 1;
}

void Worker::flushData(double ts)
{
    int i;
    // check if the time stamps are equal then save data to the file
    if (imgTS.count(ts) + transTS.count(ts) == imgTS.size() + transTS.size()) {
        //qDebug() << "save";
        if (imgTS.count() == 0) // !!! At least one image
             return;
        if (!imageCounter) { // first image has arrived - generate header
            writeHeader(imgMsgList[0]);
        }
        QString s = SessionNameGenerator::generateTransformFileName(imageCounter);
        tstr << s << "Timestamp = " << ts << '\n';
        for (i = 0; i < transTS.size(); ++i) {
            tstr << s << transMsgList[i]->GetDeviceName() << "Transform = ";
            saveTransform(transMsgList[i]);
            tstr << s << transMsgList[i]->GetDeviceName() << "TransformStatus = OK\n";

        }
        saveImage(imgMsgList[0]);
        tstr << s << "ImageStatus = OK\n";
        qDebug() << imageCounter;
    }

}

int Worker::createOutDir()
{
    if (session->getOutDir().exists()) {
        qWarning() << "Directory exists data can be overwritten: " << session->getOutDir().absolutePath();
    } else {
        qDebug() << "Creating target directory: " << session->getOutDir().absolutePath();
        if (!QDir().mkpath(session->getOutDir().absolutePath())) {
            return 1;
        }
    }

    return 0;
}

void Worker::setOutput()
{       
    for (int i = 0; i < session->getImageNames().size(); ++i)
        imgMsgList.append(igtl::ImageMessage::New());

    for (int i = 0; i < session->getTransNames().size(); ++i)
        transMsgList.append(igtl::TransformMessage::New());

    imgTS.fill(0.0,session->getImageNames().size());
    transTS.fill(0.0,session->getTransNames().size());
}

void Worker::openHeaderFile()
{
    qWarning() << "opening header file";
    headerFile.setFileName(session->getHeaderFileName());
    if (!headerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file" << headerFile.fileName();
        _writeFlag = false;
        return;
    } else {
        qDebug() << "Header file opened: " << session->getHeaderFileName();
    }
    _writeFlag = true;
    tstr.setDevice(&headerFile);
    tstr.setRealNumberPrecision(16);

    imageCounter = 0;
    fileCounter = 0;
}

void Worker::closeFiles()
{
    if(_writeFlag)
    {
        writeFooter();
    }
    _writeFlag = false;
    rawFile.close();
    headerFile.close();
}

void Worker::writeHeader(const igtl::ImageMessage::Pointer &imgMsg)
{
    // Retrive the image data
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

    tstr << "ObjectType = Image\n"
       << "NDims = 3\n"
       << "AnatomicalOrientation = RAI\n"
       << "BinaryData = True\n"
       << "BinaryDataByteOrderMSB = False\n"
       << "CenterOfRotation = 0 0 0\n"
       << "CompressedData = False\n"
       << "ElementNumberOfChannels = 1\n"
       << "Offset = 0 0 0\n"
       << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n"
       << "UltrasoundImageOrientation = MF\n"
       << "UltrasoundImageType = BRIGHTNESS\n"
       << "ElementType = MET_UCHAR\n"
       << "ElementSpacing = 1 1 1\n"; //<< "ElementSpacing = " << spacing[0] << ' ' << spacing[1] << ' ' << spacing[2] << '\n'
}

void Worker::writeFooter()
{
    int i;
    tstr << "DimSize = " << size[0] << ' ' << size[1] << ' ' << imageCounter << '\n'
       << "ElementDataFile = "; // LIST" << '\n';
    for (i = 0; i < fileCounter; ++i)
    {
        tstr << session->getRawFileName(i) << ' ';
    }
    tstr.flush();
}

void Worker::stop()
{
    Lock.lockForWrite();
    Terminate = true;
    Lock.unlock();
}

int Worker::isRunning()
{
    int r;
    Lock.lockForRead();
    r = !Terminate;
    Lock.unlock();
    return r;
}

void Worker::start()
{
    int resultCode;
    // Create a message buffer to receive header
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();

    //------------------------------------------------------------
    // Allocate a time stamp
    igtl::TimeStamp::Pointer ts;
    ts = igtl::TimeStamp::New();

    createOutDir();

    openHeaderFile();

    Lock.lockForWrite();
    Terminate = false;
    Lock.unlock();

    resultCode = session->openSocket();
    if (resultCode != 0) {
        qWarning() << "Connection to server failed. Error code: %1" << resultCode;
        return;
    } else {
        qDebug() << "Socket opened. Reading data loop ...";
    }

    setOutput();

    while (1) {
        Lock.lockForRead();
        if (Terminate) {
            Lock.unlock();
            resultCode = IGTLinkClient::UserInterrupt;
            break;
        }
        Lock.unlock();

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = session->socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r == 0) {
            qWarning() << "Receive error:" << r;
            resultCode = IGTLinkClient::ReceiveError;
            break;
        }
        if (r != headerMsg->GetPackSize()) {
            continue;
        }

        // Deserialize the header
        headerMsg->Unpack();

        // Get time stamp
        igtlUint32 sec;
        igtlUint32 nanosec;

        headerMsg->GetTimeStamp(ts);
        ts->GetTimeStamp(&sec, &nanosec);

        //qDebug() << "Time stamp:"
        //          << sec << "." //<< std::setw(9) << std::setfill('0')
        //          << nanosec;

        // Check data type and receive data body
        if (qstrcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0) {
            ReceiveTransform(session->socket, headerMsg);
        } else if (qstrcmp(headerMsg->GetDeviceType(), "POSITION") == 0) {
            ReceivePosition(session->socket, headerMsg);
        } else if (qstrcmp(headerMsg->GetDeviceType(), "IMAGE") == 0) {
            ReceiveImage(session->socket, headerMsg);
        } else {
            qWarning() << "Receiving unknown message:" << headerMsg->GetDeviceType();
            session->socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
    }

    closeFiles();
    session->closeSocket();
    emit stopped(resultCode);
}

int Worker::ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  int i;
  //qDebug() << "Receiving TRANSFORM data type.";

  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) { // if CRC check is OK
      // Allocate a time stamp
      igtl::TimeStamp::Pointer ts;
      ts = igtl::TimeStamp::New();
      transMsg->GetTimeStamp(ts);

      i = session->getTransNames().indexOf(transMsg->GetDeviceName());
      if (i == -1) {
          //qDebug() << "Transform ignored:" << transMsg->GetDeviceName();
          return i;
      }
      transTS[i] = ts->GetTimeStamp();
      transMsgList[i] = transMsg;

      if (_writeFlag)
          flushData(transTS[i]);
      emit transformReceived(transMsg);
      return i;
  }
  qWarning() << "CRC check error!";
  return 0;
}

int Worker::ReceivePosition(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  qDebug() << "Receiving POSITION data type.";

  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();

  // Receive position position data from the socket
  socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) { // if CRC check is OK
    // Retrive the transform data
    //float position[3];
    //float quaternion[4];

    //positionMsg->GetPosition(position);
    //positionMsg->GetQuaternion(quaternion);

    //std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
    //std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
     //         << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;
    // emit ******
    return 1;
  }
  qWarning() << "CRC check error!";
  return 0;
}

int Worker::ReceiveImage(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
  int i;
  //qDebug() << "Receiving IMAGE data type.";

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) { // if CRC check is OK
      //qDebug() << "Image name:" << imgMsg->GetDeviceName();
      // Allocate a time stamp
      igtl::TimeStamp::Pointer ts;
      ts = igtl::TimeStamp::New();
      imgMsg->GetTimeStamp(ts);

      i = session->getImageNames().indexOf(imgMsg->GetDeviceName());
      if (i == -1) {
          qDebug() << "Image ignored:" << imgMsg->GetDeviceName();
          return i;
      }
      qDebug() << 1/(ts->GetTimeStamp()-imgTS[i]);
      imgTS[i] = ts->GetTimeStamp();
      imgMsgList[i] = imgMsg;
      if (_writeFlag) {
          flushData(imgTS[i]);
      }
      emit imageReceived(imgMsg);
      return i;
  }
  qWarning() << "CRC check error!";
  return 0;
}
