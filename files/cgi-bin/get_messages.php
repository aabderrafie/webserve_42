<?php
$file = 'chat.txt';
if (file_exists($file)) {
    echo file_get_contents($file);
} else {
    echo "<p>No messages yet. Start the conversation!</p>";
}
?>