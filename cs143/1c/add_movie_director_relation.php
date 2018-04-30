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
            include_once('nav.php');
        ?>

        <div class="container">
            <h1>Add Movie/Director Relation</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label for="mid">Movie Title</label>
                    <select class="form-control" name="mid">
                        <option value="Default">Select one</option>
                        <?php
                            $db = new mysqli('localhost', 'cs143', '', 'CS143');

                            if ($db->connect_errno > 0) {
                                die('Unable to connect to database [' . $db->connect_error . ']');
                            }

                            $query = "SELECT id, title, year FROM Movie;";

                            $execute_query = $db->query($query);

                            if (!$execute_query) {
                                echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                            }

                            $option = $execute_query->fetch_assoc();

                            while ($option) {
                                echo '<option value="' . $option['id'] . '">' . $option['title'] . ' (' . $option['year'] . ')</option>';
                                $option = $execute_query->fetch_assoc();
                            }
                        ?>
                    </select>
                </div>
                <div class="form-group">
                    <label for="did">Director</label>
                    <select class="form-control" name="did">
                        <option value="Default">Select one</option>
                        <?php
                            $db = new mysqli('localhost', 'cs143', '', 'CS143');

                            if ($db->connect_errno > 0) {
                                die('Unable to connect to database [' . $db->connect_error . ']');
                            }

                            $query = "SELECT id, first, last, dob FROM Director;";

                            $execute_query = $db->query($query);

                            if (!$execute_query) {
                                echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                            }

                            $option = $execute_query->fetch_assoc();

                            while ($option) {
                                echo '<option value="' . $option['id'] . '">' . $option['first'] . ' ' . $option['last'] . ' (' . $option['dob'] . ')</option>';
                                $option = $execute_query->fetch_assoc();
                            }
                        ?>
                    </select>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>

            <hr>

            <?php
                function valid_input() {
                    if (isset($_GET['submit'])) {
                        $valid_input = True;

                        if ($_GET['mid'] == "Default") {
                            echo '<div class="alert alert-danger" role="alert">Please pick a movie.</div>';
                            $valid_input = False;
                        } else if ($_GET['did'] == "Default") {
                            echo '<div class="alert alert-danger" role="alert">Please pick an actor.</div>';
                            $valid_input = False;
                        }

                        return $valid_input;
                    }
                }

                function insert_relation() {
                    $mid  = $_GET['mid'];
                    $aid = $_GET['did'];

                    $db = new mysqli('localhost', 'cs143', '', 'CS143');

                    if ($db->connect_errno > 0) {
                        die('Unable to connect to database [' . $db->connect_error . ']');
                    }

                    $query = "INSERT INTO MovieDirector (mid, did) VALUES ('$mid', '$did');";

                    if (!($db->query($query))) {
                        echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                    } else {
                        echo '<div class="alert alert-success" role="alert">Success! Executed query: ' . $query . '</div>';
                    }
                }

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    if (valid_input()) {
                        insert_relation();
                    }
                }
            ?>
        </div>

        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
