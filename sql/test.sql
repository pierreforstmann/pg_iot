--
-- test.sql
--
create extension pg_iot;
--
create table t(x int);
insert into t values(1);
select * from t;
update t set x=2;
select * from t;
delete from t;
select * from t;
--
select pg_iot_set('public','t');
insert into t values(1);
select * from t;
delete from t;
select * from t;
update t set x=2;
select * from t;

