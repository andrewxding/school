<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta http-equiv="X-UA-Compatible" content="ie=edge">

        <title>CS 143 Project 1C</title>

        
    </head>
    <body>
        <?php
            // header navigation bar
            include_once('nav.php');
        ?>

        <div class="container">
            <h1>Add Actor/Director</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label class="radio-inline">
                        <input type="radio" name="occupation" value="Actor">Actor
                    </label>
                    <label class="radio-inline">
                        <input type="radio" name="occupation" value="Director">Director
                    </label>
                </div>
                <div class="form-group">
                    <label for="first_name">First Name</label>
                    <input type="text" class="form-control" placeholder="" name="first_name">
                </div>
                <div class="form-group">
                    <label for="last_name">Last Name</label>
                    <input type="text" class="form-control" placeholder="" name="last_name">
                </div>
                <div class="form-group">
                    <label class="radio-inline">
                        <input type="radio" name="gender" value="Male">Male
                    </label>
                    <label class="radio-inline">
                        <input type="radio" name="gender" value="Female">Female
                    </label>
                </div>
                <div class="form-group">
                    <label for="dob">Date of Birth</label>
                    <input type="text" class="form-control" placeholder="yyyy-mm-dd" name="dob">
                    <p class="help-block"></p>
                </div>
                <div class="form-group">
                    <label for="dod">Date of Death</label>
                    <input type="text" class="form-control" placeholder="yyyy-mm-dd" name="dod">
                    <p class="help-block">Leave blank if actor/director is still alive</p>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>

            <hr>

            <?php
                define("DATE_REGEX", "/\d{4}-\d{2}-\d{2}/");

                function valid_input() {
                    if (isset($_GET['submit'])) {
                        $valid_input = True;

                        if (empty($_GET['occupation'])) {
                            echo '<div class="alert alert-danger" role="alert">Please pick an occupation Actor/Director.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['first_name'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a first name.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['last_name'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a last name.</div>';
                            $valid_input = False;
                        } else if ($_GET['occupation'] == "Actor" && empty($_GET['gender'])) {
                            echo '<div class="alert alert-danger" role="alert">Please pick a gender for the desired actor.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['dob']) || !preg_match(DATE_REGEX, $_GET['dob'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a valid birth date in the correct format.</div>';
                            $valid_input = False;
                        } else if (!empty($_GET['dod']) && !preg_match(DATE_REGEX, $_GET['dod'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a valid death date in the correct format.</div>';
                            $valid_input = False;
                        }

                        return $valid_input;
                    }
                }

                function insert_actor_director() {
                    $occupation = $_GET['occupation'];
                    $first_name = $_GET['first_name'];
                    $last_name = $_GET['last_name'];
                    $gender = $_GET['gender'];
                    $dob = $_GET['dob'];
                    $dod = $_GET['dod'];

                    $db = new mysqli('localhost', 'cs143', '', 'CS143');

                    if ($db->connect_errno > 0) {
                        die('Unable to connect to database [' . $db->connect_error . ']');
                    }

                    $getMaxID = $db->query("SELECT id FROM MaxPersonID;");

                    if (!$getMaxID) {
                        echo '<div class="alert alert-danger" role="alert">Unable to retrieve the MaxPersonID: ' . $db->error . '</div>';
                        exit(1);
                    }

                    $newID = $getMaxID->fetch_assoc()[id] + 1;

                    $first_name = str_replace("'", "\'", $first_name);
                    $last_name = str_replace("'", "\'", $last_name);

                    if ($occupation == "Actor") {
                        $query = "INSERT INTO Actor (id, last, first, sex, dob, dod) VALUES ('$newID', '$last_name', '$first_name', '$gender', '$dob',";
                    } else {
                        $query = "INSERT INTO Director (id, last, first, dob, dod) VALUES ('$newID', '$last_name', '$first_name', '$dob',";
                    }

                    if (empty($dod)) {
                        $query = $query . " NULL);";
                    } else {
                        $query = $query . " '$dod');";
                    }

                    $executeQuery = $db->query($query);

                    if (!($db->query($query))) {
                        echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                    } else {
                        if (!($db->query("UPDATE MaxPersonID SET id = id + 1;"))) {
                            echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                        }

                        echo '<div class="alert alert-success" role="alert">Success! Executed query: ' . $query . '</div>';
                    }
                }

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    if (valid_input()) {
                        insert_actor_director();
                    }
                }
            ?>
        </div>

        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
