#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace std;
int ZAPYAT = 0;

struct StudentRecord {
public:
    StudentRecord(
        string табель,
        string фио,
        string отдел,
       string должность,
        string примечание,
        string время_прохода,
        string направление,
        string событие,
        string объект_прохода
    )
    {
        ТАБЕЛЬ = табель;
        ФИО = фио;
        ОТДЕЛ = отдел;
        ДОЛЖНОСТЬ = должность;
        ПРИМЕЧАНИЕ = примечание;
        ВРЕМЯ_ПРОХОДА = время_прохода;
        НАПРАВЛЕНИЕ = направление;
        СОБЫТИЕ = событие;
        ОБЪЕКТ_ПРОХОДА = объект_прохода;
    }
    void display() {
        cout << "                 ID: " << ТАБЕЛЬ << endl;
        cout << "                ФИО: " << ФИО << endl;
        cout << "              Отдел: " << ОТДЕЛ << endl;
        cout << "          Должность: " << ДОЛЖНОСТЬ << endl;
        cout << "примечание(комната): " << ПРИМЕЧАНИЕ << endl;
        cout << "      Время прохода: " << ВРЕМЯ_ПРОХОДА << endl;
        cout << "        Направление: " << НАПРАВЛЕНИЕ << endl;
        cout << "     Сбытие(да/нет): " << СОБЫТИЕ << endl;
        cout << "                Где: " << ОБЪЕКТ_ПРОХОДА << endl;
        cout << endl;
    }
    string ТАБЕЛЬ;
    string ФИО;
    string ОТДЕЛ;
    string ДОЛЖНОСТЬ;
    string ПРИМЕЧАНИЕ;
    string ВРЕМЯ_ПРОХОДА;
    string НАПРАВЛЕНИЕ;
    string СОБЫТИЕ;
    string ОБЪЕКТ_ПРОХОДА;
};


void updateSkudCheck(sql::Connection* con, bool a) {
    try {
        sql::PreparedStatement* pstmt;
        if (a==True) {
            pstmt = con->prepareStatement("UPDATE attendance SET skud_check = 1");
        }
        else {
            pstmt = con->prepareStatement("UPDATE attendance SET skud_check = 0");
        }
        
        pstmt->executeUpdate();
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error updating skud_check: " << e.what() << std::endl;
    }
}



void displayStudents(vector<StudentRecord>& students) {
    for (auto student : students) {
        student.display();
    }
}
void displayStudentsOnly(vector<StudentRecord>& students,string needly) {
    for (auto student : students) {
        if(student.ФИО==needly) student.display();
    }
}

bool checker(vector<StudentRecord>& students, string place, string name, string path, string timeNeed) {
    srand((unsigned)time(NULL));
    ofstream fout;
    fout.open(path, ofstream::app);
    bool flag = 1;
    setlocale(LC_ALL, "ru");


    for (auto student : students) {
        bool k = 1;
        for (int i = 0; i < 10; i++) {
            if (timeNeed[i] != student.ВРЕМЯ_ПРОХОДА[i]) {
                k = false;
                break;
            }
        }
        if (name == student.ФИО ) {   
            
            cout << " время   " << student.ВРЕМЯ_ПРОХОДА << endl;
            if (flag) {
                if (ZAPYAT == 0) {
                    fout << "\n\t{\n\t \"class_id\":" << rand() % 400 + 1 << ',' << endl
                        << " \t \"student_id\":" << student.ТАБЕЛЬ << ',' << endl
                        << " \t \"qr_check\":" << 1 << ',' << endl;
                        updateSkudCheck(con, true);
                    ZAPYAT = 1;
                }
                else {
                    fout << "\n\t,{\n\t \"class_id\":" << rand() % 400 + 1 << ',' << endl
                        << " \t \"student_id\":" << student.ТАБЕЛЬ << ',' << endl
                        << " \t \"qr_check\":" << 1 << ',' << endl;
                    updateSkudCheck(con, true);
                }
                flag = 0;
            }
            if (place == student.ОБЪЕКТ_ПРОХОДА && student.НАПРАВЛЕНИЕ == "Вход" && k) {
                cout << "\nНА МЕСТЕ";
                fout << "\t \"skud_check\":" << 1  << endl << "\t}";
                updateSkudCheck(con, true);
                fout.close();
                return 1;
            }
        }
    }


    cout << "\nНЕТ";
    fout << "\t \"skud_check\":" << 0 << endl << "\t}";
    fout.close();
    updateSkudCheck(con, false);
    return 0;



}

