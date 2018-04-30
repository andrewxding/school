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
            <h1>Add Movie Information</h1>
            <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
                <div class="form-group">
                    <label for="title">Title</label>
                    <input type="text" class="form-control" placeholder="" name="title">
                </div>
                <div class="form-group">
                    <label for="company">Company</label>
                    <input type="text" class="form-control" placeholder="" name="company">
                </div>
                <div class="form-group">
                    <label for="year">Year</label>
                    <input type="text" class="form-control" placeholder="yyyy" name="year">
                </div>
                <div class="form-group">
                    <label for="rating">MPAA Rating</label>
                    <select class="form-control" name="rating">
                        <option value="Default">Select one</option>
                        <option value="G">G</option>
                        <option value="NC-17">NC-17</option>
                        <option value="PG">PG</option>
                        <option value="PG-13">PG-13</option>
                        <option value="R">R</option>
                        <option value="surrendere">surrendere</option>
                    </select>
                </div>
                <div class="form-group">
                    <label>Movie Genre (select one or more):</label>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Action">Action</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Adult">Adult</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Adventure">Adventure</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Animation">Animation</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Comedy">Comedy</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Crime">Crime</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Documentary">Documentary</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Drama">Drama</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Family">Family</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Fantasy">Fantasy</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Horror">Horror</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Musical">Musical</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Mystery">Mystery</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Romance">Romance</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Short">Short</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Thriller">Thriller</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="War">War</div>
                    <div class="checkbox"><input type="checkbox" name="genre[]" value="Western">Western</div>
                </div>
                <button type="submit" class="btn btn-default" name="submit">Submit</button>
            </form>

            <hr>

            <?php
                define("YEAR_REGEX", "/\d{4}/");

                function valid_input() {
                    if (isset($_GET['submit'])) {
                        $valid_input = True;

                        if (empty($_GET['title'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a title for your movie.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['company'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a production company for your movie.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['year']) || !preg_match(YEAR_REGEX, $_GET['year'])) {
                            echo '<div class="alert alert-danger" role="alert">Please enter a valid year in the correct format.</div>';
                            $valid_input = False;
                        } else if ($_GET['rating'] == "Default") {
                            echo '<div class="alert alert-danger" role="alert">Please choose a rating for your movie.</div>';
                            $valid_input = False;
                        } else if (empty($_GET['genre'])) {
                            echo '<div class="alert alert-danger" role="alert">Please pick at least one genre for your movie.</div>';
                            $valid_input = False;
                        }

                        return $valid_input;
                    }
                }

                function insert_movie() {
                    $title  = $_GET['title'];
                    $company = $_GET['company'];
                    $year = $_GET['year'];
                    $rating = $_GET['rating'];
                    $genre = $_GET['genre'];

                    $db = new mysqli('localhost', 'cs143', '', 'CS143');

                    if ($db->connect_errno > 0) {
                        die('Unable to connect to database [' . $db->connect_error . ']');
                    }

                    $getMaxID = $db->query("SELECT id FROM MaxMovieID;");

                    if (!$getMaxID) {
                        echo '<div class="alert alert-danger" role="alert">Unable to retrieve the MaxMovieID: ' . $db->error . '</div>';
                        exit(1);
                    }

                    $newID = $getMaxID->fetch_assoc()[id] + 1;

                    $title = str_replace("'", "\'", $title);

                    $query = "INSERT INTO Movie (id, title, year, rating, company) VALUES ('$newID', '$title', '$year', '$rating', '$company');";

                    if (!($db->query($query))) {
                        echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                    } else {
                        if (!($db->query("UPDATE MaxMovieID SET id = id + 1;"))) {
                            echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                        }

                        echo '<div class="alert alert-success" role="alert">Success! Executed query: ' . $query . '</div>';

                        foreach ($genre as $value) {
                            $insertGenre = "INSERT INTO MovieGenre (mid, genre) VALUES ('$newID', '$value');";

                            if (!($db->query($insertGenre))) {
                                echo '<div class="alert alert-danger" role="alert">Failed to execute query: ' . $db->error . '</div>';
                            }

                            echo '<div class="alert alert-success" role="alert">Success! Executed query: ' . $insertGenre . '</div>';
                        }
                    }
                }

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    if (valid_input()) {
                        insert_movie();
                    }
                }
            ?>

        </div>

        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
