http://dotnet.github.io/orleans/1.5/Tutorials/index.html

Front Ends for Orleans Services
Exposing silo gateway ports as public endpoints of an Orleans cluster is not recommended. Instead, Orleans is intended to be fronted by your own API.

Creating an HTTP API, or web application is a common scenario. Let's extend the Employee/Manager scenario from the Declarative-Persistence walk-through to see what steps are required to publish grain data over HTTP.