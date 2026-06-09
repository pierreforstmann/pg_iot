## pg_iot

pg_iot is a PostgreSQL extension which allows to define table as INSERT only table (IOT):
- only INSERT and SELECT statements are allowed
- UPDATE and DELETE statements are forbidden.


### Installation

Extension must be loaded at server level with ```shared_preload_libraries``` parameter:

```
shared_preload_libraries = 'pg_iot'
```

Stop and restart PostgreSQL instance and create the extension with:

```
create extension pg_iot;
```

### Usage

Connect as superuser and flag table as IOT with ``pg_iot_set`` function giving schema name and table name as parameters:

```
select iot.pg_iot_set('public','t');
```

Trying to run UPDATE or DELETE on IOT table raise errors:
```
delete from t;
ERROR:  pg_iot: UPDATE and DELETE operations are disabled on table "public.t"
update t set x=2;
ERROR:  pg_iot: UPDATE and DELETE operations are disabled on table "public.t"
```

IOT tables are cataloged in ```iot.tables```:

```
# select * from iot.tables;
 namespace | relname 
-----------+---------
 public    | t
(1 row)
```
## Limitations

Currently there is no way to move an IOT table to a non-IOT table.

ALTER TABLE statements for an IOT table are not defined.

