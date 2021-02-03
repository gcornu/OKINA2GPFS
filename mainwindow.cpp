#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle(tr("Export GPFS"));

    this->resize(400,150);

    QWidget *widget = new QWidget();
    this->setCentralWidget(widget);

    QGridLayout * layout = new QGridLayout;

    QLabel * okinaFileLabel = new QLabel(tr("Fichier CSV OKINA :"));
    okinaFileLineEdit = new QLineEdit(okinaFile);
    connect(okinaFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setOkinaFile(QString)));
    QPushButton * chooseFileButton = new QPushButton(tr("Parcourir..."));
    connect(chooseFileButton, SIGNAL(clicked()), this, SLOT(selectOkinaFile()));
    layout->addWidget(okinaFileLabel, 0, 0);
    layout->addWidget(okinaFileLineEdit, 0, 1);
    layout->addWidget(chooseFileButton, 0, 2);

    QLabel * folderPathLabel = new QLabel(tr("Dossier export GPFS :"));
    folderPathLineEdit = new QLineEdit(folderPath);
    connect(folderPathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFolderPath(QString)));
    QPushButton * chooseFolderButton = new QPushButton(tr("Parcourir..."));
    connect(chooseFolderButton, SIGNAL(clicked()), this, SLOT(selectFolderPath()));
    layout->addWidget(folderPathLabel, 1, 0);
    layout->addWidget(folderPathLineEdit, 1, 1);
    layout->addWidget(chooseFolderButton, 1, 2);

    QPushButton * exportButton = new QPushButton(tr("Export GPFS"));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportGPFS()));
    layout->addWidget(exportButton, 2, 2);

    widget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectOkinaFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choisir fichier CSV OKINA"),
                                                 "C:\\",
                                                 tr("Fichier CSV (*.csv)"));
    okinaFile = fileName;
    okinaFileLineEdit->setText(okinaFile);
}

void MainWindow::selectFolderPath()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Choisir dossier export"),
                                                 "C:\\",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    folderPath = folderName;
    folderPathLineEdit->setText(folderPath);
}

