--
-- pg_iot-0.0.1.sql
--
DROP FUNCTION IF EXISTS pg_iot_set();
--
CREATE FUNCTION pg_iot_set(cstring, cstring) RETURNS bool
 AS 'pg_iot', 'pg_iot_set'
 LANGUAGE C STRICT;
--
DROP TABLE IF EXISTS iot.tables;
--
DROP SCHEMA IF EXISTS iot;
--
CREATE SCHEMA iot;
--
CREATE TABLE iot.tables(
	namespace name NOT NULL,
        relname name NOT NULL,
	PRIMARY KEY(namespace, relname)
)
