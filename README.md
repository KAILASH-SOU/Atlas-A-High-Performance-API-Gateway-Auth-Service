Atlas
A High-Performance C++ API Gateway & Authentication Service

Atlas is a backend-heavy, production-style API service built from scratch in C++, without frameworks.
The goal of this project is to deeply understand how real backend systems work internally — networking, HTTP, authentication, rate limiting, and API correctness.

This project intentionally avoids high-level frameworks to expose the actual backend mechanics used in industry systems.

🚀 Key Features

Raw TCP server using POSIX sockets

Manual HTTP request parsing & response construction

REST-style API routing

JSON serialization (representation, not objects)

Token-based authentication (Authorization header)

Rate limiting using Token Bucket algorithm

Correct HTTP status codes & error handling

Stateless backend design

IP-based client identification

🧠 Why This Project?

Most backend projects rely on frameworks that hide:

socket handling

request lifecycle

concurrency boundaries

API control logic

Atlas is built to answer fundamental backend questions:

What happens when a request hits a server?

Where does authentication actually live?

How does rate limiting protect services?

Why HTTP is layered over TCP

Why backend sends JSON, not objects

🏗️ Architecture Overview
Client
  |
TCP Socket
  |
HTTP Parsing
  |
Header Parsing
  |
Authentication Gate
  |
Rate Limiting Gate
  |
Routing & API Logic
  |
JSON Response


Atlas behaves like a mini API Gateway, similar to what exists in real production systems.

🔐 Authentication Model

Token-based authentication

Token passed via HTTP header:

Authorization: Bearer secret-token-123


Authentication is enforced before routing logic

Unauthorized requests return:

401 Unauthorized


This models how real backend services protect APIs.

🚦 Rate Limiting

Implemented using Token Bucket algorithm

Per-IP rate limiting

Example configuration:

5 requests per 10 seconds

Excess requests return:

429 Too Many Requests


This demonstrates backend self-protection and fairness.

📡 API Endpoints
Health Check
GET /health


Response:

200 OK

List Users
GET /users


Response:

[
  { "id": 1, "name": "Kailash" },
  { "id": 2, "name": "Alex" }
]

Create User
POST /users


Request:

{ "name": "Kailash" }


Responses:

201 Created → success

422 Unprocessable Entity → missing/invalid input

400 Bad Request → invalid JSON

Current User (Protected)
GET /users/me


Requires:

Authorization: Bearer secret-token-123


Responses:

200 OK → authenticated

401 Unauthorized → missing/invalid token

🛠️ Build & Run
Requirements

Linux / macOS

g++ (C++17)

POSIX sockets

Build
g++ src/server.cpp -Iinclude -std=c++17 -o atlas

Run
./atlas


Server starts on:

http://localhost:8080

🧪 Testing (curl)
curl localhost:8080/health
curl localhost:8080/users
curl -X POST localhost:8080/users -d '{"name":"Kailash"}'
curl localhost:8080/users/me
curl localhost:8080/users/me -H "Authorization: Bearer secret-token-123"
