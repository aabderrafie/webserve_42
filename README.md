
---

# Webserv  

## Overview  
Webserv is a custom HTTP server built from scratch in **C++98**. The project aims to provide a deep understanding of HTTP protocols, non-blocking I/O, and web server architecture. It allows serving static files, processing HTTP requests, and handling multiple connections simultaneously.  

## Features  

### Mandatory Features  
- **Configuration File Support**: The server takes a configuration file to define its behavior.  
- **Non-blocking I/O**: Uses `poll()` (or equivalent) to handle multiple clients efficiently.  
- **Multiple Clients Handling**: The server can process multiple connections at the same time.  
- **HTTP Methods**: Implements at least **GET**, **POST**, and **DELETE** methods.  
- **Static File Serving**: Supports serving static web pages (HTML, CSS, images, etc.).  
- **Directory Listing**: Optionally displays files in a directory when no default file is found.  
- **File Upload**: Clients can upload files to a specified directory.  
- **Error Pages**: Provides customizable error pages for different HTTP status codes.  
- **Port & Host Configuration**: Allows setting up multiple servers with different host:port configurations.  
- **Signal Handling**: Properly manages system signals to avoid crashes.  

### Bonus Features  
- **CGI Execution**: Supports running CGI scripts (e.g., PHP, Python).  
- **Session Management**: Implements cookies and basic session handling.  
- **Multiple CGI Handlers**: Supports various CGI languages for dynamic content generation.  
- **Load Balancing**: Distributes requests among multiple server instances.  

## Installation & Usage  

1. **Clone the Repository**  
   ```sh
   git clone https://github.com/aabderrafie/webserv.git
   cd webserv
   ```

2. **Compile the Program**  
   ```sh
   make
   ```

3. **Run the Server**  
   ```sh
   ./webserv [config_file]
   ```

4. **Test with a Web Browser**  
   Open a browser and navigate to `http://localhost:8080/` or use `curl`:  
   ```sh
   curl -X GET http://localhost:8080/
   ```

5. **Stop the Server**  
   Press `Ctrl+C` to terminate the process.  

## Configuration  
A sample configuration file should define:  
- Server names and ports.  
- Root directory for serving files.  
- Allowed HTTP methods per route.  
- Default error pages.  
- Directory listing options.  
- CGI execution settings.  

## License  
This project is licensed under the **MIT License**. See the LICENSE file for details.  

---
