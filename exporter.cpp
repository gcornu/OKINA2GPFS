#include "exporter.h"
#include <QDebug>
#include <QTextCodec>

Exporter::Exporter(QString okinaFilePathParam, QString exportFolderPathParam, int schoolYearIndexParam)
{
    okinaFilePath = okinaFilePathParam;
    exportFolderPath = exportFolderPathParam;
    schoolYearIndex = schoolYearIndexParam;
}

int Exporter::doExport() {
    QFile okinaFile(okinaFilePath);
    if (!okinaFile.open(QIODevice::ReadOnly)) {
        throw okinaFile.errorString();
    }

    if (exportFolderPath.length() == 0) {
        throw "Folder not specified";
    }

    // Create export files
    QMapIterator<QString, QString> i(gpfsFilenames);
    QFile *gpfsFile;
    bool openSuccess = true;
    while (i.hasNext()) {
        i.next();
        gpfsFile = new QFile(exportFolderPath + "\\" + i.value());
        openSuccess = openSuccess && gpfsFile->open(QIODevice::WriteOnly);
        gpfsFiles.insert(i.key(), gpfsFile);
    }

    if (openSuccess) {
        // Open textStreams for created files and write first line
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QMapIterator<QString, QFile *> i(gpfsFiles);
        QTextStream *gpfsTextStream;
        while (i.hasNext()) {
            i.next();
            QFile *file = i.value();
            gpfsTextStream = new QTextStream(file);
            gpfsTextStream->setCodec(codec);

            QList<QString> attributes = gpfsAttributes.value(i.key());
            for (int i = 0; i < attributes.length(); i++) {
                *gpfsTextStream << attributes[i];
                (i == attributes.length()-1) ? *gpfsTextStream << endl : *gpfsTextStream << ",";
            }

            gpfsTextStreams.insert(i.key(), gpfsTextStream);
        }

        QList<QString> passengers;
        QList<QString> schools;
        int tagId = 0;

        QList<QByteArray> okinaFileFirstLineByteArray, okinaFileLineByteArray;
        QList<QString> okinaFileFirstLine = {};
        QMap<QString, QString> okinaFileLine;
        QString okinaFileElement;
        QByteArray line = okinaFile.readLine();
        okinaFileFirstLineByteArray = line.split(';');
        for (int i = 0; i < okinaFileFirstLineByteArray.length(); i++) {
            okinaFileFirstLine.push_back(QString::fromLatin1(okinaFileFirstLineByteArray[i]).remove("\""));
        }
        okinaFileFirstLine[okinaFileFirstLine.length()-1].chop(4);

        while (!okinaFile.atEnd()) {
            line = okinaFile.readLine();
            okinaFileLineByteArray = line.split(';');
            for (int i = 0; i < okinaFileLineByteArray.length(); i++) {
                okinaFileElement = QString::fromLatin1(okinaFileLineByteArray[i]).remove("\"");
                if(okinaFileElement == "NULL") {
                    okinaFileElement = "";
                }
                // Insert element in map, removing trailing \r\n if present
                okinaFileLine.insert(okinaFileFirstLine[i], okinaFileElement.remove("\r\n"));
            }

            // Skipping line if passenger_id already exists
            if (passengers.indexOf(okinaFileLine.value(QString("passenger§passenger_id"))) == -1) {
               passengers.push_back(okinaFileLine.value(QString("passenger§passenger_id")));

                // Passengers file
                for (int i = 0; i < passengerAttributes.length(); i++) {
                    *gpfsTextStreams.value("passengers") << okinaFileLine.value(QString("passenger§").append(passengerAttributes[i]), "");
                    (i == passengerAttributes.length()-1) ? *gpfsTextStreams.value("passengers") << endl : *gpfsTextStreams.value("passengers") << ",";
                }

                // Passenger_comments file
                if (okinaFileLine.value(QString("passenger_comments§comment")) != "") {
                    // Add line only if comment is not empty
                    for (int i = 0; i < passengerCommentAttributes.length(); i++) {
                        *gpfsTextStreams.value("passengerComments") << okinaFileLine.value(QString("passenger_comments§").append(passengerCommentAttributes[i]), "");
                        (i == passengerCommentAttributes.length()-1) ? *gpfsTextStreams.value("passengerComments") << endl : *gpfsTextStreams.value("passengerComments") << ",";
                    }
                }


                // Passenger_tutors file
                for (int i = 0; i < passengerTutorAttributes.length(); i++) {
                    *gpfsTextStreams.value("passengerTutors") << okinaFileLine.value(QString("passenger_tutors_1§").append(passengerTutorAttributes[i]), "");
                    (i == passengerTutorAttributes.length()-1) ? *gpfsTextStreams.value("passengerTutors") << endl : *gpfsTextStreams.value("passengerTutors") << ",";
                }

                if (okinaFileLine.value(QString("passenger_tutors_2§").append(passengerTutorAttributes[0]), "") != "") {
                    // Second tutor
                    for(int i = 0; i < passengerTutorAttributes.length(); i++) {
                        *gpfsTextStreams.value("passengerTutors") << okinaFileLine.value(QString("passenger_tutors_2§").append(passengerTutorAttributes[i]), "");
                        (i == passengerTutorAttributes.length()-1) ? *gpfsTextStreams.value("passengerTutors") << endl : *gpfsTextStreams.value("passengerTutors") << ",";
                    }
                }

                // Passenger_tags file
                // Adding data if not present in OKINA file
                if (okinaFileLine.value(QString("passenger_tags§tag_id"), "") == "") {
                    okinaFileLine.insert(QString("passenger_tags§tag_id"), QString::number(tagId));
                    tagId++;
                }
                if (okinaFileLine.value(QString("passenger_tags§uid_format"), "") == "") {
                    okinaFileLine.insert(QString("passenger_tags§uid_format"), "DEC");
                }

                for (int i = 0; i < passengerTagAttributes.length(); i++) {
                    *gpfsTextStreams.value("passengerTags") << okinaFileLine.value(QString("passenger_tags§").append(passengerTagAttributes[i]), "");
                    (i == passengerTagAttributes.length()-1) ? *gpfsTextStreams.value("passengerTags") << endl : *gpfsTextStreams.value("passengerTags") << ",";
                }

                // Passenger_tickets file
                QString trip, originStopPoint, destinationStopPoint;
                QString restrictions = "\"[";
                for (int i=1; i<=9; i++) {
                    if (okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§trip_id"), "") != "") {
                        trip = okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§trip_id"), "");
                        originStopPoint = okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§origin_stoppoint_id"), "");
                        destinationStopPoint = okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§destination_stoppoint_id"), "");

                        restrictions += QByteArray("{\"\"trip\"\":{\"\"is\"\":[\"\"").append(trip).append("\"\"]}, ")
                                           .append("\"\"stoparea\"\":{\"\"is\"\":[\"\"").append(originStopPoint).append("\"\",\"\"").append(destinationStopPoint).append(("\"\"]}}, "));
                    }
                }
                if (restrictions.length() == 1) {
                    restrictions = "";
                } else {
                    restrictions.chop(2);
                    restrictions.append("]\"");
                }
                okinaFileLine.insert(QString("passenger_tickets§restrictions"), restrictions);

                // Adding static data if not present in OKINA file
                if (okinaFileLine.value(QString("passenger_tickets§fare_id"), "") == "") {
                    okinaFileLine.insert(QString("passenger_tickets§fare_id"), "2SCHOOL_SUBSCRIPTION");
                }
                if (okinaFileLine.value(QString("passenger_tickets§ticket_start_on"), "") == "") {
                    okinaFileLine.insert(QString("passenger_tickets§ticket_start_on"), QString(schoolYearStartDates[schoolYearIndex]).append("000000"));
                }
                if (okinaFileLine.value(QString("passenger_tickets§ticket_end_on"), "") == "") {
                    okinaFileLine.insert(QString("passenger_tickets§ticket_end_on"), QString(schoolYearEndDates[schoolYearIndex]).append("235959"));
                }

                for (int i = 0; i < passengerTicketAttributes.length(); i++) {
                    *gpfsTextStreams.value("passengerTickets") << okinaFileLine.value(QString("passenger_tickets§").append(passengerTicketAttributes[i]), "");
                    (i == passengerTicketAttributes.length()-1) ? *gpfsTextStreams.value("passengerTickets") << endl : *gpfsTextStreams.value("passengerTickets") << ",";
                }

                // Schools and passenger_schools file
                QString schoolId = okinaFileLine.value(QString("schools§school_id"), "");
                QString schoolName = okinaFileLine.value(QString("schools§school_name"), "");
                QString schoolShortname = okinaFileLine.value(QString("schools§school_shortname"), "");
                if (schools.indexOf(schoolId) == -1) {
                    schools.push_front(schoolId);
                    for (int i = 0; i < schoolAttributes.length(); i++) {
                        *gpfsTextStreams.value("schools") << okinaFileLine.value(QString("schools§").append(schoolAttributes[i]), "");
                        (i == schoolAttributes.length()-1) ? *gpfsTextStreams.value("schools") << endl : *gpfsTextStreams.value("schools") << ",";
                    }
                }

                for (int i = 0; i < passengerSchoolAttributes.length(); i++) {
                    *gpfsTextStreams.value("passengerSchools") << okinaFileLine.value(QString("passenger_schools§").append(passengerSchoolAttributes[i]), "");
                    (i == passengerSchoolAttributes.length()-1) ? *gpfsTextStreams.value("passengerSchools") << endl : *gpfsTextStreams.value("passengerSchools") << ",";
                }

                // Bookings file
                for (int i=1; i<=9; i++) {
                    if (okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§booking_id"), "") != "") {
                        // Adding static data if not present in OKINA file
                        if (okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§start_date"), "") == "") {
                            okinaFileLine.insert(QString("bookings_").append(QString::number(i)).append("§start_date"), schoolYearStartDates[schoolYearIndex]);
                        }
                        if (okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§end_date"), "") == "") {
                            okinaFileLine.insert(QString("bookings_").append(QString::number(i)).append("§end_date"), schoolYearEndDates[schoolYearIndex]);
                        }
                        if (okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§days"), "") == "") {
                            okinaFileLine.insert(QString("bookings_").append(QString::number(i)).append("§days"), "1111100");
                        }

                        for (int j = 0; j < bookingAttributes.length(); j++) {
                            *gpfsTextStreams.value("bookings") << okinaFileLine.value(QString("bookings_").append(QString::number(i)).append("§").append(bookingAttributes[j]), "");
                            (j == bookingAttributes.length()-1) ? *gpfsTextStreams.value("bookings") << endl : *gpfsTextStreams.value("bookings") << ",";
                        }
                    }
                }
            }
        }

    } else {
        throw "Cannot create files";
    }

    return 0;
}
