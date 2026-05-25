# SQL-Dictionary 定仙游SQL
A lightweight SQL syntax study assistant tool developed with C++ and Qt 6.

Feature Overview
This application is an SQL syntax auxiliary query and management tool designed for developers to solve the efficiency issues associated with frequently looking up common SQL statements during development.

Core Features
Local Syntax Indexing and Retrieval: Supports categorized queries (Category) and name indexing (Name) of the SQL syntax library, allowing for quick location of required SQL statements.

Efficient Interaction Design: Integrates a "One-Click Copy" function that directly imports selected SQL code snippets into the system clipboard, effectively streamlining the development workflow.

Bilingual Support and Documentation: Each record includes detailed Chinese and English descriptions along with standardized code examples to meet technical reference needs in different environments.

Lightweight Deployment Architecture: Based on SQLite for data persistence, the entire program requires no installation and operates without a network connection, ensuring a "plug-and-play" experience across different development environments.

Technical Implementation Features
Data Security and Integrity: The program includes a built-in database initialization mechanism, ensuring that necessary table structures are automatically validated and created before execution.

Robust Error Handling: Strict exception validation is implemented for database connections and file reading, ensuring accurate feedback in the event of anomalies.

Standardized Build Process: Adheres to standard C++ development workflows, utilizing the Qt 6 Widgets framework for interface logic, with the build process managed by CMake.
