# Webserv: HTTP Server Project

## Overview
Webserv is a custom HTTP server implementation in C++ 98, designed to provide a deep understanding of web server functionality and HTTP protocol intricacies.

## Project Description
This project involves creating a fully functional web server that can:
- Handle HTTP requests
- Serve static websites
- Support multiple server configurations
- Implement non-blocking I/O operations
- Handle CGI (Common Gateway Interface) scripts

## Features
- Supports HTTP 1.1 protocol
- Non-blocking server architecture
- Configurable through a dedicated configuration file
- Supports multiple ports and server configurations
- Implements GET, POST, and DELETE HTTP methods
- CGI script execution
- File upload capabilities
- Custom error page handling

## Requirements
- C++ 98 compiler
- No external libraries (except standard C++ libraries)
- Operating System: Linux, macOS, or Windows with Docker

## Installation

### Linux Users
```bash
git clone https://github.com/aabderrafie/webserve_42.git
cd webserve_42
make
```

### macOS or Windows Users
1. Ensure Docker is installed and running
2. Build the project:
```bash
make docker
```

## Configuration
The server is configured using a configuration file similar to NGINX. You can specify:
- Server ports and hosts
- Server names
- Default error pages
- Client body size limit
- Route configurations
  - Accepted HTTP methods
  - URL redirects
  - File and directory serving
  - Directory listing
  - CGI script execution
  - File upload handling

## Usage
```bash
./webserv [configuration_file]
```
If no configuration file is provided, a default configuration will be used.

## Testing
- Use telnet for basic HTTP testing
- Compare behavior with NGINX
- Implement stress tests to ensure server resilience

## Supported CGI
The server supports at least one CGI implementation:
- PHP-CGI
- Python
- Other CGI-compatible scripting languages

## Bonus Features
- Cookies and session management
- Multiple CGI support

## Development Notes
- Follows C++ 98 standard
- Uses non-blocking I/O with poll() (or equivalent)
- Handles client disconnections gracefully
- Supports multiple simultaneous client connections

## Limitations
- Does not implement the entire HTTP RFC
- Designed for educational purposes

## Contributing
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a pull request

## Acknowledgments
- 42 Network
- HTTP RFC specifications
- NGINX documentation
