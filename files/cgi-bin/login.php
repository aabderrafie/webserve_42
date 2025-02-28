<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {    
    $email = $_POST['email'] ?? '';
    $password = $_POST['password'] ?? '';
    if (empty($email) || empty($password)) {
        echo json_encode(['success' => false, 'message' => 'Email and password are required.']);
        exit;
    }
    $filename = '../../credentials.txt';
    if (!file_exists($filename)) {
        echo json_encode(['success' => false, 'message' => 'User data file not found.']);
        exit;
    }
    $file = fopen($filename, 'r');
    if ($file) {
        $userFound = false;
        $loginSuccess = false;
        while (($line = fgets($file)) !== false) {
            $line = trim($line);
            if (empty($line)) {
                continue; 
            }
            $parts = explode(', ', $line);            
            $sessionID = str_replace('SessionID: ', '', $parts[0]);
            $storedEmail = str_replace('Email: ', '', $parts[1]);
            $storedPassword = str_replace('Password: ', '', $parts[2]);
            if ($storedEmail === $email) {
                $userFound = true;
                if ($storedPassword === $password) {
                    $loginSuccess = true;
                    break;
                }
            }
        }
        fclose($file);
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