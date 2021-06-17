# Loonix Rootkit

## Inserting:
Compiling:
```
sudo make all
```

Starting (Usually this would be done with an exploit):
```
sudo make start
```

Removing:
```
sudo make stop
```
>The ability to remove without a reboot can be removed by deleting the ``__exit`` parameter in ``escape_mod_exit``

## Usage:
Upon insertion, two procfiles will be created:
``/proc/root``, ``/proc/shell`` and ``/proc/escape``.
> ``/proc/shell`` has not been implemented in this version

Calling ``open`` on ``/proc/root`` will elevate the privledges of the calling process to root.

Calling ``open`` on ``/proc/escape``will replace the calling processes namespace with pid 1s namespace, effectively breaking it out of any container it was in.
