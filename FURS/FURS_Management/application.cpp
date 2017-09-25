#include "application.h"

Application::Application()
{

}

void Application::set_last_name(std::string last_name)
{
    m_last_name = last_name;
}

std::string Application::last_name() const
{
    return m_last_name;
}

void Application::set_first_name(std::string first_name)
{
    m_first_name = first_name;
}

std::string Application::first_name() const
{
    return m_first_name;
}

void Application::set_gender(std::string gender)
{
    m_gender = gender;
}

std::string Application::gender() const
{
    return m_gender;
}

void Application::set_phone(std::string phone)
{
    m_phone = phone;
}

std::string Application::phone() const
{
    return m_phone;
}

void Application::set_street_address(std::string street_address)
{
    m_street_address = street_address;
}

std::string Application::street_address() const
{
    return m_street_address;
}

void Application::set_city(std::string city)
{
    m_city = city;
}

std::string Application::city() const
{
    return m_city;
}

void Application::set_state(std::string state)
{
    m_state = state;
}

std::string Application::state() const
{
    return m_state;
}

void Application::set_zipcode(std::string zipcode)
{
    m_zipcode = zipcode;
}

std::string Application::zipcode() const
{
    return m_zipcode;
}

void Application::set_speciality(const std::string speciality)
{
    m_speciality = speciality;
}

std::string Application::speciality() const
{
    return m_speciality;
}

void Application::set_application_status(std::string application_status)
{
    m_application_status = application_status;
}

std::string Application::application_status() const
{
    return m_application_status;
}

void Application::set_payment_status(std::string payment_status)
{
    m_payment_status = payment_status;
}

std::string Application::payment_status() const
{
    return m_payment_status;
}

void Application::set_camp(std::string camp)
{
    m_camp = camp;
}

std::string Application::camp() const
{
    return m_camp;
}
