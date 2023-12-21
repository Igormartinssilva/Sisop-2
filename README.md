# Welcome to Y

Y is a local network messaging application that enables users to send messages to their followers. This repository provides both the server and client components necessary to run the application. Follow the instructions below to set up and use Y on your devices.

## Application Usage

1. Clone the repository:

    ```bash
    git clone https://github.com/igormartinssilva/Sisop-2.git
    cd Sisop-2
    ./build.sh
    ```

    If you encounter a permission denied error, execute the following commands:

    ```bash
    chmod +x bin/server
    chmod +x bin/client_executable
    chmod +x start_server.sh
    chmod +x start_client.sh
    chmod +x build.sh
    ```

2. Start the server:

    ```bash
    ./start_server.sh
    ```

3. Start the client on other devices:

    ```bash
    ./start_client.sh <server_ip_address>
    ```

### Possible Errors:

If you encounter any issues, refer to the following solutions:

- **Permission Denied Error:**
    Execute the following commands to grant necessary permissions:

    ```bash
    chmod +x bin/server
    chmod +x bin/client_executable
    chmod +x start_server.sh
    chmod +x start_client.sh
    chmod +x build.sh
    ```

For more detailed information, refer to the [complete documentation](https://docs.google.com/document/d/1vqt1WpEX4UF9TbzQ4E9LubHF81-lHciaTV9i7U5GoCM/edit).
