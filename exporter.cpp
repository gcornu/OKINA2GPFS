#include "exporter.h"
#include <QDebug>

Exporter::Exporter(QString okinaFilePathParam, QString exportFolderPathParam)
{
    okinaFilePath = okinaFilePathParam;
    exportFolderPath = exportFolderPathParam;
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
    QString passengersFilename = exportFolderPath + "\\passengers.txt";
    QFile passengersFile(passengersFilename);
    QString passengerTutorsFilename = exportFolderPath + "\\passengers_tutors.txt";
    QFile passengerTutorsFile(passengerTutorsFilename);
    QString passengerTagsFilename = exportFolderPath + "\\passenger_tags.txt";
    QFile passengerTagsFile(passengerTagsFilename);
    QString passengerTicketsFilename = exportFolderPath + "\\passenger_tickets.txt";
    QFile passengerTicketsFile(passengerTicketsFilename);
    QString schoolsFilename = exportFolderPath + "\\schools.txt";
    QFile schoolsFile(schoolsFilename);
    QString passengerSchoolsFilename = exportFolderPath + "\\passenger_schools.txt";
    QFile passengerSchoolsFile(passengerSchoolsFilename);
    QString bookingsFilename = exportFolderPath + "\\bookings.txt";
    QFile bookingsFile(bookingsFilename);
    if (passengersFile.open(QIODevice::ReadWrite)
            && passengerTutorsFile.open(QIODevice::ReadWrite)
            && passengerTagsFile.open(QIODevice::ReadWrite)
            && passengerTicketsFile.open(QIODevice::ReadWrite)
            && schoolsFile.open(QIODevice::ReadWrite)
            && passengerSchoolsFile.open(QIODevice::ReadWrite)
            && bookingsFile.open(QIODevice::ReadWrite)) {
        QTextStream passengersStream(&passengersFile);
        QTextStream passengerTutorsStream(&passengerTutorsFile);
        QTextStream passengerTagsStream(&passengerTagsFile);
        QTextStream passengerTicketsStream(&passengerTicketsFile);
        QTextStream schoolsStream(&schoolsFile);
        QTextStream passengerSchoolsStream(&passengerSchoolsFile);
        QTextStream bookingsStream(&bookingsFile);

        passengersStream << "passenger_id,passenger_file_number,passenger_firstname,passenger_lastname" << endl;
        passengerTutorsStream << "tutor_id,passenger_id,tutor_firstname,tutor_lastname" << endl;
        passengerTagsStream << "tag_id,passenger_id,uid,uid_format,delivery_date,validity_end,disabled_date,disabled_cause" << endl;
        passengerTicketsStream << "ticket_id,passenger_id,fare_id,ticket_start_on,ticket_end_on,restrictions" << endl;
        schoolsStream << "school_id,school_name,school_shortname" << endl;
        passengerSchoolsStream << "passenger_id,school_id,internship_type,class_level" << endl;
        bookingsStream << "booking_id,passenger_id,trip_id,origin_stoppoint_id,destination_stoppoint_id,start_date,end_date,days" << endl;

        QList<QString> schools;

        QList<QByteArray> okinaFileFirstLineByteArray, okinaFileLineByteArray;
        QList<QString> okinaFileFirstLine = {}, okinaFileLine = {};
        QByteArray line = okinaFile.readLine();
        okinaFileFirstLineByteArray = line.split(';');
        for( int i = 0; i < okinaFileFirstLineByteArray.length(); i++) {
            okinaFileFirstLine.push_back(QString::fromLatin1(okinaFileFirstLineByteArray[i]));
        }
        okinaFileFirstLine[okinaFileFirstLine.length()-1].chop(4);

        while(!okinaFile.atEnd()) {
            line = okinaFile.readLine();
            okinaFileLineByteArray = line.split(';');
            okinaFileLine = {};
            for( int i = 0; i < okinaFileLineByteArray.length(); i++) {
                okinaFileLine.push_back(QString::fromLatin1(okinaFileLineByteArray[i]));
            }
            okinaFileLine[okinaFileLine.length()-1].chop(4);

            // Passengers file
            for(int i = 0; i < passengerAttributes.length(); i++) {
                passengersStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger§").append(passengerAttributes[i]))];
                if(i < passengerAttributes.length()-1) {
                    passengersStream << ",";
                } else {
                    passengersStream << endl;
                }
            }

            // Passenger_tutors file
            for(int i = 0; i < passengerTutorAttributes.length(); i++) {
                passengerTutorsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_tutors_1§").append(passengerTutorAttributes[i]))];
                if(i < passengerTutorAttributes.length()-1) {
                    passengerTutorsStream << ",";
                } else {
                    passengerTutorsStream << endl;
                }
            }

            if(okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_tutors_2§").append(passengerTutorAttributes[0]))] != "NULL") {
                // Second tutor
                for(int i = 0; i < passengerTutorAttributes.length(); i++) {
                    passengerTutorsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_tutors_2§").append(passengerTutorAttributes[i]))];
                    if(i < passengerTutorAttributes.length()-1) {
                        passengerTutorsStream << ",";
                    } else {
                        passengerTutorsStream << endl;
                    }
                }
            }

            // Passenger_tags file
            for(int i = 0; i < passengerTagAttributes.length(); i++) {
                passengerTagsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_tags§").append(passengerTagAttributes[i]))];
                passengerTagsStream << ",";
                if(i == passengerTagAttributes.length()-1) {
                    passengerTagsStream << "DEC" << ","
                                        << "20210308" << ","
                                        << "20280307" << ","
                                        << "" << ","
                                        << "" << endl;
                }
            }

            // Passenger_tickets file
            QString trip, originStopPoint, destinationStopPoint;
            QString restrictions = "[";
            for(int i=1; i<=9; i++) {
                if (okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§trip_id"))] != "NULL") {
                    trip = okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§trip_id"))];
                    originStopPoint = okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§origin_stoppoint_id"))];
                    destinationStopPoint = okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§destination_stoppoint_id"))];

                    restrictions += QByteArray("{\"line\":{\"is\":[\"").append(trip).append("\"]}, ")
                                       .append("\"stoparea\":{\"is\":[\"").append(originStopPoint).append("\",\"").append(destinationStopPoint).append(("\"]}}, "));
                }
            }
            if(restrictions.length() == 1) {
                restrictions = "";
            } else {
                restrictions.chop(2);
                restrictions.append("]");
            }

            for(int i = 0; i < passengerTicketAttributes.length(); i++) {
                passengerTicketsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_tickets§").append(passengerTicketAttributes[i]))];
                passengerTicketsStream << ",";
                if(i == passengerTicketAttributes.length()-1) {
                    passengerTicketsStream << "2SCHOOL_SUBSCRIPTION" << ","
                                           << "20210308000000" << ","
                                           << "20210706235959" << ","
                                           << restrictions << endl;
                }
            }

            // Schools and passenger_schools file
            QString schoolId = okinaFileLine[okinaFileFirstLine.indexOf(QString("schools§school_id"))];
            QString schoolName = okinaFileLine[okinaFileFirstLine.indexOf(QString("schools§school_name"))];
            QString schoolShortname = okinaFileLine[okinaFileFirstLine.indexOf(QString("schools§school_shortname"))];
            if(schools.indexOf(schoolId) == -1) {
                schools.push_front(schoolId);
                for(int i = 0; i < schoolAttributes.length(); i++) {
                    schoolsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("schools§").append(schoolAttributes[i]))];
                    if(i < schoolAttributes.length()-1) {
                        schoolsStream << ",";
                    } else {
                        schoolsStream << endl;
                    }
                }
            }

            for(int i = 0; i < passengerSchoolAttributes.length(); i++) {
                passengerSchoolsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("passenger_schools§").append(passengerSchoolAttributes[i]))];
                if(i < passengerSchoolAttributes.length()-1) {
                    passengerSchoolsStream << ",";
                } else {
                    passengerSchoolsStream << endl;
                }
            }

            // Bookings file
            for (int i=1; i<=9; i++) {
                if (okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§trip_id"))] != "NULL") {
                    for(int j = 0; j < bookingAttributes.length(); j++) {
                        bookingsStream << okinaFileLine[okinaFileFirstLine.indexOf(QString("bookings_").append(QString::number(i)).append("§").append(bookingAttributes[j]))];
                        bookingsStream << ",";
                        if(j == bookingAttributes.length()-1) {
                            bookingsStream << "20210308" << ","
                                           << "20210706" << ","
                                           << "1111100" << endl;
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
