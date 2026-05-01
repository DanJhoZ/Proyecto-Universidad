# 🎓 Proyecto Universidad (C++ + SQL Server)
Aplicación de consola en **C++** que gestiona alumnos, cursos y secciones usando **SQL Server** con conexión **ODBC**.
---
## ⚙️ Funcionalidades
* CRUD de:
  * Alumnos
  * Cursos
  * Secciones
* Asignación de cursos y sección a alumnos
* Eliminación con integridad (`ON DELETE CASCADE`)
* Menú interactivo en consola
---
## 🔌 Conexión a BD
Se realiza mediante **ODBC Driver 18 for SQL Server** usando:
* Servidor: `ZET\SQLEXPRESS`
* Base de datos: `universidad`
* Autenticación: Windows
  
El programa:

1. Crea el entorno ODBC
2. Abre la conexión (`SQLDriverConnect`)
3. Ejecuta consultas SQL
4. Muestra resultados o errores
---

## 📋 Menú
```text
1. Alumnos
2. Cursos
3. Secciones
4. Alumnos - Cursos
5. Alumnos - Sección
6. Salir
```

Cada módulo permite listar, insertar, actualizar y eliminar registros.

---

## ▶️ Uso

1. Ejecutar el script SQL
2. Configurar el servidor en el código
3. Compilar y ejecutar

---

## 🧠 Nota
Las relaciones usan `ON DELETE CASCADE`, por lo que al eliminar un registro, sus dependencias se eliminan automáticamente.
---
