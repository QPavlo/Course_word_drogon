#!/bin/bash

psql -U pavlo -d testdb -c "
 update user_details
 set shift_start  = temp_shift_start,
    shift_finish = temp_shift_finish,
    working_days = temp_working_days
 FROM (select * from user_temporary_data) as sub
 where user_details.username = sub.username;"
 
psql -U pavlo -d testdb -c " delete from user_temporary_data;"

psql -U pavlo -d testdb -c " update user_general set role = 1 where role = 5;"
 
psql -U pavlo -d testdb -c " insert into date_statistic values(CURRENT_DATE);"

psql -U pavlo -d testdb -c " insert into user_intermediate_statistic (username) 
 select ug.username from user_general ug
 inner join user_details ud on ud.username = ug.username
 where role = 1;"
 
psql -U pavlo -d testdb -c "update user_intermediate_statistic
set attendance_status = 'n'
where date = current_date and username in (select username
                   from user_general
                   where ((role = 1 or role = 5)
                       and (position
                           (cast((select extract(dow from date(current_date))) as varchar) in
                            cast((select working_days
                                  from user_details
                                  where user_general.username = user_details.username)
                                as varchar))) = 0));"
               
               
psql -U pavlo -d testdb -c " insert into user_temporary_data (username, temp_appointment, temp_shift_start, temp_shift_finish, temp_working_days)
 select username, appointment, shift_start, shift_finish, working_days from user_details;
" 

