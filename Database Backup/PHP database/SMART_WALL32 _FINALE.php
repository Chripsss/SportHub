<?php
// MySQL database configuration
$servername = "localhost";
$username = "root";
$password = "";
$database = "sportscience";
$table = "smart_wall";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP GET request
$uid = $_GET['a'];
$reactionTime = $_GET['b'];
$right = $_GET['c'];


// Prepare the SQL statement to insert data into the table

$sql = "INSERT INTO $table (ID,ReactionTime, Correct) VALUES ('$uid', '$reactionTime', '$right')";

// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>