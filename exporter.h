#ifndef EXPORTER_H
#define EXPORTER_H

#include <QList>
#include <QTextStream>

class Exporter
{
    public:
        Exporter(QString okinaFile, QString folderPath, int schoolYearIndex);
        int doExport();

    private:
        QString okinaFilePath;
        QString exportFolderPath;
        int schoolYearIndex;

        QList<QString> schoolYearStartDates = {
            "20210308",
            "20210902",
            "20220901"
        };
        QList<QString> schoolYearEndDates = {
            "20210706",
            "20220707",
            "20230708"
        };

        QList<QString> passengerAttributes = {
            "passenger_id",
            "passenger_file_number",
            "passenger_firstname",
            "passenger_lastname",
            "passenger_gender_title",
            "passenger_dateofbirth",
            "passenger_gender",
            "passenger_email",
            "passenger_mobile_phone",
            "passenger_home_phone",
            "passenger_address_1",
            "passenger_address_2",
            "passenger_address_3",
            "passenger_address_zipcode",
            "passenger_address_city",
            "passenger_address_country",
            "passenger_picture_name",
            "additional_data"
        };
        QList<QString> passengerCommentAttributes = {
            "passenger_id",
            "comment"
        };
        QList<QString> passengerTutorAttributes = {
            "tutor_id",
            "passenger_id",
            "tutor_firstname",
            "tutor_lastname",
            "tutor_gender_title",
            "tutor_parent_link",
            "tutor_email",
            "tutor_mobile_phone",
            "tutor_home_phone",
            "tutor_work_phone",
            "tutor_dateofbirth",
            "tutor_address_1",
            "tutor_address_2",
            "tutor_address_3",
            "tutor_address_zipcode",
            "tutor_address_city",
            "tutor_address_country"
        };
        QList<QString> passengerTagAttributes = {
            "tag_id",
            "passenger_id",
            "uid",
            "uid_format",
            "delivery_date",
            "validity_end",
            "disabled_date",
            "disabled_cause"
        };
        QList<QString> passengerTicketAttributes = {
            "ticket_id",
            "passenger_id",
            "fare_id",
            "ticket_start_on",
            "ticket_end_on",
            "restrictions"
        };
        QList<QString> profileAttributes = {
            "profil_id",
            "profil_name",
            "profil_start_on",
            "profil_end_on",
            "profil_age_start",
            "profil_age_end",
            "profil_anonymous"
        };
        QList<QString> passengerProfileAttributes = {
            "passenger_id",
            "profil_id",
            "profil_start_on",
            "profil_end_on"
        };
        QList<QString> schoolAttributes = {
            "school_id",
            "school_name",
            "school_shortname",
            "school_address_1",
            "school_address_2",
            "school_address_3",
            "school_address_zipcode",
            "school_address_city",
            "school_address_country",
            "school_email",
            "school_lat",
            "school_lon",
            "school_open_on"
        };
        QList<QString> passengerSchoolAttributes = {
            "passenger_id",
            "school_id",
            "internship_type",
            "class_level"
        };
        QList<QString> bookingAttributes = {
            "booking_id",
            "passenger_id",
            "trip_id",
            "origin_stoppoint_id",
            "destination_stoppoint_id",
            "start_date",
            "end_date",
            "days",
            "calendar_id"
        };

        QMap<QString, QList<QString>> gpfsAttributes = {
            {"passengers", passengerAttributes},
            {"passengerComments", passengerCommentAttributes},
            {"passengerTutors", passengerTutorAttributes},
            {"passengerTags", passengerTagAttributes},
            {"passengerTickets", passengerTicketAttributes},
            {"schools", schoolAttributes},
            {"passengerSchools", passengerSchoolAttributes},
            {"bookings", bookingAttributes}
        };

        QMap<QString, QString> gpfsFilenames = {
            {"passengers", "passengers.txt"},
            {"passengerComments", "passenger_comments.txt"},
            {"passengerTutors", "passenger_tutors.txt"},
            {"passengerTags", "passenger_tags.txt"},
            {"passengerTickets", "passenger_tickets.txt"},
            {"schools", "schools.txt"},
            {"passengerSchools", "passenger_schools.txt"},
            {"bookings", "bookings.txt"}
        };
        QMap<QString, QFile *> gpfsFiles;
        QMap<QString, QTextStream *> gpfsTextStreams;
};

#endif // EXPORTER_H
