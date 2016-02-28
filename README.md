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
* [protobuf](https://github.com/iliamo/Consumer-Producer-API)
* [yaml-cpp](https://github.com/named-data/NFD.git)
* [zeromq](http://gstreamer.freedesktop.org/data/pkg/osx/1.4.3/)

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
$ bin/test_window_others 1 3
</pre>

- Terminal 2 -- Playing video & audio
<pre>
$ bin/test_window_others 2 3
</pre>

- Terminal 0 -- Node0(Master) 
<pre>
$ bin/test_window  0 3 1 1
</pre>


License
---
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version, with the additional exemption that compiling, linking, and/or using OpenSSL is allowed.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
