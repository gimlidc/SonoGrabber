#include <QDebug>
#include "worker.h"
#include "sessionparams.h"
#include "igtlinkclient.h"
#include "imageprocessor.h"
#include "sessionnamegenerator.h"

Worker::Worker(SessionParams * conn)
{
    session = conn;
    frozenImageStored = false;
}

Worker::~Worker() {
}

void Worker::writeAndNotify(char * imgBuffer, QSize dimensions, bool isFrozen, bool isCropped)
{
    QString state;

    if (isFrozen) {
        state = "FROZEN";
    } else if (isCropped) {
        state = "CROPPED";
    } else {
        state = "OK";
    }

    if (!isFrozen || !frozenImageStored) {
        writer->writeImage(imgBuffer, dimensions);
        frozenImageStored = isFrozen;
        emit imageReceived(imgBuffer, dimensions, state);
    }

}

void Worker::flushData(double ts)
{
    // check if the time stamps are equal then save data to the file
    if (imgTS.count(ts) + transTS.count(ts) == imgTS.size() + transTS.size()) {
        //qDebug() << "save";
        if (imgTS.count() == 0) // !!! At least one image
             return;
        writer->writeHeader(imgMsgList[0]);
        writer->startSequence(ts);
        for (int i = 0; i < transTS.size(); ++i) {
            writer->writeTransform(transMsgList[i]);
        }
        int size[3];
        imgMsgList[0]->GetDimensions(size);
        QSize dimensions(size[0], size[1]);

        bool isFrozen = ImageProcessor::isFrozen((char *)imgMsgList[0]->GetScalarPointer(), dimensions, session->getFreeze());

        if (isFrozen && frozenImageStored) {
            return;
        }

        qDebug() << isFrozen << "stored: " << frozenImageStored;

        if (session->shouldCrop(dimensions)) {
            char * imgBuffer = (char *)malloc(session->getCrop().width() * session->getCrop().height() * sizeof(uchar));
            ImageProcessor::cropImage((char *)imgMsgList[0]->GetScalarPointer(), dimensions, session->getCrop(), imgBuffer);
            writeAndNotify(imgBuffer, session->getCrop().size(), isFrozen, true);

        } else {
            writeAndNotify((char *) imgMsgList[0]->GetScalarPointer(), dimensions, isFrozen, false);
        }

        writer->closeSequence();
    }

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

    writer = new Writer(session->getOutDir(), session->getChunkSize());
    writer->createOutDir();

    writer->openHeaderFile();

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

        //qWarning() << "Receiving message:" << headerMsg->GetDeviceType();

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

    writer->closeFiles();
    free(writer);
    session->closeSocket();
    emit stopped(resultCode);
}

int Worker::ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
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

      int i = session->getTransNames().indexOf(transMsg->GetDeviceName());
      if (i == -1) {
          //qDebug() << "Transform ignored:" << transMsg->GetDeviceName();
          return i;
      }
      transTS[i] = ts->GetTimeStamp();
      transMsgList[i] = transMsg;

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

      flushData(imgTS[i]);
      return i;
  }
  qWarning() << "CRC check error!";
  return 0;
}
