<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['_method']) && $_POST['_method'] === 'DELETE') {
    if (!isset($_POST['file'])) {
        http_response_code(400);
        echo json_encode(["error" => "No file specified"]);
        exit;
    }

    $fileName = basename($_POST['file']); // Sanitize filename
    $filePath = "../uploads/" . $fileName;

    if (!file_exists($filePath)) {
        http_response_code(404);
        echo json_encode(["error" => "File not found"]);
        exit;
    }

    if (unlink($filePath)) {
        http_response_code(200);
        echo json_encode(["message" => "File deleted successfully"]);
    } else {
        http_response_code(500);
        echo json_encode(["error" => "Failed to delete file"]);
    }
} else {
    http_response_code(405);
    echo json_encode(["error" => "Invalid request method"]);
}
?>
