# SR Project

This project was developed for a distributed systems class and involves three endpoints: `entr1`, `entr2`, and `server`.

## Components

1. **Server:**
   - Connected to `entr1` through UDP.
   - Connected to `entr2` through TCP.
   - Connected to a client through TCP.

2. **Entr1:**
   - Retrieves data from a CSV file.

3. **Entr2:**
   - Retrieves data from a MySQL database.
   - Make sure to update the database settings in `entr2/db.c` after configuring your MySQL database.

## Execution Notes

- Create a `bin` folder in root
- Use the provided `compile.bash` script in the root directory to compile all files.
- If you are not on a Gnome Linux Desktop, run executables manually.

## Technologies Used

- C
- Linux Sockets
