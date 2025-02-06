#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");
// phpinfo();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP CGI Response</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 20px;
            padding: 20px;
            text-align: center;
        }
        .container {
            max-width: 600px;
            margin: auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #333;
        }
        p {
            font-size: 18px;
            color: #555;
        }
        a {
            color: #007BFF;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
        .success {
            color: green;
            font-weight: bold;
        }
        .error {
            color: red;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
        <?php
        if ($_SERVER["REQUEST_METHOD"] == "GET") {
            $name = isset($_GET["name"]) ? htmlspecialchars($_GET["name"]) : "Unknown";
            $age = isset($_GET["age"]) ? htmlspecialchars($_GET["age"]) : "Unknown";
            echo "<h1>Received GET request</h1>";
            echo "<p>Name: <strong>$name</strong></p>";
            echo "<p>Age: <strong>$age</strong></p>";
        }
        elseif ($_SERVER["REQUEST_METHOD"] == "POST") {
            $name = isset($_POST["name"]) ? htmlspecialchars($_POST["name"]) : "Unknown";
            $age = isset($_POST["age"]) ? htmlspecialchars($_POST["age"]) : "Unknown";
            echo "<h1>Received POST request</h1>";
            echo "<p>Name: <strong>$name</strong></p>";
            echo "<p>Age: <strong>$age</strong></p>";
            
            if (isset($_FILES["file"])) {
                $upload_dir = "uploads/";
                $filename = basename($_FILES["file"]["name"]);
                $upload_path = $upload_dir . $filename;
                
                // Debugging information
                // echo "<pre>";
                // echo "File Info:\n";
                // print_r($_FILES["file"]);
                // echo "</pre>";

                if ($_FILES["file"]["error"] == UPLOAD_ERR_OK) {
                    if (move_uploaded_file($_FILES["file"]["tmp_name"], $upload_path)) {
                        echo "<p class='success'>File uploaded successfully: <a href='$upload_path'>$filename</a></p>";
                    } else {
                        echo "<p class='error'>File upload failed! Could not move file.</p>";
                    }
                } else {
                    echo "<p class='error'>File upload failed! Error code: " . $_FILES["file"]["error"] . "</p>";
                }
            } else {
                echo "<p class='error'>No file uploaded!</p>";
            }
        }
        ?>
    </div>
</body>
</html>