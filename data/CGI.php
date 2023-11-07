#!/usr/bin/php-cgi
<?php
$name = isset($_POST['name']) ? $_POST['name'] : '';
$login = isset($_POST['login']) ? $_POST['login'] : '';

header('Content-Type: text/html; charset=utf-8');

echo <<<EOT
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CGI Response PHP</title>
    <style>
        .navbar .active > a {
            background-color: #343a40;
            color: white;
        }
    </style>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-KK94CHFLLe+nY2dmCWGMq91rCGa5gtU4mk92HdvYe+M/SXH301p5ILy+dN9+nJOZ" crossorigin="anonymous">
</head>
<body>
<nav class="navbar navbar-expand navbar-dark bg-dark">
    <a class="navbar-brand" href="#">webserv42</a>
    <ul class="navbar-nav">
        <li class="nav-item">
            <a class="nav-link" href="../index.html">Accueil</a>
        </li>
        <li class="nav-item">
            <a class="nav-link" href="../televerser.html">Téléverser</a>
        </li>
        <li class="nav-item">
            <a class="nav-link" href="../team.html">Notre équipe</a>
        </li>
        <li class="nav-item">
            <a class="nav-link" href="../cgi.html">CGI</a>
        </li>
    </ul>
</nav>
<h1>Test CGI PHP</h1>
<p>Hello World!</p>
EOT;

if (!empty($name)) {
    echo "<p>Your name is: $name</p>";
}
if (!empty($login)) {
    echo "<p>Your login is: $login</p>";
}

echo "<h3>Environments variables:</h3>";
foreach ($_SERVER as $key => $value) {
    echo "<li>$key: $value</li>";
}

echo '</body></html>';