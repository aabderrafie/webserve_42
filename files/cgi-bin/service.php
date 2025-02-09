<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['image'])) {
    $uploadDir = 'uploads/';
    $uploadFile = $uploadDir . basename($_FILES['image']['name']);

    if (move_uploaded_file($_FILES['image']['tmp_name'], $uploadFile)) {
        $image = imagecreatefromjpeg($uploadFile); // Change to imagecreatefrompng() for PNG files
        $cropped = imagecrop($image, ['x' => 50, 'y' => 50, 'width' => 200, 'height' => 200]);
        if ($cropped !== false) {
            imagejpeg($cropped, $uploadDir . 'cropped_' . basename($_FILES['image']['name']));
            imagedestroy($cropped);
            echo 'Image cropped successfully!';
        }
        imagedestroy($image);
    } else {
        echo 'Error uploading file.';
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Image Cropper</title>
</head>
<body>
    <h1>Image Cropper</h1>
    <form method="post" enctype="multipart/form-data">
        <label for="image">Upload Image:</label>
        <input type="file" name="image" required>
        <button type="submit">Crop Image</button>
    </form>
</body>
</html>