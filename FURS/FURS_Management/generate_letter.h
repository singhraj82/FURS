#ifndef GENERATE_LETTER_H
#define GENERATE_LETTER_H

#include <QString>
#include <QCoreApplication>

struct Letter_information
{
    QString sender;
    QString address_street;
    QString address_city;
    QString address_zipcode;
    QString address_state;
    QString document_title;
    QString clerk_name;
    QString date;
    QString role;
    QString camp;
    QString receiver_last_name;
    QString receiver_first_name;
    QString receiver_address_street;
    QString receiver_address_city;
    QString receiver_address_state;
    QString receiver_address_zip_code;
    QString payment_status;
    QString wait_list;

    QString letter_path()
    {
    QString letter_path = QCoreApplication::applicationDirPath() + "/../../letters/" +
            receiver_last_name + "_" +
            receiver_first_name + "_" +
            document_title + ".pdf";

    return letter_path;
    }
};

class Generate_letter
{
public:
    Generate_letter();

    void print_letter(Letter_information letter_information);

protected:
    void print_selection_letter_(Letter_information letter_information);
    void print_rejection_letter_(Letter_information letter_information);
    void failed_payment_letter_(Letter_information letter_information);
    void incomplete_information_letter_(Letter_information letter_information);
    void failed_competiton_letter_(Letter_information letter_information);

};

#endif // GENERATE_LETTER_H
