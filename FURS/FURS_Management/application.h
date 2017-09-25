#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
class Application
{
public:
    Application();

    void set_last_name(std::string last_name);
    std::string last_name() const;

    void set_first_name(std::string first_name);
    std::string first_name() const;

    void set_age(int age);
    int age();

    void set_gender(std::string gender);
    std::string gender() const;

    void set_phone(std::string phone);
    std::string phone() const;

    void set_street_address(std::string street_address);
    std::string street_address() const;

    void set_city(std::string city);
    std::string city() const;

    void set_state(std::string state);
    std::string state() const;

    void set_zipcode(std::string zipcode);
    std::string zipcode() const;

    void set_speciality(const std::string);
    std::string speciality() const;

    void set_application_status(std::string application_status);
    std::string application_status() const;

    void set_payment_status(std::string payment_status);
    std::string payment_status() const;

    void set_camp(std::string camp);
    std::string camp() const;


private:
    std::string m_last_name;
    std::string m_first_name;
    std::string m_gender;
    std::string m_phone;
    std::string m_street_address;
    std::string m_city;
    std::string m_state;
    std::string m_zipcode;
    std::string m_speciality;
    std::string m_application_status;
    std::string m_payment_status;
    int m_age;
    std::string m_camp;
};

#endif // APPLICATION_H
