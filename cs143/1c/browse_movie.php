<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta http-equiv="X-UA-Compatible" content="ie=edge">

        <title>CS 143 Project 1C</title>

        <link rel="stylesheet" type="text/css" href="bootstrap.min.css">
    </head>
    <body>
         <?php
            include_once('nav.php');
        ?>
        <div class="container">
            <?php
            include_once('movie_actor_search.php');
                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    $movieID = $_REQUEST['movieID'];
                    if (!empty($movieID)) {
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }
                        $query = "SELECT * FROM Movie m
                        LEFT JOIN (SELECT GROUP_CONCAT(genre SEPARATOR ', ') AS genres, mid FROM MovieGenre mg WHERE mid=$movieID) AS g ON m.id = g.mid
                        LEFT JOIN (SELECT GROUP_CONCAT(fullname SEPARATOR ', ') AS fullnames, mid FROM (SELECT CONCAT(first, ' ', last) AS fullname, mid FROM MovieDirector md INNER JOIN Director d ON md.did = d.id WHERE mid=$movieID ORDER BY last) AS name) AS names ON names.mid = m.id
                        WHERE m.id=" . $movieID;
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
                            echo "<h1>" . $row["title"] . "</h1>";
                        $colheaders = ["Year", "Rating", "Company", "Director(s)", "Genre(s)"];
                        echo "<h3>Details</h3>";
                        echo "<table class=\"table\"><tr>";
                        foreach ($colheaders as $header) {
                            echo "<th>" . $header . "</th>";
                        }
                        echo "</tr>";
                        while (true) {
                            echo "<tr>";
                            if (is_null($row["year"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["year"] . "</td>";
                            if (is_null($row["rating"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["rating"] . "</td>";
                            if (is_null($row["company"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["company"] . "</td>";
                            if (is_null($row["fullnames"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["fullnames"] . "</td>";
                            if (is_null($row["genres"]))
                                echo "<td>N/A</td>";
                            else
                                echo "<td>" . $row["genres"] . "</td>";
                            echo "</tr>";
                            
                            $row = $result->fetch_assoc();
                            if (!$row)
                                break;
                        }
                        echo "</table>";
                        $result->free();
                        $query = "SELECT * FROM MovieActor ma INNER JOIN Actor a ON ma.aid = a.id WHERE ma.mid=" . $movieID;
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }
                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h3>None of the actors in the database acted in this movie!</h3>";
                        }
                        else {
                            echo "<h3>Actors</h3>";
                            echo "<table class=\"table\"><tr>";
                            echo "<th>Name</th><th>Role</th>";
                            echo "</tr>";
                            while (true) {
                                echo "<tr>";
                                if (is_null($row["last"]) || is_null($row["first"]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td><a href=\"browse_actor.php?actorID=" . $row["aid"] . "\">" . $row["first"] . " " . $row["last"] . "</a></td>";
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
                        }
                        $result->free();
                        
                        echo "<h4><a href=\"add_comment.php?movieID=" . $movieID . "\">Leave a comment on this movie here!</a></h4><br>";
                        $query = "SELECT AVG(rating) AS avg FROM Review WHERE mid=" . $movieID . " GROUP BY mid";
                        $result = $db->query($query);
                    
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }
                        $row = $result->fetch_assoc();
                        if (!$row) {
                            echo "<h4>There are no comments!</h4>";
                            $comments = false;
                        }
                        else {
                            echo "<h4>Average user rating: " . number_format($row["avg"], 1) . "</h4>";
                            echo "<h3>Comments</h3>";
                            $comments = true;
                        }
                        $result->free();
                        if ($comments) {
                            $query = "SELECT * FROM Review WHERE mid=" . $movieID . " ORDER BY time DESC";
                            $result = $db->query($query);
                        
                            if (!$result) {
                                $errmsg = $db->error;
                                print "Query failed: $errmsg<br/>";
                                exit(1);
                            }
                            $row = $result->fetch_assoc();
                            while (true) {
                                echo "<h5>";
                                echo "<font color=\"red\">" . $row["name"] . "</font>" . " gave this movie a <font color=\"blue\">" . $row["rating"] . "</font>/5 rating";
                                echo " at " . $row["time"] . ", saying:<br>";
                                echo $row["comment"];
                                echo "</h5>";                            
                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                            echo "</table>";
                            $result->free();
                        }
                    }
                    else {
                        print("<h3>No movie ID specified.</h3>");
                    }
                }
            ?>
        </div>

        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
