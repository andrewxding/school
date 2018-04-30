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
                    $searchquery = $_REQUEST['query'];
                    if (!empty($searchquery)) {
                        $sanitized = preg_replace('!\s+!', ' ', $searchquery);
                        $terms = explode(" ", $sanitized);
                        $db = new mysqli('localhost', 'cs143', '', 'CS143');
                        if ($db->connect_errno > 0) {
                            die('Unable to connect to database [' . $db->connect_error . ']');
                        }
                        echo "<h1>" . "Search results for \"" . $searchquery . "\":</h1>";
                        $query = "SELECT * FROM Movie WHERE ";
                        $query_middle = "title LIKE '%%%s%%' ";
                        $query_continuation = "AND ";
                        $query_end = "ORDER BY title";
                        foreach($terms as $term) {
                            if (IS_NULL($movie_query_first)) {
                                $query = $query . sprintf($query_middle, $term);
                                $movie_query_first = true;
                            }
                            else
                                $query = $query . $query_continuation . sprintf($query_middle, $term);                            
                        }
                        $query = $query . $query_end;
                        $result = $db->query($query);
                        if (!$result) {
                            $errmsg = $db->error;
                            print "Query failed: $errmsg<br/>";
                            exit(1);
                        }
                        $row = $result->fetch_assoc();
                        
                        if (!$row) {
                            echo "<h3>No movies match the given query!</h3>";
                        }
                        else {
                            $colheaders = ["Title", "Year"];
                            echo "<h3>Movies</h3>";
                            echo "<table class=\"table\"><tr>";
                            foreach ($colheaders as $header) {
                                echo "<th>" . $header . "</th>";
                            }
                            echo "</tr>";
                            while (true) {
                                echo "<tr>";
                                if (is_null($row["title"]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td><a href=\"browse_movie.php?movieID=" . $row["id"] . "\">" . $row["title"] . "</a></td>";
                                if (is_null($row["year"]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td>" . $row["year"] . "</td>";
                                echo "</tr>";
                                
                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                            echo "</table>";
                        }
                        $result->free();
                        
                        $query = "SELECT *, CONCAT(first, ' ', last) AS fullname FROM Actor WHERE ";
                        $query_middle = "CONCAT(first, ' ', last) LIKE '%%%s%%' ";
                        $query_continuation = "AND ";
                        $query_end = "ORDER BY last";
                        foreach($terms as $term) {
                            if (IS_NULL($actor_query_first)) {
                                $query = $query . sprintf($query_middle, $term);
                                $actor_query_first = true;
                            }
                            else
                                $query = $query . $query_continuation . sprintf($query_middle, $term);                            
                        }
                        
                        $query = $query . $query_end;
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
                            echo "<th>Name</th><th>Date of Birth</th>";
                            echo "</tr>";
                            while (true) {
                                echo "<tr>";
                                if (is_null($row["fullname"]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td><a href=\"browse_actor.php?actorID=" . $row["id"] . "\">" . $row["fullname"] . "</a></td>";
                                if (is_null($row["dob"]))
                                    echo "<td>N/A</td>";
                                else
                                    echo "<td>" . $row["dob"] . "</td>";
                                echo "</tr>";
                                
                                $row = $result->fetch_assoc();
                                if (!$row)
                                    break;
                            }
                            echo "</table>";
                        }
                        $result->free();
                    }
                    else {
                        print("<h1>No search query given!</h1>");
                    }
                }
            ?>
        </div>
        <script src="jquery.min.js"></script>
        <script src="bootstrap.min.js"></script>
        <script src="bootstrap.bundle.min.js"></script>
    </body>
</html>
