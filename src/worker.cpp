#include <QDebug>
#include <QMatrix4x4>
#include <QVector4D>
#include <QObject>
#include <QKeyEvent>
#include <QApplication>
#include "worker.h"
#include "image.h"
#include "sessionparams.h"
#include "igtlinkclient.h"
#include "imageprocessor.h"
#include "sessionnamegenerator.h"

int cnt=0;
QMatrix4x4 transf;

const QPoint fullOrigin = QPoint(187, 84);

Worker::Worker(SessionParams * conn)
{
    session = conn;
    frozenImageStored = false;
    frozenLastStatus = false;
    lastStoredTS = 0;

//    installEventFilter(this);
}

Worker::~Worker() {
}

const QString& Worker::getCurrentState()
{
    return currentState;
}

const QString& Worker::setState(const QString &state)
{
    if (state != currentState) {
        currentState = state;
        emit imageReceived(NULL, QSize(), currentState);
    }
    return currentState;
}

const QString& Worker::updateState(bool isFrozen, bool isCropped)
{
    QString state;

    if (isFrozen) {
       state = "FROZEN";
    } else if (isCropped) {
       state = "CROPPED";
    } else {
       state = "OK";
    }
    return setState(state);
}

void Worker::writeAndNotify(char * imgBuffer, QSize dimensions, bool isFrozen)
{
    qDebug() << "write and notify 1";
    if (!isFrozen || !frozenImageStored) {
        writer->writeImage(imgBuffer, dimensions);
        qDebug() << "write and notify 2: image";
    }

}

void Worker::notify(char *imgBuffer, QSize dimensions, bool isFrozen)
{
    if (!isFrozen || !frozenImageStored) {
        frozenImageStored = isFrozen;
        emit imageReceived(imgBuffer, dimensions, getCurrentState());
    }
}

void Worker::writeTransformCropped(const igtl::TransformMessage::Pointer &transMsg)
{
    qDebug() << "write transform";
    int i = session->getTransNames().indexOf(transMsg->GetDeviceName());
    if (i==0) {
        igtl::TransformMessage::Pointer tMsg;
        tMsg = igtl::TransformMessage::New();
        igtl::Matrix4x4 m;
        transMsg->GetMatrix(m);
        tMsg->SetMatrix(m);
        QString s = transMsg->GetDeviceName();
        int ind = s.indexOf("To");
        igtl::Matrix4x4 matrix = {{1, 0, 0, 0},{0, 1, 0, 0},{0, 0, 1, 0},{0, 0, 0, 1}};
        matrix[0][3] = session->getCrop().x();
        matrix[1][3] = session->getCrop().y();


        // Write original transformation with modified name Full<Name>
        tMsg->SetDeviceName(QString("Full").append(s).toLatin1());
        writer->writeTransform(tMsg);

        // Write croping trans.
        tMsg->SetDeviceName(QString("ToFull").prepend(s.left(ind)).append(s.left(ind)).toLatin1());
        tMsg->SetMatrix(matrix);
        writer->writeTransform(tMsg);
    } else
        writer->writeTransform(transMsg);
}

