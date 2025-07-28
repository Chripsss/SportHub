<?php
// MySQL database configuration
$servername = "192.168.0.199";
$username = "Tiara_21";
$password = "password";
$database = "sportscience";
$table = "sprint_new";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP GET request
$uid = $_GET['a'];
$time = $_GET['b'];
$avgspeed = $_GET['c'];
$topspeed = $_GET['d'];
$topacceleration = $_GET['e'];

// Set default value for Source
$source = "Right"; // You can change the default value here if needed.

// Prepare the SQL statement to insert data into the table
$sql = "INSERT INTO $table (ID, Time, AvgSpeed, TopSpeed, TopAcceleration, Source) VALUES ( '$uid', '$time', '$avgspeed','$topspeed','$topacceleration', '$source')";

// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>