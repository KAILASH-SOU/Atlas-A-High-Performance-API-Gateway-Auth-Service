# Atlas
## High-Performance C++ API Gateway & Authentication Service

---

**Atlas** is a backend-heavy, production-style API service built **from scratch in C++**, without using frameworks.

The goal of this project is to deeply understand **how real backend systems work internally** — networking, HTTP, authentication, rate limiting, and API correctness.

This project intentionally avoids high-level frameworks to expose the **actual backend mechanics** used in industry systems.

---

##  Highlights

- Built **without frameworks**
- Focused on **backend internals**, not UI
- Designed like a **mini API Gateway**
- Strong fit for **backend / systems interviews**

---

##  Key Features

- Raw TCP server using POSIX sockets  
- Manual HTTP request parsing & response construction  
- REST-style API routing  
- JSON serialization (representation, not objects)  
- Token-based authentication via `Authorization` header  
- Rate limiting using Token Bucket algorithm  
- Correct HTTP status codes & error handling  
- Stateless backend design  
- IP-based client identification  

---

##  Why This Project?

Most backend projects rely on frameworks that hide critical details like:

- socket handling  
- request lifecycle  
- concurrency boundaries  
- API control logic  

**Atlas is built to answer fundamental backend questions:**

- What actually happens when a request hits a server?
- Where does authentication really live?
- How does rate limiting protect backend services?
- Why is HTTP layered on top of TCP?
- Why do backends send JSON instead of objects?

---

##  Architecture Overview

Client
↓
TCP Socket
↓
HTTP Parsing
↓
Header Parsing
↓
Authentication Gate
↓
Rate Limiting Gate
↓
Routing & API Logic
↓
JSON Response

Atlas behaves like a **mini API Gateway**, similar to components used in real production systems.

---

##  Authentication

- Token-based authentication
- Token is passed via HTTP header:

Authorization: Bearer secret-token-123

- Authentication is enforced **before routing logic**
- Unauthorized requests return:

401 Unauthorized


This mirrors how real backend services protect APIs.

---

##  Rate Limiting

- Implemented using the **Token Bucket algorithm**
- **Per-IP rate limiting**
- Example configuration:

5 requests per 10 seconds

- Requests beyond the limit return:

429 Too Many Requests


This demonstrates **backend self-protection and fairness**.

---

##  API Endpoints

### Health Check
GET /health

makefile
Copy code

Response:
200 OK

yaml
Copy code

---

### List Users
GET /users

