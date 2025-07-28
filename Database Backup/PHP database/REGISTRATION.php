<?php
// MySQL database configuration
$servername = "localhost";
$username = "root";
$password = "";
$database = "sportscience";
$table = "user";

// Establish a connection to the MySQL server
$conn = new mysqli($servername, $username, $password, $database);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve the data from the HTTP request
$uid = $_REQUEST['uid'];
$name = $_REQUEST['name'];
$email = $_REQUEST['email'];
$whatsapp = $_REQUEST['whatsapp'];
$dob = $_REQUEST['dob'];

// Prepare the SQL statement to insert data into the table

$sql = "INSERT INTO $table (ID, PlayerName, DoB, Email, Whatsapp) VALUES ('$uid', '$name', '$dob', '$email', '$whatsapp')";


// Execute the SQL statement
if ($conn->query($sql) === TRUE) {
    echo "Data inserted successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the database connection
$conn->close();
?>