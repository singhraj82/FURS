#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QStringList>

// Database schema applications table
const std::string c_table_applications("applications");
const std::string c_table_field_id("id");
const std::string c_table_field_last_name("last_name");
const std::string c_table_field_first_name("first_name");
const std::string c_table_field_age("age");
const std::string c_table_field_gender("gender");
const std::string c_table_field_phone("phone");
const std::string c_table_field_street_address("street_address");
const std::string c_table_field_city("city");
const std::string c_table_field_state("state");
const std::string c_table_field_zip_code("zip_code");
const std::string c_table_field_speciality("speciality");
const std::string c_table_field_app_status("app_status");
const std::string c_table_field_payment_status("payment_status");
const std::string c_table_field_camp("camp");

const QString c_not_selected("Not selected");
const QString c_accepted("Accepted");
const QString c_rejected_failed_competition("Rejected_Failed_Competition");
const QString c_rejected_incomplete_application("Rejected_Incomplete_Application");
const QString c_rejected_failed_payment("Rejected_Failed_Payment");
const QString c_in_progress("In progress");

const QString c_singer("Singer");
const QString c_drummer("Drummer");
const QString c_guitarist("Guitarist");
const QString c_bassist("Bassist");
const QString c_keyboardist("Keyboardist");
const QString c_instrumentalist("Instrumentalist");

const QString c_june("Week 2-3 June");
const QString c_july("Week 2-3 July");
const QString c_august("Week 2-3 August");

const QString c_male("Male");
const QString c_female("Female");
const QString c_other("Other");

const QStringList states(
{
"Not selected",
"Alabama",
"Alaska",
"Arizona",
"Arkansas",
"California",
"Colorado",
"Connecticut",
"Delaware",
"Florida",
"Georgia",
"Hawaii",
"Idaho",
"Illinois",
"Indiana",
"Iowa",
"Kansas",
"Kentucky",
"Louisiana",
"Maine",
"Maryland",
"Massachusetts",
"Michigan",
"Minnesota",
"Mississippi",
"Missouri",
"Montana",
"Nebraska",
"Nevada",
"New Hampshire",
"New Jersey",
"New Mexico",
"New York",
"North Carolina",
"North Dakota",
"Ohio",
"Oklahoma",
"Oregon",
"Pennsylvania",
"Rhode Island",
"South Carolina",
"South Dakota",
"Tennessee",
"Texas",
"Utah",
"Vermont",
"Virginia",
"Washington",
"West Virginia",
"Wisconsin",
"Wyoming"});

const QStringList speciality(
{
c_not_selected,
c_singer,
c_drummer,
c_guitarist,
c_bassist,
c_keyboardist,
c_instrumentalist,
});

const QStringList camps(
{
  c_not_selected,
  c_june,
  c_july,
  c_august,
});

const QStringList application_status(
{
  c_not_selected,
  c_accepted,
  c_rejected_failed_competition,
  c_rejected_incomplete_application,
  c_rejected_failed_payment,
  c_in_progress,
});

const QStringList payment_status(
{
  "Not selected",
  "Successful",
  "Failed",
});

const QStringList gender(
{
  c_not_selected,
  c_male,
  c_female,
  c_other,
});




#endif // CONSTANTS_H
