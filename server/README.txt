int server.cpp make call to process request:
1. a request structure is created with the data
2. the requested file is searched for
3. if requested file is found in acessible directory
--construct response structure
--send response with file contents
-- else send 404 response with html file for 404
