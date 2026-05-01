-- =========================
-- ELIMINAR BD SI EXISTE
-- =========================
IF DB_ID('universidad') IS NOT NULL
BEGIN
    DROP DATABASE universidad;
END
GO

-- =========================
-- CREAR BASE DE DATOS
-- =========================
CREATE DATABASE universidad;
GO

USE universidad;
GO

-- =========================
-- TABLAS
-- =========================

CREATE TABLE alumnos (
    idAlumno INT PRIMARY KEY IDENTITY(1,1),
    nombres VARCHAR(255) NOT NULL,
    apellidos VARCHAR(255) NOT NULL,
    carnet VARCHAR(25) NOT NULL
);
GO

CREATE TABLE cursos (
    idCurso INT PRIMARY KEY IDENTITY(1,1),
    nombre VARCHAR(255) NOT NULL,
    codigo VARCHAR(255) NOT NULL
);
GO

CREATE TABLE secciones (
    idSeccion INT PRIMARY KEY IDENTITY(1,1),
    nombre VARCHAR(255) NOT NULL,
    jornada VARCHAR(255) NOT NULL
);
GO

-- =========================
-- TABLA RELACION ALUMNOS - CURSOS
-- =========================
CREATE TABLE alumnos_cursos (
    id INT PRIMARY KEY IDENTITY(1,1),
    idAlumno INT NOT NULL,
    idCurso INT NOT NULL,

    CONSTRAINT FK_AC_Alumnos 
        FOREIGN KEY (idAlumno) 
        REFERENCES alumnos(idAlumno)
        ON DELETE CASCADE,

    CONSTRAINT FK_AC_Cursos 
        FOREIGN KEY (idCurso) 
        REFERENCES cursos(idCurso)
        ON DELETE CASCADE
);
GO

-- =========================
-- TABLA RELACION ALUMNOS - SECCION
-- =========================
CREATE TABLE alumnos_seccion (
    id INT PRIMARY KEY IDENTITY(1,1),
    idAlumno INT NOT NULL,
    idSeccion INT NOT NULL,

    CONSTRAINT FK_AS_Alumnos 
        FOREIGN KEY (idAlumno) 
        REFERENCES alumnos(idAlumno)
        ON DELETE CASCADE,

    CONSTRAINT FK_AS_Seccion 
        FOREIGN KEY (idSeccion) 
        REFERENCES secciones(idSeccion)
        ON DELETE CASCADE,

    -- Evita que un alumno tenga más de una sección
    CONSTRAINT UQ_Alumno UNIQUE (idAlumno)
);
GO

-- =========================
-- INSERTAR DATOS
-- =========================

INSERT INTO alumnos (nombres, apellidos, carnet)
VALUES
('Daniel', 'Zet', '9989-25-25824'),
('Franklin', 'Hernandez', '9989-25-23325'),
('Abigail', 'Lastro', '9989-25-19162'),
('Paola', 'Contreras', '9989-25-21132');
GO

INSERT INTO cursos (nombre, codigo)
VALUES
('Derecho Informatico', '001'),
('Calculo 1', '002'),
('Proceso Administrativo', '003'),
('Programación 1', '004'),
('Fisica 1', '005');
GO

INSERT INTO secciones (nombre, jornada)
VALUES
('A', 'Sábado'),
('B', 'Sábado'),
('C', 'Sábado'),
('D', 'Sábado'),
('E', 'Sábado'),
('F', 'Sábado'),
('G', 'Sábado'),
('H', 'Sábado');
GO

INSERT INTO alumnos_cursos (idAlumno, idCurso)
VALUES
(1,2),
(1,1),
(3,1);
GO

INSERT INTO alumnos_seccion (idAlumno, idSeccion)
VALUES
(1,5),
(2,4),
(3,1),
(4,7);
GO

-- =========================
-- CONSULTAS
-- =========================

SELECT * FROM alumnos;
SELECT * FROM cursos;
SELECT * FROM secciones;
SELECT * FROM alumnos_cursos;
SELECT * FROM alumnos_seccion;
GO

-- =========================
-- PRUEBAS DE ELIMINACION
-- =========================

-- Eliminar alumno (también borra sus relaciones automáticamente)
-- DELETE FROM alumnos WHERE idAlumno = 1;

-- Eliminar curso (también borra relaciones automáticamente)
-- DELETE FROM cursos WHERE idCurso = 1;

-- Eliminar sección (borra relación del alumno)
-- DELETE FROM secciones WHERE idSeccion = 1;
GO

-- =========================
-- CONSULTA EXTRA
-- =========================

-- Alumnos que NO tienen cursos asignados
SELECT 
    a.idAlumno, 
    a.nombres, 
    a.apellidos, 
    a.carnet
FROM alumnos a
LEFT JOIN alumnos_cursos ac ON a.idAlumno = ac.idAlumno
WHERE ac.idCurso IS NULL;
GO