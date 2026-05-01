#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <cstring>

using namespace std;

SQLHENV hEnv;

// =====================
// ERROR
// =====================
void mostrarError(SQLSMALLINT tipo, SQLHANDLE handle) {
    SQLCHAR sqlState[6], message[256];
    SQLINTEGER nativeError;
    SQLSMALLINT textLength;

    if (SQLGetDiagRecA(tipo, handle, 1, sqlState, &nativeError,
        message, sizeof(message), &textLength) == SQL_SUCCESS) {
        cout << "Error: " << message << endl;
    }
}

// =====================
// CONEXION
// =====================
SQLHDBC conectar() {
    SQLHDBC hDbc;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    SQLCHAR connStr[] =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=ZET\\SQLEXPRESS;"
        "DATABASE=universidad;"
        "Trusted_Connection=yes;"
        "TrustServerCertificate=yes;";

    SQLRETURN ret = SQLDriverConnectA(
        hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT
    );

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        cout << "Conectado\n";
        return hDbc;
    }

    mostrarError(SQL_HANDLE_DBC, hDbc);
    return NULL;
}

// =====================
// ALUMNOS
// =====================
void listarAlumnos(SQLHDBC hDbc) {
    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] =
        "SELECT a.idAlumno, a.nombres, a.apellidos, a.carnet, "
        "ISNULL(c.nombre,'Sin cursos'), ISNULL(s.nombre,'Sin seccion') "
        "FROM alumnos a "
        "LEFT JOIN alumnos_cursos ac ON a.idAlumno = ac.idAlumno "
        "LEFT JOIN cursos c ON ac.idCurso = c.idCurso "
        "LEFT JOIN alumnos_seccion ase ON a.idAlumno = ase.idAlumno "
        "LEFT JOIN secciones s ON ase.idSeccion = s.idSeccion";

    if (SQLExecDirectA(hStmt, query, SQL_NTS) != SQL_SUCCESS) {
        mostrarError(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id;
    SQLCHAR n[100], a[100], c[50], curso[100], seccion[100];

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, n, sizeof(n), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, a, sizeof(a), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, c, sizeof(c), NULL);
        SQLGetData(hStmt, 5, SQL_C_CHAR, curso, sizeof(curso), NULL);
        SQLGetData(hStmt, 6, SQL_C_CHAR, seccion, sizeof(seccion), NULL);

        cout << id << " | " << n << " " << a
            << " | " << c
            << " | Curso: " << curso
            << " | Seccion: " << seccion
            << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarAlumno(SQLHDBC hDbc) {
    char n[100], a[100], c[50];

    cout << "Nombre: "; cin >> n;
    cout << "Apellido: "; cin >> a;
    cout << "Carnet: "; cin >> c;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"INSERT INTO alumnos(nombres,apellidos,carnet) VALUES(?,?,?)",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, n, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, a, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, c, 0, NULL);

    if (SQLExecute(hStmt) != SQL_SUCCESS)
        mostrarError(SQL_HANDLE_STMT, hStmt);
    else
        cout << "Insertado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarAlumno(SQLHDBC hDbc) {
    int id;
    char n[100], a[100], c[50];

    cout << "ID: "; cin >> id;
    cout << "Nuevo nombre: "; cin >> n;
    cout << "Nuevo apellido: "; cin >> a;
    cout << "Nuevo carnet: "; cin >> c;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"UPDATE alumnos SET nombres=?, apellidos=?, carnet=? WHERE idAlumno=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, n, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, a, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, c, 0, NULL);
    SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Actualizado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarAlumno(SQLHDBC hDbc) {
    int id;
    cout << "ID: "; cin >> id;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM alumnos WHERE idAlumno=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Eliminado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =====================
// CURSOS
// =====================
void listarCursos(SQLHDBC hDbc) {
    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLExecDirectA(hStmt, (SQLCHAR*)"SELECT * FROM cursos", SQL_NTS);

    int id; char nombre[100], codigo[50];

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombre, sizeof(nombre), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, codigo, sizeof(codigo), NULL);

        cout << id << " | " << nombre << " | " << codigo << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarCurso(SQLHDBC hDbc) {
    char nombre[100], codigo[50];

    cout << "Nombre: "; cin >> nombre;
    cout << "Codigo: "; cin >> codigo;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt, (SQLCHAR*)"INSERT INTO cursos(nombre,codigo) VALUES(?,?)", SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, nombre, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, codigo, 0, NULL);

    SQLExecute(hStmt);
    cout << "Curso agregado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarCurso(SQLHDBC hDbc) {
    int id;
    char nombre[100], codigo[50];

    cout << "ID Curso: "; cin >> id;
    cout << "Nuevo nombre: "; cin >> nombre;
    cout << "Nuevo codigo: "; cin >> codigo;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"UPDATE cursos SET nombre=?, codigo=? WHERE idCurso=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, nombre, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, codigo, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Curso actualizado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarCurso(SQLHDBC hDbc) {
    int id;
    cout << "ID Curso: "; cin >> id;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM cursos WHERE idCurso=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Curso eliminado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =====================
// SECCIONES
// =====================
void listarSecciones(SQLHDBC hDbc) {
    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLExecDirectA(hStmt, (SQLCHAR*)"SELECT * FROM secciones", SQL_NTS);

    int id; char nombre[100];

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombre, sizeof(nombre), NULL);

        cout << id << " | " << nombre << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarSeccion(SQLHDBC hDbc) {
    char nombre[100];
    cout << "Nombre: "; cin >> nombre;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"INSERT INTO secciones(nombre) VALUES(?)",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, nombre, 0, NULL);

    SQLExecute(hStmt);
    cout << "Seccion agregada\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarSeccion(SQLHDBC hDbc) {
    int id;
    char nombre[100];

    cout << "ID: "; cin >> id;
    cout << "Nuevo nombre: "; cin >> nombre;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"UPDATE secciones SET nombre=? WHERE idSeccion=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, nombre, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Seccion actualizada\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarSeccion(SQLHDBC hDbc) {
    int id;
    cout << "ID: "; cin >> id;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM secciones WHERE idSeccion=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << "Seccion eliminada\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =====================
// RELACIONES
// =====================
void asignarCurso(SQLHDBC hDbc) {
    int idA, idC;

    cout << "ID Alumno: "; cin >> idA;
    cout << "ID Curso: "; cin >> idC;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"IF NOT EXISTS(SELECT 1 FROM alumnos_cursos WHERE idAlumno=? AND idCurso=?) "
        "INSERT INTO alumnos_cursos VALUES(?,?)",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idA, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idC, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idA, 0, NULL);
    SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idC, 0, NULL);

    SQLExecute(hStmt);
    cout << "Curso asignado\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void listarAsignaciones(SQLHDBC hDbc) {
    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLExecDirectA(hStmt,
        (SQLCHAR*)"SELECT a.nombres, c.nombre FROM alumnos a "
        "JOIN alumnos_cursos ac ON a.idAlumno=ac.idAlumno "
        "JOIN cursos c ON ac.idCurso=c.idCurso",
        SQL_NTS);

    char alumno[100], curso[100];

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, alumno, sizeof(alumno), NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, curso, sizeof(curso), NULL);

        cout << alumno << " -> " << curso << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarAsignacion(SQLHDBC hDbc) {
    int idA, idC;

    cout << "ID Alumno: "; cin >> idA;
    cout << "ID Curso: "; cin >> idC;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM alumnos_cursos WHERE idAlumno=? AND idCurso=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idA, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idC, 0, NULL);

    SQLExecute(hStmt);
    cout << "Asignacion eliminada\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void asignarSeccion(SQLHDBC hDbc) {
    int idA, idS;

    cout << "ID Alumno: "; cin >> idA;
    cout << "ID Seccion: "; cin >> idS;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM alumnos_seccion WHERE idAlumno=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idA, 0, NULL);
    SQLExecute(hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"INSERT INTO alumnos_seccion VALUES(?,?)",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idA, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idS, 0, NULL);

    SQLExecute(hStmt);
    cout << " Seccion asignada\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void listarAlumnoSeccion(SQLHDBC hDbc) {
    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLExecDirectA(hStmt,
        (SQLCHAR*)"SELECT a.nombres, s.nombre FROM alumnos a "
        "JOIN alumnos_seccion ase ON a.idAlumno=ase.idAlumno "
        "JOIN secciones s ON ase.idSeccion=s.idSeccion",
        SQL_NTS);

    char alumno[100], seccion[100];

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, alumno, sizeof(alumno), NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, seccion, sizeof(seccion), NULL);

        cout << alumno << " -> " << seccion << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarSeccionAlumno(SQLHDBC hDbc) {
    int id;
    cout << "ID Alumno: "; cin >> id;

    SQLHSTMT hStmt; SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLPrepareA(hStmt,
        (SQLCHAR*)"DELETE FROM alumnos_seccion WHERE idAlumno=?",
        SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmt);
    cout << " Seccion eliminada del alumno\n";

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =====================
// MENUS
// =====================
void menuAlumnos(SQLHDBC hDbc) {
    int op;
    do {
        cout << "\n=== ALUMNOS ===\n1.Listar\n2.Insertar\n3.Actualizar\n4.Eliminar\n5.Regresar\n";
        cin >> op;

        switch (op) {
        case 1: listarAlumnos(hDbc); break;
        case 2: insertarAlumno(hDbc); break;
        case 3: actualizarAlumno(hDbc); break;
        case 4: eliminarAlumno(hDbc); break;
        }
    } while (op != 5);
}

void menuCursos(SQLHDBC hDbc) {
    int op;
    do {
        cout << "\n=== CURSOS ===\n1.Listar\n2.Insertar\n3.Actualizar\n4.Eliminar\n5.Regresar\n";
        cin >> op;

        switch (op) {
        case 1: listarCursos(hDbc); break;
        case 2: insertarCurso(hDbc); break;
        case 3: actualizarCurso(hDbc); break;
        case 4: eliminarCurso(hDbc); break;
        }
    } while (op != 5);
}

void menuSecciones(SQLHDBC hDbc) {
    int op;
    do {
        cout << "\n=== SECCIONES ===\n1.Listar\n2.Insertar\n3.Actualizar\n4.Eliminar\n5.Regresar\n";
        cin >> op;

        switch (op) {
        case 1: listarSecciones(hDbc); break;
        case 2: insertarSeccion(hDbc); break;
        case 3: actualizarSeccion(hDbc); break;
        case 4: eliminarSeccion(hDbc); break;
        }
    } while (op != 5);
}

void menuAlumnosCursos(SQLHDBC hDbc) {
    int op;
    do {
        cout << "\n=== ALUMNOS - CURSOS ===\n1.Asignar\n2.Listar\n3.Eliminar\n4.Regresar\n";
        cin >> op;

        switch (op) {
        case 1: asignarCurso(hDbc); break;
        case 2: listarAsignaciones(hDbc); break;
        case 3: eliminarAsignacion(hDbc); break;
        }
    } while (op != 4);
}

void menuAlumnosSeccion(SQLHDBC hDbc) {
    int op;
    do {
        cout << "\n=== ALUMNOS - SECCION ===\n1.Asignar\n2.Listar\n3.Cambiar\n4.Eliminar\n5.Regresar\n";
        cin >> op;

        switch (op) {
        case 1: asignarSeccion(hDbc); break;
        case 2: listarAlumnoSeccion(hDbc); break;
        case 3: asignarSeccion(hDbc); break;
        case 4: eliminarSeccionAlumno(hDbc); break;
        }
    } while (op != 5);
}

// =====================
// MAIN
// =====================
int main() {
    SQLHDBC hDbc = conectar();
    if (!hDbc) return 1;

    int op;
    do {
        cout << "\n===== SISTEMA UNIVERSIDAD =====\n";
        cout << "1. Alumnos\n";
        cout << "2. Cursos\n";
        cout << "3. Secciones\n";
        cout << "4. Alumnos - Cursos\n";
        cout << "5. Alumnos - Seccion\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> op;

        switch (op) {
        case 1: menuAlumnos(hDbc); break;
        case 2: menuCursos(hDbc); break;
        case 3: menuSecciones(hDbc); break;
        case 4: menuAlumnosCursos(hDbc); break;
        case 5: menuAlumnosSeccion(hDbc); break;
        }

    } while (op != 6);

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    cout << "\nSistema finalizado correctamente.\n";
    return 0;
}