<?php
// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Retrieve the data from the POST request
    $email = $_POST['email'] ?? '';
    $password = $_POST['password'] ?? '';

    // Validate that both fields are filled
    if (empty($email) || empty($password)) {
        echo json_encode(['success' => false, 'message' => 'Email and password are required.']);
        exit;
    }

    // Specify the file name
    $filename = '../credentials.txt';

    // Check if the file exists
    if (!file_exists($filename)) {
        echo json_encode(['success' => false, 'message' => 'User data file not found.']);
        exit;
    }

    // Open the file for reading
    $file = fopen($filename, 'r');

    if ($file) {
        $userFound = false;
        $loginSuccess = false;

        // Read the file line by line
        while (($line = fgets($file)) !== false) {
            // Parse the line into its components
            $line = trim($line);
            if (empty($line)) {
                continue; // Skip empty lines
            }

            // Split the line into parts
            $parts = explode(', ', $line);

            // Extract SessionID, Email, and Password
            $sessionID = str_replace('SessionID: ', '', $parts[0]);
            $storedEmail = str_replace('Email: ', '', $parts[1]);
            $storedPassword = str_replace('Password: ', '', $parts[2]);

            // Check if the email matches
            if ($storedEmail === $email) {
                $userFound = true;

                // Check if the password matches
                if ($storedPassword === $password) {
                    $loginSuccess = true;
                    break;
                }
            }
        }

        // Close the file
        fclose($file);

        // Handle login success or failure
        if ($userFound && $loginSuccess) {
            echo json_encode(['success' => true, 'email' => $email]);
        } elseif ($userFound) {
            echo json_encode(['success' => false, 'message' => 'Wrong password.']);
        } else {
            echo json_encode(['success' => false, 'message' => 'Unknown user.']);
        }
    } else {
        echo json_encode(['success' => false, 'message' => 'Failed to open the file.']);
    }
} else {
    echo json_encode(['success' => false, 'message' => 'Invalid request method.']);
}
?>