--
-- test.sql
--
create extension pg_iot;
--
create table t(x int);
--
insert into t values(1);
select * from t;
update t set x=2;
select * from t;
delete from t;
select * from t;
--
select iot.pg_iot_set('public','t');
insert into t values(1);
select * from t;
delete from t;
select * from t;
update t set x=2;
select * from t;
--
create table m(y int);
--
select iot.pg_iot_set('public','m');
merge into m as target
using t as source
on target.y = source.x 
when matched then
    update set y = 2
when not matched then
    insert (y)
    values(3);
--
truncate table t;
