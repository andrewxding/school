<html>
    <body>
        <h2>CS 143 Project 1 Part A</h2>
        <h3>Calculator</h3>

        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            <textarea name="query" rows=20 cols=60><?php
                if ($query = $_GET['query'])
                    echo $query;
            ?></textarea>
            <input type="submit" value="Submit">
        </form>

        <?php
            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $db = new mysqli('localhost', 'cs143', '', 'TEST');
                if($db->connect_errno > 0){
                    die('Unable to connect to database [' . $db->connect_error . ']');
                }
                $query = $_REQUEST['query'];
                $rs = $db->query($query);
                if (!$result) {
                    print "Query failed: $db->error<br />";
                    exit(1);
                }
                while($row = $rs->fetch_assoc()) {
                    $sid = $row['sid'];
                    $name = $row['name'];
                    $email = $row['email'];
                    echo "$sid, $name, $email<br />";
                }
                $rs->free();

                $output = $query;
                echo $output;
            }
        ?>
    </body>
</html>