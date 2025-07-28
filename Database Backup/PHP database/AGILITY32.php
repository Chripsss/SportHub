<?php
// MySQL database configuration
$servername = "192.168.0.199";
$username = "Tiara_21";
$password = "password";
$database = "sportscience";
$table = "`agility pole run`";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP GET request
$uid = $_GET['a'];
$time = $_GET['b'];
$speed = $_GET['c'];
$penalty = $_GET['d'];

// Prepare the SQL statement to insert data into the table
$sql = "INSERT INTO $table (ID, Time, Speed, Penalty) VALUES ('$uid', '$time', '$speed', '$penalty')";

// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>