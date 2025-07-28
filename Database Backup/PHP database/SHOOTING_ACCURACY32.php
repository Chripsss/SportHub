<?php
// MySQL database configuration
$servername = "192.168.0.199";
$username = "Tiara_21";
$password = "password";
$database = "sportscience";
$table = "shooting_accuracy";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP GET request
$ID = $_GET['a'];
$Shoot = $_GET['b'];
$Goal = $_GET['c'];
$Miss = $_GET['d'];
$Accuracy = $_GET['e'];



// Prepare the SQL statement to insert data into the table

$sql = "INSERT INTO $table (ID, Shoot, Goal, Miss, Accuracy) VALUES ('$ID', '$Shoot', '$Goal',  '$Miss ', '$Accuracy')";

// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>