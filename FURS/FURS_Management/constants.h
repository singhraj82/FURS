#ifndef CONSTANTS_H
#define CONSTANTS_H


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
"Not selected",
"Singer",
"Drummer",
"Guitarist",
"Bassist",
"Keyboardist",
"Instrumentalist",
});

const QStringList camps(
{
  "Not selected",
  "Week 2-3 June",
  "Week 2-3 July",
  "Week 2-3 August",
});

const QStringList application_status(
{
  "Not selected",
  "Accepted",
  "Rejected",
  "Rejected_incomplete",
  "Rejected_failed_payment",
  "In progress",
});

const QStringList payment_status(
{
  "Not selected",
  "Successful",
  "Failed",
});

const QStringList gender(
{
  "Not selected",
  "Male",
  "Female",
  "Other",
});

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


#endif // CONSTANTS_H
