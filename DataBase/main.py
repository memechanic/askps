# Подключение необходымых данных для подключения к СУБД

from config import CONFIG, DB_NAME


import mysql.connector
from mysql.connector import errorcode

# Подключение библиотек для создания отчета

import pandas as pd
from datetime import date as Date

# Подключение библиотеки для работы с JSON файлами
import json


# Функция создания таблицы
def create_table(cur, table):
    try:
        cur.execute(table)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
            print("Table already exist")
        else:
            print("Something went wrong (create table): {}".format(err.msg))
    else:
        print("Table created")
        
        
# Функция добавления записи в таблицу успеваемости

def add_check(cur, data, table):
    query = ('INSERT INTO {table} '
             '(class_id, student_id, qr_check, skud_check) '
             'VALUES (%(class_id)s, %(student_id)s, %(qr_check)s, %(skud_check)s)'.format(table=table))
    try:
        cur.execute(query, data)
    except mysql.connector.Error as err:
        print("Something went wrong (add_check): {}".format(err))

def check_student(cur, _stud_id, _class_id):
    query = ('SELECT class_id, student_id, qr_check, skud_check FROM attendance')
    _stud_id = int(_stud_id)
    _class_id = int(_class_id)
    try:
        cur.execute(query)
        
        for class_id, student_id, qr_check, skud_check in cur:
            if student_id == _stud_id and class_id == _class_id:
                if qr_check and skud_check:
                    print(f'Student with id {student_id} was on class (class_id {class_id})')
                elif qr_check and not skud_check:
                    print(f'Student with id {student_id} was not on class (class_id {class_id})\n because no skud check')
            else:
                continue
        print(f'Student with id {student_id} was not on class (class_id {class_id})\n because no qr check')
                
        
    except mysql.connector.Error as err:
        print("Something went wrong (add_check): {}".format(err))

# Функции получения информации объектов для составления отчета

def get_group_id(cur, group_name):
    
    query = "SELECT group_id FROM stud_groups WHERE name = '{gn}'".format(gn = group_name)
    
    try:
        cur.execute(query)
        results = cur.fetchall()
        return results[0][0]
    
    except mysql.connector.Error as err:
         print("Something went wrong (get_group_id): {}".format(err))

def get_teacher_id(cur, teacher_fio):
    
    query = "SELECT people_id FROM people WHERE fio = '{fio}' AND isteacher = 1".format(fio = teacher_fio)
    
    try:
        cur.execute(query)
        results = cur.fetchall()
        return results[0][0]
    
    except mysql.connector.Error as err:
         print("Something went wrong (get_teacher_id): {}".format(err))

def get_class_id(cur, group_id, teacher_id):
    
    query = "SELECT id FROM classes WHERE group_id = {gid} AND teacher_id = {tid}".format(gid = group_id, tid = teacher_id)

    try:
        cur.execute(query)
        results = cur.fetchall()
        return results[0][0]
    
    except mysql.connector.Error as err:
         print("Something went wrong (get_class_id): {}".format(err))

# Создание отчета по успеваемости

def create_attendance_statistics(cur, teacher_fio, group_name, subject = None):
    
    teacherId = get_teacher_id(cur, teacher_fio)
    groupId = get_group_id(cur, group_name)
    classId = get_class_id(cur, groupId, teacherId)
    
    query = "SELECT student_id, qr_check, skud_check FROM attendance WHERE class_id = '{cid}'".format(cid = classId)
    
    try:
        tableInfo = []
        
        cur.execute(query)
        attendance = cur.fetchall()
        
        cur.execute("SELECT date FROM classes WHERE  group_id = {gid} AND teacher_id = {tid}".format(gid = groupId, tid = teacherId))
        date = cur.fetchall()[0][0]
        filename = "отчет_{date}_{now}.xlsx".format(date = date, now = str(Date.today()))
        
        for row in attendance:
            cur.execute("SELECT fio FROM people WHERE people_id = {pid} AND isstudent = 1".format(pid = row[0]))
            fio = cur.fetchall()[0][0]
            status = ""
            
            if row[1] and row[2]: status = "+"
            else:
                if not row[1] and row[2]: status = "no qr"
                elif row[1] and not row[2]: status = "no skud"
                else: status = "-"
            
            tableInfo.append(tuple([fio, status]))

        table = pd.DataFrame(tableInfo, columns=["Фамилия", date])
        
        try:
            sheet_name = "sheet"
            with pd.ExcelWriter(filename, engine='xlsxwriter') as writer:
                table.to_excel(writer, sheet_name=sheet_name)
            
            print("Stats created")
            
        except BaseException as err :
            print("Something went wrong (file): {}".format(err))
    
    except mysql.connector.Error as err:
        print("Something went wrong (create_attendance_statistics): {}".format(err))
        

# Функция отчистки таблицы

def clear_table(cur, table):
    try:
        cur.execute("DELETE FROM "+ table)
    except mysql.connector.Error as err:
        print("Something went wrong (clear_table): {}".format(err))


# Функция обработки JSON файла

def json_to_dict(filename = "data.json"):
    try:
        file = open(filename,"r",encoding="utf-8")
        data = json.loads(file.read(), object_hook = dict)
        return data
    except BaseException as err:
        print("There is problem in json_to_dict func: {}".format(err))
        

def txt_to_dict(filename = "text.txt"):
    try:
        file = open(filename)
        data = eval(file.read())
        return data
    except BaseException as err:
        print("There is a problem in txt_to_dict func:", err)

#Соеднинение с базой данных
if __name__ == '__main__':
    try:
        Connection = mysql.connector.connect(**CONFIG)
        Cursor = Connection.cursor()
        Cursor.execute(
            'USE {}'.format(DB_NAME)
        )
        teacher_name = input("Введите имя преподавателя: ")
        group_name = input("Введите номер группы: ")
        
        
        create_attendance_statistics(Cursor, teacher_name, group_name)
        
        # clear_table(Cursor, 'attendance')
        # data = json_to_dict("19.01.2024.json")
        # for stud in data:
        #     add_check(Cursor,stud,'attendance')
        
        # gid = get_group_id(Cursor, "1a11")
        # tid = get_teacher_id(Cursor, "Агафонов Павел Маркович")
        # print(get_class_id(Cursor, gid, tid))
        # 
        # print()
        
        # print(create_attendance_statistics(Cursor, 1, "1a11", 1))
        
        # check_student(Cursor, '258970', '108')
        # check_student(Cursor, '557842', '160')
        # check_student(Cursor, '123654', '160')

    # Обработка ошибок    

    except mysql.connector.Error as err:

        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something went wrong with your login or password")

        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("This database does not exist")

        else:
            print(err)

    else:
        try:
            Connection.commit()

            Cursor.close()
            Connection.close()
        except mysql.connector.Error as err:
            print("Something went wrong (main): {}".format(err))

        else: print("\nThere are no problems with commiting and closing connections\n")