void Worker::flushData(double ts)
{   
    // check if the time stamps are equal then save data to the file
    if (imgTS.count(ts) + transTS.count(ts) == imgTS.size() + transTS.size()) {
        // there we control maximal frame rate
        if (lastStoredTS != 0 && (lastStoredTS + 1.0 / session->getFps() >= ts)) {
            // skipping frames
            return;
        }


        lastStoredTS = ts;
        // To have actual fps  equal to session->getFps() we must do it as given below
        // lastStoredTS = lastStoredTS + 1.0 / session->getFps();
        // if ( (lastStoredTS + 1.0 / session->getFps() ) < ts)
        //    lastStoredTS = ts;

        //qDebug() << "save";
        if (imgTS.count() == 0) // !!! At least one image
             return;

        int size[3];
        char * imgBuffer;
        bool isCropped;
        imgMsgList[0]->GetDimensions(size);
        QSize dimensions(size[0], size[1]);
        QSize area;

        bool isFrozen = ImageProcessor::isFrozen((char *)imgMsgList[0]->GetScalarPointer(), dimensions, session->getFreeze());

        if (session->shouldCrop(dimensions)) {
            isCropped = true;
            area = session->getCrop().size();
        }
        else {
            isCropped = false;
            area = dimensions;
        }

        updateState(isFrozen,isCropped);

//        if (isFrozen && !frozenLastStatus &&
//                writer->getImageCounter()>0) {
        if (isFrozen && !frozenLastStatus &&
                imageCounter>0) {
            cnt++;
            Image probe = Image(pos, ts, FROZEN);
            emit imgPosition(probe);
//            emit position(pos);
        }
        frozenLastStatus = isFrozen;

        if (cnt>2 && !isFrozen)
            emit imgPosition(Image(pos, ts, UNFROZEN));
//            emit position(pos);

        if (isFrozen && frozenImageStored) {
            // frozen image already stored
            return;
        }

        if (isCropped) {
            imgBuffer = (char *)malloc(session->getCrop().width() * session->getCrop().height() * sizeof(uchar));
            ImageProcessor::cropImage((char *)imgMsgList[0]->GetScalarPointer(), dimensions, session->getCrop(), imgBuffer);
        } else {
            imgBuffer = (char *) imgMsgList[0]->GetScalarPointer();
        }
        imageCounter++;

//        if (isFrozen && writer->getImageCounter()<1) {
        if (isFrozen && imageCounter) {
            frozenImageStored = isFrozen;
            emit imageReceived(imgBuffer, area, getCurrentState());
            return;
        }
        if (isFrozen) {
            writer->writeFrozenIndex();
            writer->writeFrozenIndexAndPos(pos.map(QVector4D(0, 0, 0, 1)));
        }

        notify(imgBuffer, area, isFrozen);
        bool s;
        RecordLock.lockForRead();
        s = saveVideo;
        RecordLock.unlock();
        if (s) {
            writer->writeHeader(imgMsgList[0]);
            writer->startSequence(ts);
            for (int i = 0; i < transTS.size(); ++i) {
                writeTransformCropped(transMsgList[i]);
                //qDebug() << transMsgList[i]->GetDeviceName();
            }

            writeAndNotify(imgBuffer, area, isFrozen);
            writer->closeSequence();
        }
    }
}

void Worker::startRecord()
{
    qDebug() << "worker: start record";
    RecordLock.lockForWrite();
    saveVideo = true;
    RecordLock.unlock();
}

void Worker::stopRecord()
{
    qDebug() << "worker: stop record";
    RecordLock.lockForWrite();
    saveVideo = false;
    RecordLock.unlock();
//    emit stopped(IGTLinkClient::UserInterrupt);
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
    qDebug() << "worker: start";
    int resultCode;
    currentState="";
    frozenImageStored = false;
    // Create a message buffer to receive header
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();

    Lock.lockForWrite();
    Terminate = false;
    Lock.unlock();

    resultCode = session->openSocket();
//    if (resultCode != 0) {
//        qWarning() << "Connection to server failed. Error code: " << resultCode;
//        emit stopped(IGTLinkClient::SocketOpenError);
//        return;
//    } else {
//        qDebug() << "Socket opened. Reading data loop ...";
//    }

    bool t;
    while (resultCode != 0) {
        Lock.lockForRead();
        t = Terminate;
        Lock.unlock();
        if (!t)
            resultCode = session->openSocket();
    }
    qDebug() << "Socket opened. Reading data loop ...";

    //------------------------------------------------------------
    // Allocate a time stamp
    igtl::TimeStamp::Pointer ts;
    ts = igtl::TimeStamp::New();

    writer = new Writer(session->getOutDir(), session->getChunkSize());
    writer->createOutDir();

    writer->openHeaderFile();


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

      probePos(transMsg, transTS[i]);

      flushData(transTS[i]);
      emit transformReceived(transMsg);
      return i;
  }
  qWarning() << "CRC check error!";
  return 0;
}

void Worker::probePos(const igtl::TransformMessage::Pointer &transMsg, double ts)
{
    int i = session->getTransNames().indexOf(transMsg->GetDeviceName());
    igtl::Matrix4x4 m;
    transMsg->GetMatrix(m);
    QString key = transMsg->GetDeviceName();
    transforms[key] = QMatrix4x4((const float *)m);
    transforms[key].optimize();


    // check equal time stamps
    if (imgTS.count(ts) + transTS.count(ts) == imgTS.size() + transTS.size()) {
        pos = transforms["ReferenceToTracker"].inverted()*transforms["ProbeToTracker"]
                *transforms["ImageToProbe"];
    }

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
      // qDebug() << 1/(ts->GetTimeStamp()-imgTS[i]);
      imgTS[i] = ts->GetTimeStamp();
      imgMsgList[i] = imgMsg;

      // check if we have received all requested transformations
      // if not emit message "OUT OF RANGE"
      if ( transTS.size() > 0 )
          if ( transTS.count(transTS[0]) < transTS.size() )
              setState("OUT OF RANGE");

      flushData(imgTS[i]);
      return i;
  }
  qWarning() << "CRC check error!";
  return 0;
}

