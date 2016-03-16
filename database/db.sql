PRAGMA foreign_keys=ON;
drop table if exists program ;
create table program(ptype string, name string, path string, length int);
drop table if exists program_time;
create table program_time(program_id int not null, dow int, hour int, minute int, live int);
drop table if exists program_instance;
create table program_instance(program_id int not null, ts datetime, int program_number);
