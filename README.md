# orange-tcp

todo

## Getting started

First time:
```
$ docker build -t orange-tcp:ubuntu20.10 .
```

Then run `setup.sh`.

To build:
```
./build.sh <target>
```

## Testing ARP

First, `docker run ...` to create another container; it will automatically be assigned its own ip address. We'll use multiple containers to create our simulated network.

* `ifconfig` on machine A to check its IP and MAC addresses.
* `arping -c 2 172.17.0.3 -vv` on machine B
* `arp -e` on machine B to check arp cache.

Now, to test our own ARP implementation, we'll want to disable the kernel from
responding to our ARP requests. Can do this with: `ifconfig eth0 -arp`.

Then, on the server:
```
$ ./arping --server --dump_arp
```

And on the client:
```
$ ./arping --ip 172.17.0.3 --num_requests 2
```
