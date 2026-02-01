ATLAS
A High-Performance C++ API Gateway & Authentication Service

OVERVIEW

Atlas is a backend-heavy, production-style API service built from scratch in C++.
The project focuses on understanding how real backend systems work internally,
without using high-level frameworks.

This project demonstrates networking, HTTP handling, authentication, rate
limiting, and API correctness at a low level.

Atlas behaves like a mini API Gateway.

KEY FEATURES

Raw TCP server using POSIX sockets

Manual HTTP request parsing and response construction

REST-style API routing

JSON-based responses (representation, not objects)

Token-based authentication using HTTP headers

Rate limiting using Token Bucket algorithm

Correct HTTP status codes and error handling

Stateless backend design

IP-based client identification

WHY THIS PROJECT

Most backend projects rely on frameworks that hide:

socket handling

HTTP internals

authentication boundaries

rate limiting logic

Atlas is built to understand:

how requests reach backend code

where authentication actually lives

how rate limiting protects services

why HTTP is layered on top of TCP

why backends send JSON, not objects

ARCHITECTURE

Request flow:

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
Routing and API Logic
|
JSON Response

Atlas functions like a real API Gateway used in production systems.

AUTHENTICATION

Authentication is token-based.

Clients must send the token in the HTTP header:

Authorization: Bearer secret-token-123

Authentication is checked BEFORE routing logic.

Unauthorized requests return:

401 Unauthorized

RATE LIMITING

Rate limiting is implemented using the Token Bucket algorithm.

Per-IP rate limiting

Example configuration:

5 requests per 10 seconds

If the limit is exceeded, the server responds with:

429 Too Many Requests

This protects the backend from abuse and overload.

API ENDPOINTS

Health Check

GET /health

Response:
200 OK

List Users

GET /users

Response (JSON):
[
{ "id": 1, "name": "Kailash" },
{ "id": 2, "name": "Alex" }
]

Create User

POST /users

Request body (JSON):
{ "name": "Kailash" }

Responses:

201 Created (success)

422 Unprocessable Entity (missing or invalid field)

400 Bad Request (invalid JSON)

Current User (Protected)

GET /users/me

Header required:
Authorization: Bearer secret-token-123

Responses:

200 OK (authenticated)

401 Unauthorized (missing or invalid token)

BUILD AND RUN

Requirements:

Linux or macOS

g++ compiler (C++17)

POSIX sockets

Build command:

g++ src/server.cpp -Iinclude -std=c++17 -o atlas

Run command:

./atlas

Server runs on:

http://localhost:8080

TESTING USING CURL

curl localhost:8080/health

curl localhost:8080/users

curl -X POST localhost:8080/users -d '{"name":"Kailash"}'

curl localhost:8080/users/me

curl localhost:8080/users/me -H "Authorization: Bearer secret-token-123"

DESIGN DECISIONS

TCP is used to understand backend networking fundamentals.

HTTP is implemented manually to expose request lifecycle details.

JSON is used as a language-agnostic representation format.

Authentication and rate limiting are implemented as API gates,
not as part of business logic.