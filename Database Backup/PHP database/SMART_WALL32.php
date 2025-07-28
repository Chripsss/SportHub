<?php
// MySQL database configuration
$servername = "192.168.0.199";
$username = "Tiara_21";
$password = "password";
$database = "sportscience";
$table = "smart_wall";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP GET request
$ID = $_GET['a'];
$Point = $_GET['b'];
$Fastest = $_GET['c'];
$Slowest = $_GET['d'];
$Average = $_GET['e'];
$Accuracy = $_GET['f'];


// Prepare the SQL statement to insert data into the table

$sql = "INSERT INTO $table (ID, Point, Fastest, Slowest, Average, Accuracy) VALUES ('$ID', '$Point', '$Fastest', '$Slowest', '$Average', '$Accuracy')";

// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>