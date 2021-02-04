#ifndef EXPORTER_H
#define EXPORTER_H

#include <QList>
#include <QTextStream>

class Exporter
{
    public:
        Exporter(QString okinaFile, QString folderPath);
        int doExport();

    private:
        QString okinaFilePath;
        QString exportFolderPath;

        QList<QString> passengerAttributes = {
            "passenger_id",
            "passenger_file_number",
            "passenger_firstname",
            "passenger_lastname"
        };
        QList<QString> passengerCommentAttributes = {
            "passenger_id",
            "comment"
        };
        QList<QString> passengerTutorAttributes = {
            "tutor_id",
            "passenger_id",
            "tutor_firstname",
            "tutor_lastname"
        };
        QList<QString> passengerTagAttributes = {
            "tag_id",
            "passenger_id",
            "uid"
        };
        QList<QString> passengerTicketAttributes = {
            "ticket_id",
            "passenger_id"
        };
        QList<QString> passengerProfileAttributes = {
            "passenger_id",
            "profil_id"
        };
        QList<QString> schoolAttributes = {
            "school_id",
            "school_name",
            "school_shortname"
        };
        QList<QString> passengerSchoolAttributes = {
            "school_id",
            "passenger_id",
            "internship_type",
            "class_level"
        };
        QList<QString> bookingAttributes = {
            "booking_id",
            "passenger_id",
            "trip_id",
            "origin_stoppoint_id",
            "destination_stoppoint_id"
        };
};

#endif // EXPORTER_H
