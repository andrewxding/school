<html>
    <head>
        <title>Project 1B Web Query Interface</title>
    </head>

    <body>
        <h2>CS 143 Project 1 Part B</h2>
        <h3>Web Query Interface</h3>

        <p>Query:</p>
        <form method="get" action="<?php print $_SERVER['PHP_SELF'];?>">
            <textarea name="query" rows=20 cols=60><?php
                if ($query = $_GET['query'])
                    print $query;
            ?></textarea>
            <input type="submit" Value="Submit">
        </form>

        <?php
            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $query = $_REQUEST['query'];
                if (!empty($query)) {
                    $db = new mysqli('localhost', 'cs143', '', 'CS143');
                    if ($db->connect_errno > 0) {
                        die('Unable to connect to database [' . $db->connect_error . ']');
                    }

                    if (!$result = $db->query($query)) {
                        $errmsg = $db->error;
                        print "Query failed: $errmsg<br />";
                        exit(1);
                    }
                    
                    
                    print "<h3>Results from MySQL:</h3>";
                    print "<table border=1 cellspacing=1 cellpadding=2><tr>";
                    $fields = array();
                    while ($property = mysqli_fetch_field($result)) {
                        array_push($fields, $property->name);
                        print "<th>" . $property->name . "</th>";
                    }

                    print "</tr>";
                    while ($row = $result->fetch_assoc()) {
                        print "<tr>";
                        foreach($fields as $col) {
                            if (is_null($row[$col]))
                                print "<td>N/A</td>";
                            else
                                print "<td>" . $row[$col] . "</td>";
                        }
                        print "</tr>";
                        
                        $row = $result->fetch_assoc();
                        // if (!$row)
                        //     break;
                    }
                    print "</table>";
                    $result->free();
                }
            }
        ?>
    </body>
</html>