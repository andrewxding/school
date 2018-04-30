-- This violates the primary key constraint because primary key must be unique
-- ERROR 1062 (23000) at line 4: Duplicate entry '1' for key 'PRIMARY'
INSERT INTO Movie
    VALUES (1, 'Star Wars', 1977, 5, 'Lucas Films');
INSERT INTO Movie
    VALUES (1, 'Inception', 2010, 5, 'Warner Bros');

-- This violates the primary key constraint because primary key must be unique
-- ERROR 1062 (23000) at line 8: Duplicate entry '13' for key 'PRIMARY'
INSERT INTO Actor
    VALUES (13, 'Damon', 'Matt', 'Male', '1970-10-08', NULL);
INSERT INTO Actor
    VALUES (13, 'Downey Jr', 'Robert', 'Male', '1965-4-4', NULL);

-- This violates the primary key constraint because primary key must be unique
-- ERROR 1062 (23000) at line 18: Duplicate entry '99' for key 'PRIMARY'
INSERT INTO Director
    VALUES (99, 'Nolan', 'Christopher', '1970-07-30', NULL);
INSERT INTO Director
    VALUES (99, 'Nolan', 'Christopher', '1970-07-30', NULL);

-- This violates referential integrity constraint because there's no Movie with id = 314
-- Error 1452 (23000) at line 26: Cannot add or update a child row: a foregin key constraint fails
-- (`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Movie
    WHERE id = 314;
INSERT INTO MovieGenre
    VALUES (314, 'Comedy');

-- This violates referential integrity constraint because there's no Movie with id = 222
-- Error 1451 (23000) at line 33: Cannot delete or update a parent row: a foregin key constraint fails
-- (`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Movie
    WHERE id = 222;
INSERT INTO MovieDirector
    VALUES (222, 12345);

-- This violates referential integrity constraint because there's no Director with id = 333
-- Error 1452 (23000) at line 43: Cannot add or update a child row: a foregin key constraint fails
-- (`CS143`.`MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Director
    WHERE id = 333;
INSERT INTO MovieDirector
    VALUES (12345, 333);

-- This violates referential integrity constraint because there's no Movie with id = 444
-- Error 1451 (23000) at line 49: Cannot delete or update a parent row: a foregin key constraint fails
-- (`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Movie
    WHERE id = 444;
INSERT INTO MovieActor
    VALUES (444, 12345, 'Main Actor');

-- This violates referential integrity constraint because there's no Actor with id = 555
-- Error 1452 (23000) at line 59: Cannot add or update a child row: a foregin key constraint fails
-- (`CS143`.`MovieActor`, CONSTRAINT `MovieActor_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Actor
    WHERE id = 555;
INSERT INTO MovieActor
    VALUES (12345, 555, 'Main Actor');

-- This violates referential integrity constraint because there's no Movie with id = 666
-- Error 1451 (23000) at line 65: Cannot delete or update a parent row: a foregin key constraint fails
-- (`CS143`.`MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
DELETE FROM Movie
    WHERE id = 666;
INSERT INTO Review
    VALUES ('Messi', CURRENT_TIMESTAMP, 666, 5, 'Great movie!!');

-- This violates the CHECK constraint because we specified that dob has to be before or equal to current date
INSERT INTO Actor
    VALUES (1234, 'Burrito', 'Chipotle', 'N/A', '2018-06-10', NULL);

-- This violates the CHECK constraint because date of birth should be before or equal to date of death
INSERT INTO Director
    VALUES (1111, 'World', 'Hello', '2016-01-01', '2015-01-01');

-- This violates the CHECK constraint because rating has to be between 0 and 5 inclusive
INSERT INTO Review
    VALUES ('Yo', CURRENT_TIMESTAMP, 123, 6, 'This is a comment');