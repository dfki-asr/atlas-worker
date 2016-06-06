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

Features
--------

This import worker supports the following formats:

### Collada

In addition to the extent of Collada supported by the underlying Assimp library (which may vary, depending on what version atlas-worker is linked against),
we have added support for Collada references. This means that it is possible to import Collada files that reference other Collada files.
However, only relative references (inside the archive retrieved from atlas-server) will be taken into account. No loading of references over the network will take place.

From the imported Collada documents, only transformation hierarchy, geometry, and materials (including textures) will be transferred to the ATLAS asset.
Other Collada features (animations, joints, etc.) are ignored.

### AutomationML

Since ATLAS concerns itself with 3D geometry, the import worker pays no heed to the non-geometry aspects of AutomationML.
Of the various ways to specify geometry references in AutomationML only a subset is implemented. These are:
"Reference using URI without a fragment, target and ID" (Whitepaper July 2013, Part 3, Annex A, Section A1.6) and
"Reference using URI and fragments without target and ID" (ibidem, Section A.1.2).
Furthermore, Frame attributes (ibidem, Chapter 4) are used to create a transformation hierarchy in the ATLAS asset.
Other features (kinematics, implicit referencing, attachments, etc.) specified by the AutomationML whitepaper, are ignored.

Once a Collada reference has been encountered, it is imported as per the previous paragraph on Collada files.
AutomationML (and their referenced Collada) hierarchies are converted from AutomationML's Z-Up coordinate system to ATLAS' Y-Up.


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

### Required dependencies

* Boost >= 1.49 (1.55 if you want to build with clang)
* POCO  >= 1.4.6
* ActiveMQ-CPP >= 3.6.0
* Assimp >= 3.1.1
* Jansson >= 2.2.1

If you want to import AutomationML, we recommend using Assimp version 3.2 or later. We've contributed code to assimp that enables it to properly load trifans and tristrips, which AutomationML COLLADA files seem to make heavy use of.

#### OS-Provided dependencies

If you're lucky and there is a package management system for your operating system, then you should be able to get most, if not all, of the dependencies from there. We've made good experiences with Homebrew on MacOS and APT on Linux. Here's the packages you need:

* Homebrew: `brew install jansson activemq-cpp assimp boost poco`
 * if you want the trifan code mentioned above, you can `brew install --HEAD assimp`
 * `activemq-cpp` writes a wrong `pkg-config` file (reported and fixed upstream in version 3.8.5).
    * For a fix, edit `/usr/local/lib/pkgconfig/activemq-cpp.pc` to say `Requires.private: apr-1`

* APT: `apt-get install libassimp-dev libboost-dev libjansson-dev`
 * ActiveMQ-CPP is not currently in Debian's APT, you will need to build it yourself. Therefore you need the `libapr1-dev` package.
 * Poco from Debian's APT is too old, you will also need to build it yourself. (Or install the package from the `experimental` repo.)
 * For both of the above dependencies, you can use the scripts we use to install them on the Travis CI infrastructure. See the `.travis` directory.

#### On Windows platforms (Win 8, Visual Studio 2013)

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

### Performing the build

It is recommended to perform the build in a separate directory, e.g. a `build` folder.
CMake recommends to keep this folder beside the source tree (as to avoid accidental commits of binaries); but it should also work to have it as a subfolder to this repository, in which case you need to be careful when committing changes.

#### On UNIX systems (Linux, OSX)

    mkdir ../atlas-worker-build
    cd ../atlas-worker-build
    cmake ../atlas-worker
    make

#### On Windows

* Open CMake GUI
* Select the repository (`atlas-worker`) as source directory.
* Select an output folder. (Again, we recommend one next to the source repository.)
* Click configure. Select your compile environement.
* Optional: Specify dependency paths, if you did not use the `contrib` layout above.
* Click generate.
* Open the generated solution and run.


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
    GetGitRevisionDescription.cmake - Boost License

