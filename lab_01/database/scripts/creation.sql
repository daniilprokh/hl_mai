# Создание базы данных
CREATE DATABASE IF NOT EXISTS messenger;

USE messenger;

# Создание таблицы пользователей
CREATE TABLE IF NOT EXISTS users (
    PRIMARY KEY (user_id),
    user_id     INT             NOT NULL    AUTO_INCREMENT,
    first_name  VARCHAR(256)    NOT NULL,
    last_name   VARCHAR(256)    NOT NULL,
    email       VARCHAR(256)    NOT NULL,
        INDEX name_index    USING   BTREE (first_name, last_name),
        INDEX email_index   USING   HASH  (email)
)
  CHARACTER SET utf8
  COLLATE utf8_unicode_ci;
