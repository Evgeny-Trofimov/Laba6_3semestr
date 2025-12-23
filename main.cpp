#include <iostream>       
#include <pqxx/pqxx>      
#include <string>         
#include <iomanip>        

using namespace std;

class CarDealershipDB {
private:
    string connection_string; //cтрока подключения к PostgreSQL

public:
    //сохраняет строку подключения
    CarDealershipDB(const string& conn_str) : connection_string(conn_str) {}

    void execute_command(const string& sql, const string& desc) {
        try {
            pqxx::connection C(connection_string); //создаём подключение
            pqxx::work W(C);                       //начинаем транзакцию
            W.exec(sql);                           //выполняем SQL
            W.commit();                            //подтверждаем изменения
            cout << "[SUCCESS] " << desc << endl;  //успешное завершение
        } catch (const exception &e) {
            //обработка ошибок 
            cerr << "[ERROR] " << desc << ": " << e.what() << endl;
        }
    }

    void print_query_results(const string& sql, const string& title) {
        try {
            pqxx::connection C(connection_string);
            //nontransaction — для запросов только на чтение
            pqxx::nontransaction N(C);
            pqxx::result R(N.exec(sql)); //результат запроса

            cout << "\n--- " << title << " ---" << endl;
            if (R.empty()) {
                cout << "Нет данных." << endl;
                return;
            }

            //выводим заголовки колонок
            for (int col = 0; col < R.columns(); ++col) {
                cout << setw(20) << left << R.column_name(col);
            }
            cout << endl << string(R.columns() * 20, '-') << endl;

            //выводим строки результата
            for (const auto& row : R) {
                for (const auto& field : row) {
                    cout << setw(20) << left << field.c_str(); // field — значение ячейки
                }
                cout << endl;
            }
        } catch (const exception &e) {
            cerr << "Ошибка выполнения запроса: " << e.what() << endl;
        }
    }
};

int main() {
    //cтрока подключения к локальной БД cars_db
    string conn_str = "dbname=cars_db user=postgres password=labpass host=localhost port=5432";
    CarDealershipDB db(conn_str); //cоздаём объект для работы с БД

    cout << "Управление базой данных автосалона\n" << endl;

    //1. INSERT — добавление новой записи (клиент)
    db.execute_command(
        "INSERT INTO clients (full_name, city) VALUES ('Елена Волкова', 'Екатеринбург');",
        "Добавление клиента: Елена Волкова"
    );

    //2. UPDATE — изменение данных (цена автомобиля)
    db.execute_command(
        "UPDATE cars SET price = 2450000.00 WHERE vin = 'VIN123456789TOY1';",
        "Обновление цены на Camry (VIN: VIN123...)"
    );

    //3. SELECT с фильтрацией (WHERE) — автомобили до 2023 г.
    db.print_query_results(
        "SELECT vin, year_produced FROM cars WHERE year_produced < 2023;",
        "Автомобили, произведённые до 2023 года"
    );

    //4. INNER JOIN — связь моделей и производителей
    db.print_query_results(
        "SELECT m.name AS Модель, man.name AS Производитель, man.country AS Страна "
        "FROM models m "
        "JOIN manufacturers man ON m.manufacturer_id = man.id;",
        "Модели и их производители"
    );

    //5. LEFT JOIN — поиск клиентов без покупок (s.id IS NULL)
    db.print_query_results(
        "SELECT c.full_name AS Имя FROM clients c "
        "LEFT JOIN sales s ON c.id = s.client_id "
        "WHERE s.id IS NULL;",
        "Клиенты без покупок"
    );

    //6. Агрегация: COUNT + GROUP BY — сколько моделей у каждого производителя
    db.print_query_results(
        "SELECT man.name AS Производитель, COUNT(m.id) AS Количество_моделей "
        "FROM manufacturers man "
        "JOIN models m ON man.id = m.manufacturer_id "
        "GROUP BY man.name;",
        "Количество моделей у каждого производителя"
    );

    //7. JOIN четырёх таблиц — полная история продаж
    db.print_query_results(
        "SELECT cl.full_name AS Клиент, man.name AS Бренд, mo.name AS Модель, "
        "ca.year_produced AS Год_выпуска, s.sale_date AS Дата_продажи "
        "FROM sales s "
        "JOIN cars ca ON s.car_id = ca.id "
        "JOIN models mo ON ca.model_id = mo.id "
        "JOIN manufacturers man ON mo.manufacturer_id = man.id "
        "JOIN clients cl ON s.client_id = cl.id "
        "ORDER BY s.sale_date;",
        "История продаж"
    );

    //8. HAVING — фильтрация после агрегации (только производители с >1 моделью)
    db.print_query_results(
        "SELECT man.name AS Производитель, COUNT(m.id) AS Количество "
        "FROM manufacturers man "
        "JOIN models m ON man.id = m.manufacturer_id "
        "GROUP BY man.name "
        "HAVING COUNT(m.id) > 1;",
        "Производители с более чем одной моделью"
    );

    //9. Подзапрос (вложенный SELECT) — японские авто после 2022 г.
    db.print_query_results(
        "SELECT vin AS VIN, year_produced AS Год_выпуска FROM cars "
        "WHERE model_id IN ("
        "    SELECT id FROM models "
        "    WHERE manufacturer_id IN ("
        "        SELECT id FROM manufacturers WHERE country = 'Japan'"
        "    )"
        ") AND year_produced > 2022;",
        "Японские автомобили, произведённые после 2022 года"
    );

    //10. DELETE — удаление временной записи (тестового клиента)
    db.execute_command(
        "DELETE FROM clients WHERE full_name = 'Елена Волкова';",
        "Очистка: удаление тестового клиента"
    );

    return 0;
}
