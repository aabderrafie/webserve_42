<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = htmlspecialchars($_POST['username']);
    $message = htmlspecialchars($_POST['message']);
    $file = 'chat.txt';

    // Format the message
    $chatMessage = "<p><strong>$username:</strong> $message</p>\n";

    // Append the message to the file
    file_put_contents($file, $chatMessage, FILE_APPEND);
}
?>