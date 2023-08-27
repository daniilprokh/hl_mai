# Создание базы данных
CREATE DATABASE IF NOT EXISTS messenger;
# Создание админа базы данных
CREATE USER IF NOT EXISTS test_admin IDENTIFIED BY 'password';
# Предоставления админу полномочий
GRANT ALL ON messenger.* TO test_admin;

USE messenger;

# Создание таблицы пользователей
CREATE TABLE IF NOT EXISTS users (
    PRIMARY KEY (user_id),
    user_id     INT             NOT NULL    AUTO_INCREMENT,
    first_name  VARCHAR(256)    NOT NULL,
    last_name   VARCHAR(256)    NOT NULL,
    email       VARCHAR(256)    NOT NULL,
    login       VARCHAR(256)    NOT NULL,
    password    VARCHAR(256)    NOT NULL,
        INDEX name_index    USING   BTREE (first_name, last_name),
        INDEX email_index   USING   HASH  (email)
)
  CHARACTER SET utf8
  COLLATE utf8_unicode_ci;
