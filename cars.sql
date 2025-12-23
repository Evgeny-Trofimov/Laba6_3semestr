-- Очистка существующих таблиц
DROP TABLE IF EXISTS sales CASCADE;
DROP TABLE IF EXISTS cars CASCADE;
DROP TABLE IF EXISTS clients CASCADE;
DROP TABLE IF EXISTS models CASCADE;
DROP TABLE IF EXISTS manufacturers CASCADE;

-- 1. Производители 
CREATE TABLE manufacturers (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    country VARCHAR(50)
);

-- 2. Модели автомобилей 
CREATE TABLE models (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    year_started INT,
    manufacturer_id INT REFERENCES manufacturers(id) ON DELETE SET NULL
);

-- 3. Клиенты 
CREATE TABLE clients (
    id SERIAL PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    city VARCHAR(50)
);

-- 4. Автомобили в наличии 
CREATE TABLE cars (
    id SERIAL PRIMARY KEY,
    vin VARCHAR(17) UNIQUE NOT NULL,
    model_id INT REFERENCES models(id) ON DELETE CASCADE,
    year_produced INT,
    price NUMERIC(10,2)
);

-- 5. Продажи 
CREATE TABLE sales (
    id SERIAL PRIMARY KEY,
    car_id INT REFERENCES cars(id) ON DELETE CASCADE,
    client_id INT REFERENCES clients(id) ON DELETE CASCADE,
    sale_date DATE,
    discount_percent NUMERIC(5,2) DEFAULT 0
);

-- Заполнение данными
INSERT INTO manufacturers (name, country) VALUES 
('Toyota', 'Japan'),
('BMW', 'Germany'),
('Ford', 'USA');

INSERT INTO models (name, year_started, manufacturer_id) VALUES 
('Camry', 1982, 1),
('X5', 1999, 2),
('Mustang', 1964, 3),
('Corolla', 1966, 1);

INSERT INTO clients (full_name, city) VALUES 
('Иван Петров', 'Москва'),
('Мария Сидорова', 'Санкт-Петербург'),
('Алексей Козлов', 'Новосибирск');

INSERT INTO cars (vin, model_id, year_produced, price) VALUES 
('VIN123456789TOY1', 1, 2023, 2500000.00),
('VIN987654321BMW2', 2, 2024, 6500000.00),
('VIN555555555FORD', 3, 2022, 3200000.00),
('VIN777777777TOY2', 4, 2023, 1800000.00);

INSERT INTO sales (car_id, client_id, sale_date, discount_percent) VALUES 
(1, 1, '2024-03-15', 5.00),
(2, 2, '2024-04-01', 0.00),
(4, 3, '2024-05-10', 3.50);
