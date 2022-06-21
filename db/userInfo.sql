CREATE TABLE IF NOT EXISTS user_general (
    firstname VARCHAR(50) NOT NULL ,
    surname VARCHAR(50) NOT NULL,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(50) NOT NULL,
    role INTEGER DEFAULT 0, /* 0 - common user , 1 - admin, 2 - temporary block, 3 - always block*/
    PRIMARY KEY(username)
    );

CREATE TABLE IF NOT EXISTS user_details (
    username VARCHAR(50) NOT NULL PRIMARY KEY REFERENCES user_general (username),
    appointment VARCHAR(50) NOT NULL, /* only admin can set appointment of worker */
    shift_start TIME NOT NULL, /* only admin can set shift start of worker */
    shift_finish TIME NOT NULL, /* only admin can set shift finish of worker */
    working_days VARCHAR(7) NOT NULL,
    street VARCHAR(50),
    city VARCHAR(50),
    zip_code VARCHAR(50),
    phone_number VARCHAR(25),
    gender VARCHAR(50)
    );

CREATE TABLE IF NOT EXISTS date_statistic (
                                             "date" DATE NOT NULL default CURRENT_DATE,
                                             PRIMARY KEY("date")
    );


CREATE TABLE IF NOT EXISTS user_intermediate_statistic (
    username  VARCHAR(50) NOT NULL,
    "date" DATE DEFAULT CURRENT_DATE NOT NULL,
    arrival_time TIME default '00:00:00' NOT NULL,
    finish_time TIME default '00:00:00' NOT NULL,
    attendance_status INTEGER DEFAULT 0, /* 0 - absent, 1 - late , 2 - shortcomings, 3 - in time, 4 - overwork, 5 - late_closed, 6 - in_time_closed */
    break_start TIME default '00:00:00' NOT NULL,
    break_finish TIME default '00:00:00' NOT NULL,
    count_worked_hour_and_minutes TIME default '00:00:00' NOT NULL, 
    FOREIGN KEY(username)
    REFERENCES user_general (username),
    FOREIGN KEY("date")
    REFERENCES date_statistic("date"),
    primary key (username, "date")
    );
    
    CREATE TABLE IF NOT EXISTS user_temporary_data (
    	username VARCHAR(50) NOT NULL,
    	"date" DATE DEFAULT CURRENT_DATE NOT NULL,
    	appointment VARCHAR(50) NOT NULL, /* only admin can set appointment of worker */
    	shift_start TIME NOT NULL, /* only admin can set shift start of worker */
    	shift_finish TIME NOT NULL,
    	working_days VARCHAR(7) NOT NULL,
    	FOREIGN KEY(username)
    	REFERENCES user_general (username),
    	FOREIGN KEY("date")
    	REFERENCES date_statistic("date")
    	);
