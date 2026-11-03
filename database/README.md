# Database setup

The server currently connects to the `qt_project` MySQL database through Qt's `QMYSQL` driver. `init.sql` creates the database and the `user_info` and `friend` tables using the schema supplied for this project. No test records are required by the current source, so the script does not insert seed data.

## Initialize

Open a MySQL command-line client with an account allowed to create databases and tables, then run the script. It creates the database if absent and does not drop, truncate, or modify existing tables. Run it once on a fresh database; if tables already exist, MySQL will report that they exist and leave them unchanged.

For example:

```text
SOURCE path/to/database/init.sql;
```

## Configure the server

Copy `Server/database.ini.example` to a file named `database.ini` beside the running `Server.exe`, then fill in the local connection values. The application reads this file at startup. Do not commit the local file or place real credentials in the example.

The database account needs access to `qt_project`. Prefer a project-specific account with only the permissions the application needs.
