PRAGMA foreign_keys=ON;
drop table if exists program ;
create table program(ptype string, name string, path string, length int);
drop table if exists program_time;
create table program_time(program_id int not null, dow int, hour int, minute int, live int);
drop table if exists program_instance;
create table program_instance(program_id int not null, ts datetime, int program_number);
begin;
insert into program values ("live", "Radioflautas", null, 120);
insert into program values ("live", "Tofuria", null, null);
insert into program values ("live", "Hard cuore", null, null);
insert into program values ("live", "Panorama cultureta", null, null);
insert into program values ("live", "5...", null, null);
insert into program values ("live", "Ponme otra", null, null);
insert into program values ("live", "Pobles de la mar", null, null);
insert into program values ("live", "Malva-Rosa Pop", null, null);

insert into program_time values ((select rowid from program where name = 'Radioflautas'), 1, 19, 0, 1);
insert into program_time values ((select rowid from program where name = 'Radioflautas'), 5, 23, 0, 0);
insert into program_time values ((select rowid from program where name = 'Tofuria'), 1, 12, 0, 1);
insert into program_time values ((select rowid from program where name = 'Tofuria'), 6, 3, 0, 0);
insert into program_time values ((select rowid from program where name = 'Hard cuore'), 2, 19, 00, 1);
insert into program_time values ((select rowid from program where name = 'Hard cuore'), 4, 5, 00, 0);
insert into program_time values ((select rowid from program where name = 'Hard cuore'), 7, 10, 00, 0);
insert into program_time values ((select rowid from program where name = '5...'), 3, 2, 00, 1);
insert into program_time values ((select rowid from program where name = '5...'), 4, 21, 00, 0);
insert into program_time values ((select rowid from program where name = '5...'), 5, 17, 00, 0);
insert into program_time values ((select rowid from program where name = 'Ponme otra'), 2, 15, 00, 0);
insert into program_time values ((select rowid from program where name = 'Ponme otra'), 5, 15, 00, 1);
insert into program_time values ((select rowid from program where name = 'Ponme otra'), 4, 4, 00, 0);
insert into program_time values ((select rowid from program where name = 'Pobles de la mar'), 2, 21, 00, 1);
insert into program_time values ((select rowid from program where name = 'Pobles de la mar'), 5, 11, 00, 0);
insert into program_time values ((select rowid from program where name = 'Pobles de la mar'), 1, 2, 00, 1);
insert into program_time values ((select rowid from program where name = 'Malva-Rosa Pop'), 2, 2, 00, 0);
insert into program_time values ((select rowid from program where name = 'Malva-Rosa Pop'), 2, 10, 00, 0);
insert into program_time values ((select rowid from program where name = 'Malva-Rosa Pop'), 3, 14, 00, 1);

commit;

select rowid,* from program;
select program.rowid,* from program_time inner join program on program.rowid = program_id;
-- #select rowid,* from program_time where ;




