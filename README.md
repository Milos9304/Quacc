# Quacc - the Quest Accelerator for XACC
A plugin for Xacc that implements the QuEST accelerator.

Installation
------------
With the XACC framework installed, run the following
```bash
$ cd build
$ cmake .. -DXACC_DIR=$HOME/.xacc (or wherever you installed XACC)
$ make install
```

To use the QuEST simulator in Xacc just use
```
auto qpu = xacc::getAccelerator("quest"));
```
Tests
-------------
After installation run

```bash
$ cd build
$ ctest
```
or for more detailed output, run tests manually by

```bash
$ cd build
$ tests/[test_name]
```
Work in progress for creating more sophisticated tests.

QuEST Configuration
-------------
QuEST cmake file is located in **quacc/visitors/quest-default/QuEST** and configurations can be done as described in [link](https://github.com/QuEST-Kit/QuEST)

Documentation
-------------
Work in progress.

Questions, Bug Reporting, and Issue Tracking
--------------------------------------------
Use GitHub interface.

License
-------
Quacc is licensed by MIT License.

Credentials
----------
This project is a modification of TNQVM [link](https://github.com/ORNL-QCI/tnqvm) from which we adopted the main code skeleton.
