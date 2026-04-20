# Step 1: MySQL install + schema load

Goal: bring up a local MariaDB on WSL2, create the two databases the
server expects (`DARKEDEN`, `USERINFO`), create user `elcastle`@`%` with
password `elca110`, grant it privileges, and load the two schema dumps.

Why MariaDB (not MySQL 8)? The dumps in `dkrixserver/initdb/` were taken
from MySQL 5.7.35 and the server uses the libmysqlclient C API. MariaDB
10.6 is wire-compatible, ships with sensible defaults on Ubuntu 22.04,
and is a single-package install. MySQL 8.0 also works but enforces
`ONLY_FULL_GROUP_BY` and `NO_ZERO_DATE` by default, which 15-year-old
dumps trip on; if you already have MySQL 8 installed, skip to the
"MySQL 8 notes" section at the bottom.

Why user `elcastle` / password `elca110` and not `rixpass`? Those creds
are hardcoded in three server config files (`gameserver.conf`,
`loginserver.conf`, `sharedserver.conf`) as `DB_USER` / `DB_PASSWORD` /
`UI_DB_USER` / `UI_DB_PASSWORD` / `DIST_DB_USER` / `DIST_DB_PASSWORD`.
Matching the SQL bootstrap to the configs is nine lines shorter than
rewriting the configs, and this is a local smoke test — nothing is
exposed.

## 1. Install MariaDB

```bash
sudo apt update
sudo apt install -y mariadb-server mariadb-client
```

Start it and enable on boot (WSL2 systemd-less note: if `systemctl`
returns "System has not been booted with systemd…", use the service
command instead — both are shown):

```bash
# WSL2 with systemd (WSL >= 0.67.6 + /etc/wsl.conf has systemd=true):
sudo systemctl start  mariadb
sudo systemctl enable mariadb

# WSL2 without systemd, fallback:
sudo service mariadb start
```

Verify:

```bash
sudo ss -ltnp | grep 3306
# expect: LISTEN 0 80 127.0.0.1:3306 ...  (or 0.0.0.0:3306)
```

If nothing is listening, check `sudo journalctl -u mariadb --no-pager | tail -30`
(or `/var/log/mysql/error.log`).

## 2. Lock down the root account (optional for smoke test)

You can skip `mysql_secure_installation` for a local-only smoke test —
Ubuntu's MariaDB package uses `unix_socket` auth for root so nobody
outside your user can log in anyway. If you want to run it:

```bash
sudo mysql_secure_installation
# Enter (blank) for current root password
# n — don't switch to unix_socket (already on it)
# n — don't set root password (keep unix_socket)
# y — remove anonymous users
# y — disallow root remote login
# y — remove test db
# y — reload privileges
```

## 3. Create databases, user, grants

`dkrixserver/initdb/a-setup.sql` already has the right CREATE/GRANT
statements:

```sql
CREATE DATABASE IF NOT EXISTS DARKEDEN;
CREATE DATABASE IF NOT EXISTS USERINFO;
CREATE USER 'elcastle'@'%' IDENTIFIED BY 'elca110';
GRANT ALL PRIVILEGES ON DARKEDEN.* TO 'elcastle'@'%';
GRANT ALL PRIVILEGES ON USERINFO.* TO 'elcastle'@'%';
```

Run it as root (unix_socket auth, no password needed):

```bash
cd /mnt/c/newdk/dkrixserver
sudo mysql < initdb/a-setup.sql
sudo mysql -e "FLUSH PRIVILEGES;"
```

Add a `'elcastle'@'localhost'` grant too — some MariaDB configs treat
`localhost` and `%` as distinct hosts and the server will connect over
TCP to 127.0.0.1:

```bash
sudo mysql <<'SQL'
CREATE USER IF NOT EXISTS 'elcastle'@'localhost' IDENTIFIED BY 'elca110';
GRANT ALL PRIVILEGES ON DARKEDEN.* TO 'elcastle'@'localhost';
GRANT ALL PRIVILEGES ON USERINFO.* TO 'elcastle'@'localhost';
CREATE USER IF NOT EXISTS 'elcastle'@'127.0.0.1' IDENTIFIED BY 'elca110';
GRANT ALL PRIVILEGES ON DARKEDEN.* TO 'elcastle'@'127.0.0.1';
GRANT ALL PRIVILEGES ON USERINFO.* TO 'elcastle'@'127.0.0.1';
FLUSH PRIVILEGES;
SQL
```

Verify login as `elcastle`:

```bash
mysql -h 127.0.0.1 -u elcastle -pelca110 -e "SHOW DATABASES;"
# expect to see DARKEDEN and USERINFO in the list
```

## 4. Load the schema dumps

```bash
cd /mnt/c/newdk/dkrixserver/initdb

mysql -h 127.0.0.1 -u elcastle -pelca110 DARKEDEN < DARKEDEN.sql
mysql -h 127.0.0.1 -u elcastle -pelca110 USERINFO < USERINFO.sql
```

Expected: silent success. If you see warnings about `NO_AUTO_CREATE_USER`
sql_mode, ignore them — that mode was removed in MySQL 8 / MariaDB 10.6
and the dump's SET statement is a no-op.

Verify row counts:

```bash
mysql -h 127.0.0.1 -u elcastle -pelca110 DARKEDEN <<'SQL'
SHOW TABLES;
SELECT COUNT(*) AS tables_in_DARKEDEN
  FROM information_schema.tables
  WHERE table_schema='DARKEDEN';
SQL

mysql -h 127.0.0.1 -u elcastle -pelca110 USERINFO <<'SQL'
SHOW TABLES;
SELECT COUNT(*) AS tables_in_USERINFO
  FROM information_schema.tables
  WHERE table_schema='USERINFO';
SQL
```

Paste me the table counts and any errors — that tells us whether the
schema landed cleanly before we touch the server configs in step 2.

## 5. What step 2 will change

Preview so you know what's coming and can sanity-check now: the three
configs (`gameserver.conf`, `loginserver.conf`, `sharedserver.conf`)
currently point `DB_HOST` / `UI_DB_HOST` / `DIST_DB_HOST` at
`192.168.0.16`. In step 2 we rewrite all three to `127.0.0.1`. If your
WSL2 distro uses a different loopback (unusual), now's the time to
flag it.

## MySQL 8 notes (skip if you're on MariaDB)

If you already have MySQL 8 installed, two gotchas:

1. `sql_mode` — the dumps were taken before `ONLY_FULL_GROUP_BY` and
   `NO_ZERO_DATE` became defaults. Either start mysqld with
   `--sql-mode=""` or add to `/etc/mysql/mysql.conf.d/mysqld.cnf`:
   ```
   [mysqld]
   sql_mode = ""
   ```
   then `sudo systemctl restart mysql`.

2. `mysql_native_password` — the server's libmysqlclient is old
   enough that MySQL 8's default `caching_sha2_password` auth plugin
   will reject it. Create the user with the legacy plugin instead:
   ```sql
   CREATE USER 'elcastle'@'%' IDENTIFIED WITH mysql_native_password BY 'elca110';
   ```
   Same for the `localhost` and `127.0.0.1` variants.

## Quick rollback

If something goes sideways and you want to start over:

```bash
sudo mysql <<'SQL'
DROP DATABASE IF EXISTS DARKEDEN;
DROP DATABASE IF EXISTS USERINFO;
DROP USER IF EXISTS 'elcastle'@'%';
DROP USER IF EXISTS 'elcastle'@'localhost';
DROP USER IF EXISTS 'elcastle'@'127.0.0.1';
FLUSH PRIVILEGES;
SQL
```

Then rerun from step 3.
