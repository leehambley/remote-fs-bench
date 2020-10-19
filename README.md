# Filesystem "benchmark" for remote filesystems

This is my naïve attempt at some performance benchmarking for filesystems
across "shared" boundaries.

The use-case is checking for raw performance and latency on file-systems
commonly used for development (e.g Docker).

## Design

The design uses a simple `client.c` which is compiled with GCC (see the
Dockerfile) using fanotify. This requires Linux above 5.1 to benefit from the
"super block root watch" in quickly checking for filesystem events across an
entire filesystem.

The container should be started with a working directory shared with the host,
and an exposed port.

There are two suites of benchmarks:

  - Raw filesystem performance (open, read, write, stat, lstat and friends).
  - Latency (host changes reflected on the container, if at all)

## Performance Test

For checking the performance of the host. 10,000 files are generates on teh
host (before the server sends "ready\0", see below). The files are generated
between 1..15 levels deep in order to simiulate directory tree as may be found
in a software project.

## Socket Commands

Server to Client:

  - "ready\0" - indicates that filesystem set-up is done, and client should
    begin to report events over the socket

Client to Server:

  - "_____?" - TBD, client should "forward" fanotify events to server

## Filesystem events on the host

To be fair, the "server" (running on the host machine) also uses FS events,
that gives us three moving pieces:

 - fanotify (or similar) on the host
 - fanotify on the container (always Linux)
 - a loop modifying the filesytem on the host

fanotify on the host then records the event timestamp once an event is
observed, and waits for the same from the container. This helps give an
impression of the latency.