void displayStudentsOnlyStuding(vector<StudentRecord>& students,string needly) {
    for (auto student : students) {
        if    ((student.ОБЪЕКТ_ПРОХОДА[0] == 'У'
            || student.ОБЪЕКТ_ПРОХОДА[0] == 'Н'
            || student.ОБЪЕКТ_ПРОХОДА[0] == 'К') && student.НАПРАВЛЕНИЕ=="Вход" && student.ФИО == needly)
        student.display();
    }
}

string ТАБЕЛЬ;
string ФИО;
string ОТДЕЛ;
string ДОЛЖНОСТЬ;
string ПРИМЕЧАНИЕ;
string ВРЕМЯ_ПРОХОДА;
string НАПРАВЛЕНИЕ;
string СОБЫТИЕ;
string ОБЪЕКТ_ПРОХОДА;
string tempString;





vector<StudentRecord> students;




int main()
{

    time_t now = time(0);
    srand((unsigned)time(NULL));
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    const time_t tm = time(nullptr);
    char buf[64];
    strftime(buf, std::size(buf), "%d.%m.%Y", localtime(&tm));

    string timeNeed = "";
    for (int i = 0; i < 10; i++) {
        timeNeed += buf[i];
    }


    //cout << timeNeed;
    ifstream inputFile;
    inputFile.open("C:\\temp\\NOVEMBER.csv");
    string line = "";

    string path = "C:\\bufer\\";
    path += buf;
    path += ".json";
    ofstream fout;
    fout.open(path);
    fout << "[";
    fout.close();

    while (getline(inputFile, line)) {
        stringstream inputString(line);
        getline(inputString, ТАБЕЛЬ, ';');
        getline(inputString, ФИО, ';');
        getline(inputString, ОТДЕЛ, ';');
        getline(inputString, ДОЛЖНОСТЬ, ';');
        getline(inputString, ПРИМЕЧАНИЕ, ';');
        getline(inputString, ВРЕМЯ_ПРОХОДА, ';');
        getline(inputString, НАПРАВЛЕНИЕ, ';');
        getline(inputString, СОБЫТИЕ, ';');
        getline(inputString, ОБЪЕКТ_ПРОХОДА, ';');
        //  if (ФИО == "Будзиаловский Павел Борисович") {
        StudentRecord student(ТАБЕЛЬ, ФИО, ОТДЕЛ, ДОЛЖНОСТЬ, ПРИМЕЧАНИЕ, ВРЕМЯ_ПРОХОДА, НАПРАВЛЕНИЕ, СОБЫТИЕ, ОБЪЕКТ_ПРОХОДА);
        students.push_back(student);

        line = "";
    }

    string Who;
    string Where;

    ////////////////////////////////////////////////////////

    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        // Инициализация драйвера
        driver = sql::mysql::get_mysql_driver_instance();

        // Установка соединения
        con = driver->connect("tcp://127.0.0.1:3306/your_database", "your_username", "your_password");

        // Использование конкретной базы данных
        con->setSchema("your_database");

        // Извлечение всех строк из таблицы attendance
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT student_id, class_id FROM attendance");

        // Проход по всем строкам таблицы attendance
        while (res->next()) {
            std::string fio = "0";
            std::string building = "0";

            int student_id = res->getInt("student_id");
            int class_id = res->getInt("class_id");

            // Извлечение FIO из таблицы people по student_id
            pstmt = con->prepareStatement("SELECT fio FROM people WHERE id = ?");
            pstmt->setInt(1, student_id);
            sql::ResultSet* res_people = pstmt->executeQuery();

            if (res_people->next()) {
                fio = res_people->getString("fio");
                std::cout << "Student FIO: " << fio << std::endl;
            }

            delete res_people;
            delete pstmt;

            // Извлечение building из таблицы classes по class_id
            pstmt = con->prepareStatement("SELECT building FROM classes WHERE id = ?");
            pstmt->setInt(1, class_id);
            sql::ResultSet* res_classes = pstmt->executeQuery();

            if (res_classes->next()) {
                building = res_classes->getString("building");
                std::cout << "Class Building: " << building << std::endl;

                if (fio == "0") break;

                //timeNeed = "30.11.2021"; //////////////time
                checker(students, building, fio, path, timeNeed);
                cout << endl;







            }

            delete res_classes;
            delete pstmt;
        }

        // Освобождение ресурсов
        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    /////////////////////////////////////////////////////////
    



    /*
    while (1) {
        cout << "Кого проверить...";
        getline(cin, Who);

        if (Who == "1") break;
        cout << "\nКуда должен был зайти студент?";
        getline(cin, Where);
        //timeNeed = "30.11.2021"; //////////////time
        checker(students, Where, Who, path, timeNeed);
        cout << endl;
    }
 */


    /*cout << "Кого проверить...";
    getline(cin, Who);*/
    fout.open(path, ofstream::app);
    fout << "\n]";
    fout.close();
    //displayStudentsOnly(students, Who);
    //displayStudents(students);
}
