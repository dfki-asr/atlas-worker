ATLAS (import worker)
=====================

ATLAS (which stands for "Advanced Three-dimensional Large-scale Asset Server") is a system for
storage and conversion of polygonal 3D models. It offers a REST interface to add and retrieve
models, where HTTP content negotiation is used to determine the input and output format(s).

Its current primary use is to import 3D models into its own storage format, and export them
to something usable on the web.


Architecture
------------

Here is a brief overview diagram of ATLAS' architecture: 

		  +--------+    +-------------+     +---------+
		  |        |    |             |     |         |
	------> Upload +---->             +----->  Export +----->
		  |        |    |             |     |         |
		  +--------+    |             |     +----^----+
						|   Storage   |          |
		  +--------+    |             |     +- - + - -+
		  |        |    |             |     |         |      
		  | Import <---->             +-----> Filter
		  |        |    |             |     |         |
		  +--------+    +-------------+     +- - - - -+

The Upload and Export steps are handled by a Java Enterprise web application. The rationale behind
this is that web services and serialization are very easy to handle in Java through well-standardized
interfaces (JAX-RS, JAXB, etc.), and that it should be easy to add export processing. For the latter
aspect Java fit well, because it is a well-known language with a large ecosystem to draw from.

The web application communicates (via JMS/STOMP) with an import worker application, written in C++.
The worker places 3D models in ATLAS' internal data format into the storage via another REST interface. 
The import part lives separated from the web application since many (especially proprietary) 3D data
formats have SDKs or libraries available with a C/C++ interface. Thus, integration is much easier into
the worker application which is not constrained (with respect to temporary files, for example) 
by running inside a JavaEE container.


Working principle
-----------------

The worker connects to a STOMP queue of the container where the atlas web application resides.
Upon receipt of a message from the queue (a request to import an asset), it downloads the uploaded asset from the temporary storage, unpacks it locally (maybe does some preprocessing, in case of AutomationML) and asks assimp to import (parts of) the asset.
When the asset's 3D model data have been imported, the worker reads assimp's internal representation, converts it to ATLAS' internal format, and uploads it to ATLAS' storage.
After that, completion of the import operation (or eventual failures) are reported to another STOMP queue, on whose other end the web application listens for these status reports.


Repository structure
--------------------

This repository houses the C++ import worker for ATLAS.

    src/AssimpWorker/
        connection - remote interface classes: HTTP convenience classes and STOMP
        import     - interface to assimp
        internal   - code specific to the worker, such as command line parsing
        model      - implementation for ATLAS' data structure classes
        storage    - interface to ATLAS' storage
        worker     - classes for coordinating an import operation
    include/atlas
        model      - headers for ATLAS' internal data structures


Building
--------

The worker uses CMake as a build system. Hopefully the hardest part about building will therfore be organizing all the dependency libraries that are used to build the import worker.

### Dependencies

* Boost >= 1.55
* POCO  >= 1.4.6
* ActiveMQ-CPP >= 3.6.0
* Assimp >= 3.1.1
* Jansson

If you want to import AutomationML, we recommend building Assimp yourself. We've contributed code to assimp that enables it to properly load trifans and tristrips, which AutomationML COLLADA files seem to make heavy use of.

### OS-Provided dependencies

If you're lucky and there is a package management system for your operating system, then you should be able to get all of the dependencies from there. We've made good experiences with Homebrew on MacOS and APT on Linux. Here's the packages you need:

* Homebrew: `brew install jansson activemq-cpp assimp boost poco`
 * if you want the trifan code mentioned above, you can `brew install --HEAD assimp`
 * `activemq-cpp` (up to version 3.8.5) writes a wrong `pkg-config` file, which may result in CMake not finding it.
* APT: `apt-get install libassimp-dev libboost-dev libjansson-dev`
 * ActiveMQ-CPP is not currently in Debian's APT, you will need to build it yourself. Therefore you need the `libapr1-dev` package.
 * Poco from Debian's APT is too old, you will also need to build it yourself. (Or install the package from the `experimental` repo.)
 * For both of the above dependencies, you can use the scripts we use to install them on the Travis CI infrastructure. See the `.travis` directory.

### On Windows platforms (Win 8, Visual Studio 2013)

The following is for MSVC 12 (aka Visual Studio 13) on Windows 8 64bit. On any other Compiler or Windows, you are alone, sorry.

Download the dependencies, either as pre-build binaries or as source and if necessary build them.
Place the binaries in a folder resembling this structure:

	/worker repository
	/contrib/
		x64_win_msvc_120
			activemq-cpp
				bin
				include
				lib
			assimp
				bin
				include
				lib
			Boost
				...
			Jansson
				...
			poco
				...

Alternatively specify all the paths while generating CMake. Run CMake and generate the Visual Studio solution. Open the solution and run.


Running
-------

To run the worker, you have to tell it how to connect to the container. You can do this by setting the options using the command line, or a configuration file. (The latter will overwrite the former as of now.)
Most of the default values fit, if your WildFly's HTTP is running on port 8080 and STOMP connections can be made to port 61614 (as detailed in the CONTAINER.md file in the web application's repository).


Contributing
------------

Contributions are very welcome. We're using the well-known [git branching model](http://nvie.com/posts/a-successful-git-branching-model/), supported by [git flow](https://github.com/nvie/gitflow). We recommend reading the [git flow cheatsheet](http://danielkummer.github.io/git-flow-cheatsheet/).
Please use the github workflow (i.e., pull requests) to get your features and/or fixes to our attention.


License
-------

Our code (and documentation) is licensed under the Apache License version 2.0. You should have received an approriate LICENSE.txt file with this distribution. At any rate, you can get the official license text from http://www.apache.org/licenses/LICENSE-2.0.txt

### Third party contents ###

Our source distribution includes the following third party items with respective licenses:

    LibFindMacros.cmake   - Public Domain
    FindCXX11.cmake       - CC-BY-SA 3.0 (by martiert, from http://stackoverflow.com/a/13571695)
    FindJansson.cmake     - New BSD License
    FindPoco.cmake        - Boost license (from Poco "contrib/camke/PocoConfig.cmake" @6b36a1)

