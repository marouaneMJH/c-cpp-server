# C HTTP Server

A lightweight HTTP server implementation in C with multithreading support.

## Features

-   Multithreaded request handling
-   HTTP request parsing (GET, POST, PUT, DELETE, PATCH)
-   Query parameter parsing
-   File serving capabilities
-   HTTP/1.1 support

## Project Structure

```
.
├── build/              # Build output directory
│   └── obj/            # Object files
├── docs/               # Documentation
├── include/            # Header files
│   ├── common/         # Common utilities
│   ├── http/           # HTTP handling
│   └── server/         # Server functionality
├── lib/                # Libraries
├── src/                # Source code
│   ├── http/           # HTTP implementation
│   ├── server/         # Server implementation
│   └── main.c          # Entry point
├── test/               # Test files
└── www/                # Web content
    ├── 404.html        # Error page
    └── index.html      # Default page
```

## Building

To build the server:

```bash
make
```

## Running

To run the server:

```bash
make run
```

The server will start on http://localhost:8080 by default.

## Cleaning

To clean the build:

```bash
make clean
```

## License

[MIT](LICENSE)
