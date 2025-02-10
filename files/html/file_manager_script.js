// document.addEventListener("DOMContentLoaded", function () {
//     const fileList = document.getElementById('fileList');

//     function fetchFileList() {
//         fetch('/folder-contents.php')
//             .then(response => response.json())
//             .then(data => {
//                 fileList.innerHTML = ''; // Clear list

//                 data.forEach(fileName => {
//                     const listItem = document.createElement('li');
//                     listItem.textContent = fileName;

//                     const deleteButton = document.createElement('button');
//                     deleteButton.textContent = 'DELETE';
//                     deleteButton.addEventListener('click', () => {
//                         sendDeleteRequest(fileName)
//                             .then(() => fetchFileList()) // Refresh list after delete
//                             .catch(error => console.error('Error deleting file:', error));
//                     });

//                     listItem.appendChild(deleteButton);
//                     fileList.appendChild(listItem);
//                 });
//             })
//             .catch(error => console.error('Error fetching files:', error));
//     }

//     function sendDeleteRequest(fileName) {
//         return fetch(`/delete-file.php?file=${encodeURIComponent(fileName)}`, {
//             method: 'DELETE'
//         }).then(response => {
//             if (!response.ok) {
//                 throw new Error('Failed to delete file');
//             }
//         });
//     }

//     fetchFileList();
// });


//////////////////////////////2///////////////////////////////


// document.addEventListener("DOMContentLoaded", function () {
//     const fileList = document.getElementById('fileList');

//     function fetchFileList() {
//         fetch('/folder-contents.php')
//             .then(response => response.json())
//             .then(data => {
//                 fileList.innerHTML = ''; // Clear list before updating

//                 data.forEach(fileName => {
//                     const listItem = document.createElement('li');
//                     listItem.textContent = fileName;

//                     const deleteButton = document.createElement('button');
//                     deleteButton.textContent = 'DELETE';
//                     deleteButton.addEventListener('click', () => {
//                         sendDeleteRequest(fileName)
//                             .then(() => fetchFileList()) // Refresh file list
//                             .catch(error => console.error('Error deleting file:', error));
//                     });

//                     listItem.appendChild(deleteButton);
//                     fileList.appendChild(listItem);
//                 });
//             })
//             .catch(error => console.error('Error fetching files:', error));
//     }

//     function sendDeleteRequest(fileName) {
//         return fetch(`/delete-file.php`, {
//             method: 'POST', // Using POST with _method=DELETE
//             headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
//             body: `file=${encodeURIComponent(fileName)}&_method=DELETE`
//         }).then(response => {
//             if (!response.ok) throw new Error('Failed to delete file');
//             return response.json(); // Ensure response is processed before reloading
//         }).then(() => {
//             fetchFileList(); // Refresh file list instead of full reload
//         }).catch(error => {
//             console.error('Error deleting file:', error);
//         });
//     }

//     fetchFileList(); // Load file list on page load
// });





/////////////////////////////////////3//////////////////////////////////////////

document.addEventListener("DOMContentLoaded", function () {
    const fileList = document.getElementById('fileList');

    function fetchFileList() {
        fetch('/folder-contents.php')
            .then(response => response.json())
            .then(data => {
                fileList.innerHTML = ''; // Clear list before updating

                data.forEach(fileName => {
                    const listItem = document.createElement('li');
                    listItem.textContent = fileName;

                    const downloadButton = document.createElement('button');
                    downloadButton.textContent = 'Download';
                    downloadButton.addEventListener('click', () => sendGetRequest(fileName));

                    const deleteButton = document.createElement('button');
                    deleteButton.textContent = 'Delete';
                    deleteButton.classList.add('delete-btn');
                    deleteButton.addEventListener('click', () => {
                        sendDeleteRequest(fileName)
                            .then(() => fetchFileList()) // Refresh file list
                            .catch(error => console.error('Error deleting file:', error));
                    });

                    listItem.appendChild(downloadButton);
                    listItem.appendChild(deleteButton);
                    fileList.appendChild(listItem);
                });
            })
            .catch(error => console.error('Error fetching files:', error));
    }

    function sendGetRequest(fileName) {
        const downloadLink = document.createElement('a');
        downloadLink.href = `/files/cgi-bin/uploads/${encodeURIComponent(fileName)}`;
        downloadLink.download = fileName;
        document.body.appendChild(downloadLink);
        downloadLink.click();
        document.body.removeChild(downloadLink);
    }

    function sendDeleteRequest(fileName) {
        return fetch(`/delete-file.php`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `file=${encodeURIComponent(fileName)}&_method=DELETE`
        }).then(response => {
            if (!response.ok) throw new Error('Failed to delete file');
            return response.json();
        }).then(() => {
            fetchFileList(); // Refresh file list instead of full reload
        }).catch(error => {
            console.error('Error deleting file:', error);
        });
    }

    fetchFileList(); // Load file list on page load
});

