PurePaxos
----

PurePaxos is the very basic version of Multi-Paxos implementation. It also inculdes some simple testing of functionality and performance.  

For license information see LICENSE.

Structure
----

The directory structure is as follows:

* **/root**
    * **libpaxos/** *-- PurePaxos library source code*
    * **waf** *-- binary waf file*
    * **wscript** *-- waf instruction file*
    * **config/** *-- config files of this project* 
    * **waf-tools/** *-- additional waf tools*
    * **loli_test/** *-- test code*
    * **libzfec/** *-- zefc library using for RS code*
    * **script/** *-- some python scripts*
    * **LICENSE**
    * **README.md**
    * **INSTALL.md**

Building instructions
----
Prerequisites
==
These are prerequisites to build PurePaxos.

**Required:**
* [boost](http://www.boost.org/)
* [protobuf](https://developers.google.com/protocol-buffers/)
* [yaml-cpp](http://yaml.org/)
* [zeromq](http://zeromq.org/)

Prerequisites build instructions
==

Mac OS build considerations (Ubuntu or CentOS Needed)
-

boost
--
brew install boost

protobuf
--
brew install protobuf

zeromq
--
brew install zeromq

yaml-cpp
--
brew install libyaml


Linux(Ubuntu) build considerations (Ubuntu or CentOS Needed)

clang
--
sudo apt-get install clang 

boost
--
sudo apt-get install libboost-all-dev

protobuf
--
sudo apt-get install libprotobuf-dev protobuf-compiler python-protobuf 

zeromq
--
sudo apt-get install libzmq3-dev python-zmq

yaml-cpp
--
sudo apt-get install libyaml-cpp-dev 


Build instructions
==
<pre>
$ ./waf configure -l info
$ ./waf
</pre>


Run Test
--


- Terminal 1 -- Node1 
<pre>
$ bin/servant 1 3
(node_id node_num)
</pre>

- Terminal 2 -- Node2
<pre>
$ bin/servant 2 3
(ndoe_id node_num)
</pre>

- Terminal 0 -- Node0(Master) 
<pre>
$ bin/master 0 3 1 1 200000
(ndoe_id node_num value_size win_size total_num)
</pre>


License
---
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version, with the additional exemption that compiling, linking, and/or using OpenSSL is allowed.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
