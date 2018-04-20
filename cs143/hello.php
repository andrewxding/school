<html>
    <body>
        <h2>CS 143 Project 1 Part A</h2>
        <h3>Calculator</h3>

        <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
            Expression: <input type="text" name="expr">
            <input type="submit" value="Calculate">
        </form>

        <?php
            // Approach:
            $MATH_EXPRESSION_REGEX =  "^\-?\d+(\.\d+)?([\+\-\*\/]\-?\d+(\.\d+)?)*$";
            if ($_SERVER["REQUEST_METHOD"] == "GET") {
                $exp = $_REQUEST['expr'];
                if (!empty($exp)) {
                    $output = "Invalid Expression";
                    $expSpaced = preg_replace('/\s+/', ' ', $exp);
                    $expNone = preg_replace('/\s+/', '', $exp);
                    if (preg_match("/$MATH_EXPRESSION_REGEX/", $expNone)) {
                        // valid math expression            
                        if (!preg_match("/" . INVALID_ZEROES_REGEX . "/", $expSpaced)) {
                            // valid math expression without invalid leading zeroes before .
                            eval( '$output = (' . $expNone . ');' );
                            $output = $expSpaced . " = " . $output;
                        }
        
                    }
                    echo $output;
                }
            }
        ?>
    </body>
</html>