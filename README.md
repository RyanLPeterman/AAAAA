# Nginx Clone Webserver
A webserver written in C++ using the Boost API. This webserver supports a common
RequestHandler interface/abstract base class that can be used to easily implement
any handler.

**Authors**:
* Ryan Peterman
* Aditya Raju
* Yilei Zhang

## Important Commands
To compile:

``` make ```

To run:

``` ./web-server <config-file> ```

To test:

``` make test ```

This will build and run the unit and integration tests

To get boost (Ubuntu):

``` sudo apt-get install libboost-all-dev ```

## Config File Format
``` bash
# port the webserver is listening on
port <port-num>;

# how to connected a url to a handler
path <url-mapping> <handler-name> {
    <additional parameters as expected by handler>
}

# Handler to be called if no url matches
default <handler-name> {}
```

## Current Handlers
All handlers extend the RequestHandler abstract base class/interface. They implement two virtual functions, Init and HandleRequest.

* EchoHandler - returns the raw request back as a response
* StaticHandler - serves files from configured path (root <absolute path from base dir>; in child block)
* StatusHandler - displays information on the status of webserver (# of requests, types of handlers that exist and for which url prefixes)
* NotFoundHandler - always displays a 404 error when handling a request

## Adding Handlers

To add a handler, you must extend the Handler abstract class. Here is an example of how it should look:

```
class <YourHandler> : public RequestHandler {
public:
    Status Init(const std::string& uri_prefix, const NginxConfig& config);

    Status HandleRequest(const Request& request,
                         Response* response);
}
```


