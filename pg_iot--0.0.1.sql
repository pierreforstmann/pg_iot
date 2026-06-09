--
-- pg_iot-0.0.1.sql
--

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_iot" to load this file. \quit

--
CREATE SCHEMA iot;
--
CREATE FUNCTION iot.pg_iot_set(cstring, cstring) RETURNS bool
 AS 'pg_iot', 'pg_iot_set'
 LANGUAGE C STRICT;
--
CREATE TABLE iot.tables(
	namespace name NOT NULL,
        relname name NOT NULL,
        reloid Oid NOT NULL,
	PRIMARY KEY(namespace, relname)
);
--
REVOKE EXECUTE ON FUNCTION iot.pg_iot_set FROM PUBLIC;
