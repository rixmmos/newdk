-- Grants for the application account.
--
-- This file used to also run `CREATE USER 'elcastle'@'%' IDENTIFIED BY '<literal>'`,
-- which put a password in a public repository. It no longer creates the account:
-- the MySQL image does that from MYSQL_USER / MYSQL_PASSWORD, which
-- docker/docker-compose.yml feeds from docker/.env (see docker/.env.example).
-- The image creates the account before it runs anything in
-- /docker-entrypoint-initdb.d/, so the account already exists by the time this
-- file executes and only the grants are left to do here.
--
-- The user name is not a secret; it is pinned in docker-compose.yml and in
-- docker/conf/*.conf.template (`DB_USER : elcastle`), and the three must agree.
--
-- The databases are created by DARKEDEN.sql and USERINFO.sql, which sort before
-- this file and carry their own `CREATE DATABASE ... ; USE ...;`. The
-- IF NOT EXISTS statements below are kept so this file is still correct when run
-- on its own against an empty server.
--
-- That was only half true until 2026-08-13: USERINFO.sql was dumped with
-- --databases and DARKEDEN.sql was not, so DARKEDEN.sql carried neither
-- statement. Under docker-compose the entrypoint therefore aborted it at its
-- first DROP TABLE with `ERROR 1046 (3D000): No database selected`, and the
-- DARKEDEN created here -- last, by the IF NOT EXISTS below -- stayed empty.
-- DARKEDEN.sql now carries both statements and the sentence above is true of
-- both dumps. Verified by seeding a clean mysql/mysql-server:5.7 container from
-- this directory: 374 DARKEDEN tables and 7 USERINFO tables [measured
-- 2026-08-13].

CREATE DATABASE IF NOT EXISTS DARKEDEN;
CREATE DATABASE IF NOT EXISTS USERINFO;

GRANT ALL PRIVILEGES ON DARKEDEN.* TO 'elcastle'@'%';
GRANT ALL PRIVILEGES ON USERINFO.* TO 'elcastle'@'%';

FLUSH PRIVILEGES;
