#include "generate_letter.h"
#include <QPrinter>
#include <QTextDocument>

Generate_letter::Generate_letter()
{

}

void Generate_letter::print_letter(Letter_information letter_information)
{
   if (letter_information.document_title == "Accepted")
   {
       print_selection_letter_(letter_information);
   }
   else
   {
      print_rejection_letter_(letter_information);
   }
}

void Generate_letter::print_selection_letter_(Letter_information letter_information)
{
    QString html =
        "<div align=right>"
            + letter_information.receiver_first_name + " " + letter_information.receiver_last_name + "<br>"
            + letter_information.receiver_address_street + "<br>"
            + letter_information.receiver_address_city + "<br>"
            + letter_information.receiver_address_zip_code + "<br>"
            + letter_information.receiver_address_state + "<br>"
            + letter_information.date +
        "</div>"
        "<div align=left>"
           + letter_information.sender + "<br>"
           + letter_information.address_street + "<br>"
           + letter_information.address_city + "<br>"
           + letter_information.address_zipcode + "<br>"
           + letter_information.address_state + "<br>"
        "</div>"
        "<div align=left>"
           + "Dear " + letter_information.receiver_last_name + ","
        "</div>"
        "<h1 align=center>" + letter_information.document_title + "</h1>"
        "<p align=justify>"
           "We at Futre Rock Stars are glad to inform you that based on the review of your application you have been selected for the role of " + letter_information.role +
           " at the upcoming camp in the week of " + letter_information.camp + ". Along with the letter this packet includes all the checkin formailities and things that you need to bring "
           "when you report at the camp. Please note that if you did not bring all the required items you will not be allowed to attend the camp. "
           "Once again we congratulate on your selection and will forward to meeting you at the camp and hope that you will have fantastic time."
        "</p>"
        "<div align=right>"
        "Sincerely<br>"
        + letter_information.clerk_name +
        "</div>";

        QTextDocument document;
        document.setHtml(html);

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(letter_information.letter_path());
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));

        document.print(&printer);
}

void Generate_letter::print_rejection_letter_(Letter_information letter_information)
{
    if (letter_information.document_title == "Rejected_incomplete")
    {
        incomplete_information_letter_(letter_information);
    }
    else if (letter_information.document_title == "Rejected_failed_payment")
    {
        failed_payment_letter_(letter_information);
    }
    else if (letter_information.document_title == "Rejected")
    {
        failed_competiton_letter_(letter_information);
    }
    else
    {

    }
}

void Generate_letter::failed_payment_letter_(Letter_information letter_information)
{
    QString html =
        "<div align=right>"
            + letter_information.receiver_first_name + " " + letter_information.receiver_last_name + "<br>"
            + letter_information.receiver_address_street + "<br>"
            + letter_information.receiver_address_city + "<br>"
            + letter_information.receiver_address_zip_code + "<br>"
            + letter_information.receiver_address_state + "<br>"
            + letter_information.date +
        "</div>"
        "<div align=left>"
           + letter_information.sender + "<br>"
           + letter_information.address_street + "<br>"
           + letter_information.address_city + "<br>"
           + letter_information.address_zipcode + "<br>"
           + letter_information.address_state + "<br>"
        "</div>"
        "<div align=left>"
           + "Dear " + letter_information.receiver_last_name + ","
        "</div>"
        "<h1 align=center>" + letter_information.document_title + "</h1>"
        "<p align=justify>"
           "We at Futre Rock Stars are sad to inform you that you have not been selected for the role of " + letter_information.role +
           "at the upcoming camp in the week of " + letter_information.camp + "  since your payment failed to process. We would encourage you to please review the "
           "payment information before submitting the application. You can still apply for our upcoming camps. "
           "We would be glad to hear from you soon."
        "</p>"
        "<div align=right>"
        "sincerly<br>"
        + letter_information.clerk_name +
        "</div>";

        QTextDocument document;
        document.setHtml(html);

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(letter_information.letter_path());
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));

        document.print(&printer);
}

void Generate_letter::incomplete_information_letter_(Letter_information letter_information)
{
    QString html =
        "<div align=right>"
            + letter_information.receiver_first_name + " " + letter_information.receiver_last_name + "<br>"
            + letter_information.receiver_address_street + "<br>"
            + letter_information.receiver_address_city + "<br>"
            + letter_information.receiver_address_zip_code + "<br>"
            + letter_information.receiver_address_state + "<br>"
            + letter_information.date +
        "</div>"
        "<div align=left>"
           + letter_information.sender + "<br>"
           + letter_information.address_street + "<br>"
           + letter_information.address_city + "<br>"
           + letter_information.address_zipcode + "<br>"
           + letter_information.address_state + "<br>"
        "</div>"
        "<div align=left>"
           + "Dear " + letter_information.receiver_last_name + ","
        "</div>"
        "<h1 align=center>" + letter_information.document_title + "</h1>"
        "<p align=justify>"
           "We at Futre Rock Stars are sad to inform you that you have not been selected for the role of " + letter_information.role +
           "at the upcoming camp in the week of " + letter_information.camp + "  since your application is incomplete. We would encourage you to please review the application "
           "requirements before submitting the information. We will refund your fee. You can still apply for our upcoming camps. "
           "We would be glad to hear from you soon."
        "</p>"
        "<div align=right>"
        "sincerly<br>"
        + letter_information.clerk_name +
        "</div>";

        QTextDocument document;
        document.setHtml(html);

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(letter_information.letter_path());
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));

        document.print(&printer);
}

void Generate_letter::failed_competiton_letter_(Letter_information letter_information)
{
    QString html =
        "<div align=right>"
            + letter_information.receiver_first_name + " " + letter_information.receiver_last_name + "<br>"
            + letter_information.receiver_address_street + "<br>"
            + letter_information.receiver_address_city + "<br>"
            + letter_information.receiver_address_zip_code + "<br>"
            + letter_information.receiver_address_state + "<br>"
            + letter_information.date +
        "</div>"
        "<div align=left>"
           + letter_information.sender + "<br>"
           + letter_information.address_street + "<br>"
           + letter_information.address_city + "<br>"
           + letter_information.address_zipcode + "<br>"
           + letter_information.address_state + "<br>"
        "</div>"
        "<div align=left>"
           + "Dear " + letter_information.receiver_last_name + ","
        "</div>"
        "<h1 align=center>" + letter_information.document_title + "</h1>"
        "<p align=justify>"
           "We at Futre Rock Stars are sad to inform you that based on the review of your application you have not been selected for the role of " + letter_information.role +
           "at the upcoming camp in the week of " + letter_information.camp + ". There was a stiff competition for this position in this camp and you narrowly missed by a small"
            "margin. So we would encourage yu to practice a little more, improve your skills and apply for the upcoming camps. We will refund your fee"
            "We would be glad to hear from you soon."
        "</p>"
        "<div align=right>"
        "sincerly<br>"
        + letter_information.clerk_name +
        "</div>";

        QTextDocument document;
        document.setHtml(html);

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(letter_information.letter_path());
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));

        document.print(&printer);
}
