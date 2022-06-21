#!/bin/bash
psql -U pavlo -d testdb -c "update user_intermediate_statistic set attendance_status = 'k' where attendance_status = 'b';
			     update user_intermediate_statistic set attendance_status = 'd' where attendance_status = 'l';
			     update user_intermediate_statistic set attendance_status = 'i' where attendance_status = 'm';
			     "
