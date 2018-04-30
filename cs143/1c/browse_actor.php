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
            <?php
                include_once('movie_actor_search.php');
                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    $actorID = $_REQUEST['actorID'];
                    if (!empty($actorID)) {
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }
                        $query = "SELECT * FROM Actor WHERE id=" . $actorID;
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }
                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h1>No results!</h1>";
                            exit(1);
                        }
                        else
                            echo "<h1>" . $row["first"] . " " . $row["last"] . "</h1>";
                        $colheaders = ["Gender", "Date of Birth", "Date of Death"];
                        $coldbnames = ["sex", "dob", "dod"];
                        echo "<h3>Details</h3>";
                        echo "<table class=\"table\"><tr>";
                        foreach ($colheaders as $header) {
                            echo "<th>" . $header . "</th>";
                        }
                        echo "</tr>";
                        while (true) {
                            echo "<tr>";
                            foreach ($coldbnames as $name) {
                                if (is_null($row[$name]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td>" . $row[$name] . "</td>";
                            }
                            echo "</tr>";
                            
                            $row = $result->fetch_assoc();
                            if (!$row)
                                break;
                        }
                        echo "</table>";
                        $result->free();
                        $query = "SELECT * FROM MovieActor ma INNER JOIN Movie m ON ma.mid = m.id WHERE ma.aid=" . $actorID;
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }
                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h3>This actor did not act in any of the movies in the database!</h3>";
                            exit(1);
                        }
                        
                        echo "<h3>Filmography</h3>";
                        echo "<table class=\"table\"><tr>";
                        echo "<th>Movie</th><th>Role</th>";
                        echo "</tr>";
                        while (true) {
                            echo "<tr>";
                            if (is_null($row["title"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td><a href=\"browse_movie.php?movieID=" . $row["mid"] . "\">" . $row["title"] . "</a></td>";
                            if (is_null($row["role"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["role"] . "</td>";
                            echo "</tr>";
                            
                            $row = $result->fetch_assoc();
                            if (!$row)
                                break;
                        }
                        echo "</table>";
                        $result->free();
                    }
                }
            ?>
        </div>

        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
