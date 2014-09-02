crossdocs
=========

Repository for the development of the CrossDocs GUI utility.

The whole application is still in a __VERY INITIAL STAGE__. Don't expect it to be doing anything interesting by now. 

Everything here is strongly based on Qt. Clone the repo, grab __Qt5__ or higher and compile it, in a a _cmake_ style:

    $ cd crossdocs/
    $ mkdir build
    $ cd build
    $ qmake .. 
    $ make      # I had some trouble with the -j* flag, so avoid it
