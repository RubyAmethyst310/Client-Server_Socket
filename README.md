# Socket Programming Project

This project implements a client-server application using **TCP/IP socket programming**.  
The server hosts a directory of files, while clients can connect, request, and download files.  
The communication between the client and server follows a custom protocol designed to ensure  
successful data transmission, priority handling, and multi-file download support.  

The goal of the project is to practice **network programming concepts**, gain experience with  
socket communication, and demonstrate robust error handling, concurrency, and protocol design.

---

## Authors
- **23127052 – Duong Gia Huy**
- **23127125 – Nguyen Hieu Thuan**

---

## Project Structure
- **Server**: Contains the server-side source code responsible for handling multiple client connections, file requests, and file transfer.  
- **Client**: Contains the client-side source code that connects to the server, sends file requests, and downloads requested files.  
- **Input File (`input.txt`)**: A client-side file listing the filenames to be downloaded from the server.  
- **Output Folder**: The directory where downloaded files are saved on the client side.  
- **Documentation**: Includes the report detailing system design, communication protocols, and instructions.

---

## Focus of the Project
This project emphasizes the following **key features and concepts**:

- **Socket Communication over TCP/IP**  
  A custom protocol defines message formats, data types, and acknowledgment steps between server and client.  

- **Single and Multi-File Download**  
  Clients can download one or multiple files simultaneously, ensuring correctness and completeness.  

- **Priority Handling**  
  File downloads are prioritized as `CRITICAL`, `HIGH`, or `NORMAL` based on user settings.  

- **Progress Display**  
  The client shows download progress (in %) for each file.  

- **Concurrency and Robustness**  
  Multiple clients can connect to the server, and the communication protocol ensures reliable transfers.  

---

## How to Run

### 1. Get the Server IP Address
On the server machine, open **Command Prompt** and run:
  ```bash
  ipconfig
  ```
Copy the IPv4 Address to use in the client setup.

### 2. Run the Server
Open the server solution file (.sln) in Visual Studio.
Compile and run the server executable.

### 3. Run the Client
Open the client solution file (.sln) in Visual Studio.
Enter the server’s IPv4 address when prompted.
Compile and run the client executable.

### 4. Request Files
Open input.txt in the client folder.
Add filenames to request (one filename per line).
Ensure there is an output folder in the client directory to store downloaded files.

### 5. Download and Exit
The client will automatically download listed files.
Progress will be displayed in the console.
To exit, press Ctrl+C.
