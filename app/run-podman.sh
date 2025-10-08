podman run -it --rm \
    -v /dev/shm:/dev/shm:rw  \
    ipc \
    bash -c "./producer && tail -f /dev/null"

podman run -it --rm \
    -v /dev/shm:/dev/shm:rw  \
    ipc \
    bash -c "./consumer && tail -f /dev/null"