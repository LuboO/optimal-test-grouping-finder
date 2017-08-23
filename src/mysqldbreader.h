#ifndef MYSQLDBREADER_H
#define MYSQLDBREADER_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "atomictest.h"

class MySQLDbReader {
public:
    MySQLDbReader() = delete;

    MySQLDbReader(const std::string & dbConfigPath) {
        /* Parsing options from ini file */
        namespace pt = boost::property_tree;
        pt::ptree t;
        pt::ini_parser::read_ini(dbConfigPath, t);

        std::stringstream fullHost;

        _name = t.get<std::string>("Database.Name");
        fullHost << t.get<std::string>("Database.Host")
                 << ":"
                 << t.get<int>("Database.Port");
        _host = fullHost.str();
        _user = t.get<std::string>("Database.User");
        _passwd = t.get<std::string>("Database.Password");
    }

    std::vector<AtomicTest> getDataEntries() const;

private:
    std::string _name;
    std::string _host;
    std::string _user;
    std::string _passwd;
};

#endif // MYSQLDBREADER_H