void MainWindow::exportGPFS()
{
    QFile okinaQFile(okinaFile);
    if (!okinaQFile.open(QIODevice::ReadOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de l'export");
        msgBox.setDetailedText(okinaQFile.errorString());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if(folderPath.length() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de l'export");
        msgBox.setDetailedText("Folder not specified");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    // Create export files
    QString passengersFilename = folderPath + "\\passengers.txt";
    QFile passengersFile(passengersFilename);
    QString passengerTutorsFilename = folderPath + "\\passengers_tutors.txt";
    QFile passengerTutorsFile(passengerTutorsFilename);
    QString passengerTagsFilename = folderPath + "\\passenger_tags.txt";
    QFile passengerTagsFile(passengerTagsFilename);
    QString passengerTicketsFilename = folderPath + "\\passenger_tickets.txt";
    QFile passengerTicketsFile(passengerTicketsFilename);
    QString schoolsFilename = folderPath + "\\schools.txt";
    QFile schoolsFile(schoolsFilename);
    QString passengerSchoolsFilename = folderPath + "\\passenger_schools.txt";
    QFile passengerSchoolsFile(passengerSchoolsFilename);
    QString bookingsFilename = folderPath + "\\bookings.txt";
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

        passengersStream << "passenger_id,passenger_file_number,passenger_firstname,passenger_lastname,passenger_dateofbirth,passenger_email,passenger_mobile_phone,passenger_address_1,passenger_address_2,passenger_address_zipcode,passenger_address_city" << endl;
        passengerTutorsStream << "tutor_id,passenger_id,tutor_firstname,tutor_lastname,tutor_gender_title,tutor_email,tutor_mobile_phone,tutor_home_phone,tutor_address_1,tutor_address_2,tutor_address_zipcode,tutor_address_city" << endl;
        passengerTagsStream << "tag_id,passenger_id,uid,uid_format,delivery_date,validity_end,disabled_date,disabled_cause" << endl;
        passengerTicketsStream << "ticket_id,passenger_id,fare_id,ticket_start_on,ticket_end_on,restrictions" << endl;
        schoolsStream << "school_id,school_name,school_shortname,school_address_zipcode,school_address_city" << endl;
        passengerSchoolsStream << "passenger_id,school_id,internship_type,class_level" << endl;
        bookingsStream << "booking_id,passenger_id,trip_id,origin_stoppoint_id,destination_stoppoint_id,start_date,end_date,days" << endl;

        QList<QByteArray> schools;

        QList<QByteArray> okinaFileFirstLine, okinaFileLine;
        QByteArray line = okinaQFile.readLine();
        okinaFileFirstLine = okinaFileLine = line.split(',');
        while (!okinaQFile.atEnd()) {
            line = okinaQFile.readLine();
            okinaFileLine = line.split(',');

            // Passengers file
            QByteArray passengerId = okinaFileLine[okinaFileFirstLine.indexOf("id")];
            QByteArray passengerFileNumber = okinaFileLine[okinaFileFirstLine.indexOf("numero_abonne")];
            QByteArray passengerFirstname = okinaFileLine[okinaFileFirstLine.indexOf("prenom")];
            QByteArray passengerLastname = okinaFileLine[okinaFileFirstLine.indexOf("nom")];
            QByteArray passengerDateOfBirth = okinaFileLine[okinaFileFirstLine.indexOf("date_de_naissance")];
            QByteArray passengerEmail = okinaFileLine[okinaFileFirstLine.indexOf("email")];
            QByteArray passengerMobilePhone = okinaFileLine[okinaFileFirstLine.indexOf("telephone")];
            QByteArray passengerAddress1 = okinaFileLine[okinaFileFirstLine.indexOf("adresse_1")];
            QByteArray passengerAddress2 = okinaFileLine[okinaFileFirstLine.indexOf("adresse_complement_1")];
            QByteArray passengerAddressZipcode = okinaFileLine[okinaFileFirstLine.indexOf("code_postal_1")];
            QByteArray passengerAddressCity = okinaFileLine[okinaFileFirstLine.indexOf("ville_1")];

            passengersStream << passengerId << ","
                             << passengerFileNumber << ","
                             << passengerFirstname << ","
                             << passengerLastname << ","
                             << passengerDateOfBirth << ","
                             << passengerEmail << ","
                             << passengerMobilePhone << ","
                             << passengerAddress1 << ","
                             << passengerAddress2 << ","
                             << passengerAddressZipcode << ","
                             << passengerAddressCity << endl;

            // Passenger_tutors file
            QByteArray tutorId = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_id")];
            QByteArray tutorFirstname = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_prenom")];
            QByteArray tutorLastname = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_nom")];
            QByteArray tutorGenderTitle = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_civilite")];
            QByteArray tutorEmail = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_email")];
            QByteArray tutorMobilePhone = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_telephone_1")];
            QByteArray tutorHomePhone = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_telephone_2")];
            QByteArray tutorAddress1 = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_adresse")];
            QByteArray tutorAddress2 = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_adresse_complement")];
            QByteArray tutorAddressZipcode = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_code_postal")];
            QByteArray tutorAddressCity = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_1_city")];

            passengerTutorsStream << tutorId << ","
                                  << passengerId << ","
                                  << tutorFirstname << ","
                                  << tutorLastname << ","
                                  << tutorGenderTitle << ","
                                  << tutorEmail << ","
                                  << tutorMobilePhone << ","
                                  << tutorHomePhone << ","
                                  << tutorAddress1 << ","
                                  << tutorAddress2 << ","
                                  << tutorAddressZipcode << ","
                                  << tutorAddressCity << endl;

            if(okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_id")] != "") {
                // Second tutor
                QByteArray tutorId = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_id")];
                QByteArray tutorFirstname = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_prenom")];
                QByteArray tutorLastname = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_nom")];
                QByteArray tutorGenderTitle = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_civilite")];
                QByteArray tutorEmail = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_email")];
                QByteArray tutorMobilePhone = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_telephone_1")];
                QByteArray tutorHomePhone = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_telephone_2")];
                QByteArray tutorAddress1 = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_adresse")];
                QByteArray tutorAddress2 = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_adresse_complement")];
                QByteArray tutorAddressZipcode = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_code_postal")];
                QByteArray tutorAddressCity = okinaFileLine[okinaFileFirstLine.indexOf("representant_legal_2_city")];

                passengerTutorsStream << tutorId << ","
                                      << passengerId << ","
                                      << tutorFirstname << ","
                                      << tutorLastname << ","
                                      << tutorGenderTitle << ","
                                      << tutorEmail << ","
                                      << tutorMobilePhone << ","
                                      << tutorHomePhone << ","
                                      << tutorAddress1 << ","
                                      << tutorAddress2 << ","
                                      << tutorAddressZipcode << ","
                                      << tutorAddressCity << endl;
            }

            // Passenger_tags file
            QByteArray uidKorrigo = okinaFileLine[okinaFileFirstLine.indexOf("numero_korrigo")];

            passengerTagsStream << uidKorrigo << ","
                                << passengerId << ","
                                << uidKorrigo << ","
                                << "DEC" << ","
                                << "20210308" << ","
                                << "20280307" << ","
                                << "" << ","
                                << "" << endl;

            // Passenger_tickets file
            QByteArray trip, originStopPoint, destinationStopPoint;
            QByteArray restrictions = "[";
            for (int i=1; i<=10; i++) {
                if (okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_id"))] != "") {
                    trip = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_nom"))];
                    originStopPoint = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_point_montee"))];
                    destinationStopPoint = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_point_arrivee"))];

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

            passengerTicketsStream << passengerId << ","
                                   << passengerId << ","
                                   << "2SCHOOL_SUBSCRIPTION" << ","
                                   << "20210308000000" << ","
                                   << "20210706235959" << ","
                                   << restrictions << endl;

            // Schools and passenger_schools file
            QByteArray schoolName = okinaFileLine[okinaFileFirstLine.indexOf("etablissement")];
            QByteArray schoolAddressZipcode = okinaFileLine[okinaFileFirstLine.indexOf("dpt_ets")];
            QByteArray schoolAddressCity = okinaFileLine[okinaFileFirstLine.indexOf("ville_etablissement")];
            if(schools.indexOf(schoolName) == -1) {
                schools.push_front(schoolName);
                schoolsStream << QString::number(schools.indexOf(schoolName)) << ","
                              << schoolName << ","
                              << schoolName << ","
                              << schoolAddressZipcode << ","
                              << schoolAddressCity << endl;
            }

            QByteArray internshipType = okinaFileLine[okinaFileFirstLine.indexOf("qualite")];
            QByteArray classLevel = okinaFileLine[okinaFileFirstLine.indexOf("classe")];

            passengerSchoolsStream << passengerId << ","
                                   << QString::number(schools.indexOf(schoolName)) << ","
                                   << internshipType << ","
                                   << classLevel << endl;

            // Bookings file
            QByteArray bookingId, tripId, originStopPointId, destinationStopPointId;
            for (int i=1; i<=10; i++) {
                if (okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_id"))] != "") {
                    bookingId = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_id"))];
                    tripId = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_nom"))];
                    originStopPointId = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_point_montee"))];
                    destinationStopPointId = okinaFileLine[okinaFileFirstLine.indexOf(QByteArray("abonnement_").append(QString::number(i)).append("_point_arrivee"))];

                    bookingsStream << bookingId << ","
                                   << passengerId << ","
                                   << tripId << ","
                                   << originStopPointId << ","
                                   << destinationStopPointId << ","
                                   << "20210308" << ","
                                   << "20210706" << ","
                                   << "1111100" << endl;
                }
            }
        }

        QMessageBox msgBox;
        msgBox.setText("Export terminé");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        close();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de l'export");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        close();
    }


}
