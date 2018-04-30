<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>

    <body>
        <h1>Search Actors and Movies</h1>
        <form method="get" class="navbar-form navbar-right" action="search_actor_movie.php?query=$_REQUEST['query']">
            <div class="form-group">
                <input type="text" name="query" class="form-control" placeholder="Search...">
            </div>
            <button type="submit" class="btn btn-primary">Search</button>
        </form>
    </body>
</html>